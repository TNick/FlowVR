/******* COPYRIGHT ************************************************
*                                                                 *
*                         FlowVR Render                           *
*                   Parallel Rendering Modules                    *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
*  ALL RIGHTS RESERVED.                                           *
*                                                                 *
* This source is covered by the GNU GPL, please refer to the      *
* COPYING file for further information.                           *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Clement Menier.                                              *
*                                                                 *
*******************************************************************
*                                                                 *
* File: ./src/renderer/cg/cgengine.cpp                            *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include "flowvr/render/cg/cgengine.h"
#include "flowvr/render/chunkrenderreader.inl"
#include <flowvr/render/chunkrenderwriter.h>

#include <iostream>
#include <string>
#include <set>


#ifndef NODEVIL
#include <IL/il.h>
#include <IL/ilu.h>
#if IL_VERSION >= 175
typedef void ILvoid;
static ILboolean IsInit;
#else
extern ILboolean IsInit;
#endif
#endif

#define _ONCE(A) { static bool b = false; if(!b) { A;  b = true; } };
#define _PRINTMAT(A) { if(dump) { printMatrix(#A, A); } }

namespace flowvr { namespace render { namespace cg {
	/**
	 * @brief render behavior for texture targets, wrapped in a scene subclass.
	 */
	class TextureTargetScene : public flowvr::render::BaseEngine<flowvr::render::cg::CgData>::Scene
	{
	public:
		/**
		 * @param _tg target-group id (1..TG_MAX)
		 * @param parent the CgEngine that uses this target group.
		 */
		TextureTargetScene( flowvr::render::cg::CgEngine *parent, flowvr::ID _tg )
		: m_parent(parent)
		, m_tg(_tg)
		{
		}

		virtual bool preRender(float &aspectRatio, int vp[4], int lastIt, bool &setVp )
		{
			return m_parent->beginRenderToTexture( this, normalizedId(), aspectRatio, vp, lastIt, setVp );
		}

		virtual void postRender()
		{
			m_parent->endRenderToTexture(this, normalizedId());
		}

		int normalizedId() const
		{
			return flowvr::render::TargetGroups::tg2TxTp(m_tg);
		}

		virtual void dump()
		{
			std::cout << "[TextureTargetScene] with tid = ["
					  << m_tg
					  << "] (normalized: "
					  << normalizedId()
					  << ")" << std::endl;
		}

		flowvr::render::cg::CgEngine *m_parent;
		flowvr::ID        m_tg;
	};

}}}

namespace
{

	void glSetBool(GLenum id, bool val)
	{
	 if (val)
		glEnable(id);
	 else
		glDisable(id);
	}

	void setGLStateBool( const flowvr::render::cg::CgEngine::StateGL &state )
	{
		if( state.glid )
			glSetBool( state.glid, state.val.b );
	}

	void setGLStateColorMask( const flowvr::render::cg::CgEngine::StateGL &state )
	{
		int i = state.val.i;
		glColorMask((i==1 || i==2)?GL_TRUE:GL_FALSE,
			(i==1 || i==3)?GL_TRUE:GL_FALSE,
			(i==1 || i==4)?GL_TRUE:GL_FALSE,
			(i==1 || i==5)?GL_TRUE:GL_FALSE);
	}

	void setGLStateSetThreshold( const flowvr::render::cg::CgEngine::StateGL &state )
	{
		float f = state.val.f;
		glSetBool(state.glid,f > 0);
		if (f > 0)
		  glAlphaFunc(GL_GEQUAL,f);
	}

	void setGLStateDepthMask( const flowvr::render::cg::CgEngine::StateGL &state )
	{
		glDepthMask(state.val.b ? GL_TRUE:GL_FALSE );
	}

	void setGLStateCullFace( const flowvr::render::cg::CgEngine::StateGL &state )
	{
		glSetBool(GL_CULL_FACE, state.val.i != 0);
		glCullFace(state.val.i<0 ? GL_FRONT : GL_BACK);
	}

	void setGLStateDepthFunc( const flowvr::render::cg::CgEngine::StateGL &state )
	{
		glDepthFunc(state.val.i);
	}


	void setGLStateBlendFunc( const flowvr::render::cg::CgEngine::StateGL &state )
	{
		switch (state.val.i)
		{
		case flowvr::render::BLEND_ADD:
		  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		  glBlendEquation(GL_FUNC_ADD);
		  break;
		case flowvr::render::BLEND_SUB:
		  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		  glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
		  break;
		case flowvr::render::BLEND_MULT:
		  glBlendFunc(GL_DST_COLOR, GL_ZERO);
		  glBlendEquation(GL_FUNC_ADD);
		  break;
		case flowvr::render::BLEND_PREMULT:
		  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		  glBlendEquation(GL_FUNC_ADD);
		  break;
		case flowvr::render::BLEND_ONE:
		  glBlendFunc(GL_ONE, GL_ONE);
		  glBlendEquation(GL_FUNC_ADD);
		  break;
		case flowvr::render::BLEND_MIN:
		  glBlendFunc(GL_ONE, GL_ONE);
		  glBlendEquation(GL_MIN);
		  break;
		case flowvr::render::BLEND_MAX:
		  glBlendFunc(GL_ONE, GL_ONE);
		  glBlendEquation(GL_MAX);
		  break;
		case flowvr::render::BLEND_STD:
		default:
		  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		  glBlendEquation(GL_FUNC_ADD);
		  break;
		}
	}

	void setGLStateShadeModel( const flowvr::render::cg::CgEngine::StateGL &state )
	{
		 glShadeModel(state.val.b?GL_FLAT:GL_SMOOTH);
	}

	void setGLStatePolygonOffset( const flowvr::render::cg::CgEngine::StateGL &state )
	{
	    if (state.val.f==0.0)
	    {
	      glDisable(GL_POLYGON_OFFSET_FILL);
	      glDisable(GL_POLYGON_OFFSET_LINE);
	      glDisable(GL_POLYGON_OFFSET_POINT);
	    }
	    else
	    {
	      glPolygonOffset(state.val.f,0);
	      glEnable(GL_POLYGON_OFFSET_FILL);
	      glEnable(GL_POLYGON_OFFSET_LINE);
	      glEnable(GL_POLYGON_OFFSET_POINT);
	    }
	}

	void setGLStateClipPlanes( const flowvr::render::cg::CgEngine::StateGL &state )
	{
		for (int i=0;i<6;i++)
		  glSetBool(GL_CLIP_PLANE0+i, i < state.val.i);
	}


	void setGLStateScissorTest( const flowvr::render::cg::CgEngine::StateGL &state )
	{
		if( state.val.v[0] == 0 and state.val.v[1] == 0 and state.val.v[2] == 1 and state.val.v[3] == 1 )
			glSetBool( GL_SCISSOR_TEST, false );
		else
			glSetBool( GL_SCISSOR_TEST, true );

		if(glIsEnabled( GL_SCISSOR_TEST ))
		{
			std::cout << "setting scissor to ["
					  << state.val.v[0]
					  << " ; "
					  << state.val.v[1]
					  << " ; "
					  << state.val.v[2]
					  << " ; "
					  << state.val.v[3]
					  << "]."
					  << std::endl;
		   glScissor( state.val.v[0],state.val.v[1], state.val.v[2], state.val.v[3] );
		}

	}
	// ########################################################################
	// ########################################################################

	ftl::Mat4x4f readBack( GLuint stack )
	{
		ftl::Mat4x4f m;
		glGetFloatv(stack,m.ptr());
		return m;
	}

	template<typename T>
	static void assignFromParam( const flowvr::render::cg::CgEngine::Primitive *p,
			                     const std::string &sValName, T &val)
	{
		  typedef std::map<std::string, flowvr::render::PrimitiveTemplate<flowvr::render::cg::CgData>::ParamOpenGL> OGLVALMP;
		  OGLVALMP::const_iterator it = p->paramOpenGL.find(sValName);
		  if ( it != p->paramOpenGL.end())
			  (*it).second.assign<T> (val);
	}

	void printPrimitive( const flowvr::render::cg::CgEngine::Primitive *p )
	{
		std::cout << "primitive @ " << p << " [" << p->id << "] ; (" << p->tp << ") [" << p->name << "]" << std::endl;
	}

	void printMatrix( const std::string &strName, const ftl::Mat4x4f &m )
	{
		std::cout << strName << ": " << std::endl
				  << m << std::endl;
	}


	 bool checkErrors(const std::string& id)
	{
	  GLenum err = glGetError();
	  if (err!=GL_NO_ERROR)
	  {
		std::cerr << "GL ERRORS AT "<<id<<":";
		do
		{
		  const char* str = (const char*)gluErrorString(err);
		  if (str)
			  std::cerr << ' ' << str;
		  else
			  std::cerr << " 0x"<<std::hex<<err<<std::dec;
		  err = glGetError();
		}
		while (err!=GL_NO_ERROR);
		std::cerr << std::endl;
		return false;
	  }
	  return true;
	}

	#define tostr2(a) #a
	#define tostr(a) tostr2(a)



	bool isPow2(int x)
	{
	  return (x&(x-1))==0;
	}

	 const char* compilerArgs[] =
	{
	  "-DUSE_RECT",
	  NULL
	};

	 #define GLERR() checkErrors(__FILE__ "(" tostr(__LINE__) ")") // __PRETTY_FUNCTION__)

	 void CgErrorCallback()
	{
           CGerror errorcode=cgGetError();
           
           const char *errorstring = cgGetErrorString(errorcode);
           
           fprintf(stderr, "cgError %s\n", errorstring);
	}

	 void CgErrorHandler( CGcontext context, CGerror errorcode, void *appdata )
	 {

         const char *errorstring = cgGetErrorString(errorcode);

         fprintf(stderr, "cgErrorHandler: %s\n", errorstring);
         if( errorcode == CG_COMPILER_ERROR)
         {
           fprintf(stderr, "Compiler Error:\n%s\n", cgGetLastListing(context));
         }
   	 }

	 void drawBBox(const flowvr::render::BBox& bb)
	{
	  glBegin(GL_LINE_LOOP);
	  glVertex3f(bb.a[0],bb.a[1],bb.a[2]);
	  glVertex3f(bb.b[0],bb.a[1],bb.a[2]);
	  glVertex3f(bb.b[0],bb.b[1],bb.a[2]);
	  glVertex3f(bb.a[0],bb.b[1],bb.a[2]);
	  glEnd();

	  glBegin(GL_LINE_LOOP);
	  glVertex3f(bb.a[0],bb.a[1],bb.b[2]);
	  glVertex3f(bb.b[0],bb.a[1],bb.b[2]);
	  glVertex3f(bb.b[0],bb.b[1],bb.b[2]);
	  glVertex3f(bb.a[0],bb.b[1],bb.b[2]);
	  glEnd();

	  glBegin(GL_LINES);
	  glVertex3f(bb.a[0],bb.a[1],bb.a[2]);      glVertex3f(bb.a[0],bb.a[1],bb.b[2]);
	  glVertex3f(bb.b[0],bb.a[1],bb.a[2]);      glVertex3f(bb.b[0],bb.a[1],bb.b[2]);
	  glVertex3f(bb.b[0],bb.b[1],bb.a[2]);      glVertex3f(bb.b[0],bb.b[1],bb.b[2]);
	  glVertex3f(bb.a[0],bb.b[1],bb.a[2]);      glVertex3f(bb.a[0],bb.b[1],bb.b[2]);
	  glEnd();
	}

} // anonymous namespace


namespace flowvr
{

namespace render
{

namespace cg
{

	typedef std::list<CgPrimitive>::iterator listIterator;
	typedef std::list<CgPrimitive>::const_iterator cListIterator;

	typedef std::map<std::string, CgParamShader >::iterator paramIterator;
	typedef std::map<std::string, CgParamShader >::const_iterator cParamIterator;

// ############################################################################
// ############################################################################


CgEngine::CgEngine()
: context(0)
, vertexProfile(CG_PROFILE_UNKNOWN)
, pixelProfile(CG_PROFILE_UNKNOWN)
, nbPoints(0)
, nbLines(0)
, nbTriangles(0)
, nbChangesVShader(0)
, nbChangesPShader(0)
, debugMode(DEBUG_STD)
, useShader(false)
, doStats(false)
, m_lastVShader(NULL)
, m_lastPShader(NULL)
, m_resolvedPath()
, m_bgrender(NULL)
, m_bgdata(NULL)
#if defined(FLOWVR_RENDER_PREFIX)
  # define FLOWVR_STR(f) #f
  # define FLOWVR_STR2(f) FLOWVR_STR(f)
, path("share/flowvr-render/data", "FLOWVR_DATA_PATH", m_resolvedPath, "FLOWVR_RENDER_PREFIX", FLOWVR_STR2(FLOWVR_RENDER_PREFIX))
#else
, path()
#endif

{
  glstate[P_COLOR_WRITE   ].init("ColorWrite" , 1, &setGLStateColorMask, 0);
  glstate[P_BLEND         ].init("Blend"      , false, &setGLStateBool, GL_BLEND);
  glstate[P_DEPTH_TEST    ].init("DepthTest"  , true, &setGLStateBool,  GL_DEPTH_TEST);
  glstate[P_DEPTH_WRITE   ].init("DepthWrite" , true, &setGLStateDepthMask );
  glstate[P_CULL_FACE     ].init("CullFace"   , 0, &setGLStateCullFace);
  glstate[P_STENCIL_TEST  ].init("StencilTest", false, &setGLStateBool, GL_STENCIL_TEST);
  glstate[P_DEPTH_FUNC    ].init("DepthFunc"  , GL_LESS, &setGLStateDepthFunc);
  glstate[P_BLEND_MODE    ].init("BlendMode"  , BLEND_STD, &setGLStateBlendFunc);
  glstate[P_FLAT          ].init("Flat"       , false, &setGLStateShadeModel);
  glstate[P_ALPHA_TEST    ].init("AlphaTest"  , 0.0f, &setGLStateSetThreshold, GL_ALPHA_TEST);
  glstate[P_POLYGON_OFFSET].init("ZOffset"    , 0.0f, &setGLStatePolygonOffset );
  glstate[P_CLIP_PLANES   ].init("ClipPlanes" , 0, &setGLStateClipPlanes);
  glstate[P_FOG           ].init("Fog"        , false, &setGLStateBool, GL_FOG);
  glstate[P_LIGHTING      ].init("Lighting"   , true, &setGLStateBool, GL_LIGHTING );
  glstate[P_TEX_GEN_S     ].init("TexGenS"   , false, &setGLStateBool, GL_TEXTURE_GEN_S );
  glstate[P_TEX_GEN_R     ].init("TexGenR"   , false, &setGLStateBool, GL_TEXTURE_GEN_R );
  glstate[P_TEX_GEN_T     ].init("TexGenT"   , false, &setGLStateBool, GL_TEXTURE_GEN_T );
  float defScissor[4] = {0,0,1,1};
  glstate[P_SCISSOR       ].init("Scissor"   , defScissor , &setGLStateScissorTest );
}

CgEngine::~CgEngine()
{
    if(defaultVertexShader)
        cgDestroyProgram( defaultVertexShader );
    if( defaultPixelShader )
        cgDestroyProgram( defaultPixelShader );
    if( context )
        cgDestroyContext(context);
}


bool CgEngine::init()
{
  if (!BaseEngine<CgData>::init())
	  return false;

  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    std::cerr << "GLEW INIT FAILED: "<<glewGetErrorString(err)<<std::endl;
  }

#ifdef GL_ARB_vertex_buffer_object
  std::cout << (GLEW_ARB_vertex_buffer_object?"Using":"NOT Using")<<" GL_ARB_vertex_buffer_object"<<std::endl;
#endif

#ifdef GL_EXT_framebuffer_object
  std::cout << (GLEW_EXT_framebuffer_object?"Using":"NOT Using")<<" GL_EXT_framebuffer_object"<<std::endl;
#endif

#ifdef GL_NV_primitive_restart
  std::cout << (GLEW_NV_primitive_restart?"Using":"NOT Using")<<" GL_NV_primitive_restart"<<std::endl;
#endif


  std::cout<< "Creating Cg Context."<<std::endl;
//  cgSetErrorCallback( CgErrorCallback );
  cgSetErrorHandler( CgErrorHandler,  (void*)this );

  context = cgCreateContext();

  // Get the latest GL vertex profile
  vertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
  // Validate our profile
  if (vertexProfile == CG_PROFILE_UNKNOWN)
  {
    std::cerr << "No Cg vertex profile supported on this system."<<std::endl;
    return true;
  }

  // Get the latest GL fragment profile
  pixelProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
  // Validate our profile
  if (pixelProfile == CG_PROFILE_UNKNOWN)
  {
    std::cerr << "No Cg fragment profile supported on this system."<<std::endl;
    return true;
  }

  std::cout << "CG INIT: vertex: "<<cgGetProfileString(vertexProfile)<<", pixel: "<<cgGetProfileString(pixelProfile)<<std::endl;

  std::cout<<"Creating default vertex shader."<<std::endl;
  defaultVertexShader = loadShaderFromFile(vertexProfile, "shaders/default_v.cg");

  std::cout<<"Creating default pixel shader."<<std::endl;
  defaultPixelShader = loadShaderFromFile(pixelProfile, "shaders/default_p.cg");

  useShader = true;


  // creating camera for default rendering on framebuffer (tg0)
  Primitive *cam = primitives.add(ID_CAMERA, NULL, 0, TP_CAMERA);
  if( !cam )
	  std::cerr << "Camera creation failed." << std::endl;
  else
  {
	  std::cout << "Camera created @ " << cam << std::endl;
	  cam->name = "default camera (ID_CAMERA)";
	  Primitive::ParamOpenGL* dest = &cam->paramOpenGL["clear"];

	  float col[4] = {0.5,0.5,0.5,0};
	  *dest = TypedArray(Type::Vec4f, &col, 4*sizeof(float) );

	  cam->proj.mat = ftl::matrixPerspectiveProjection( sceneVertFOV, 4.0f/3.0f, 0.001f, 1000.0f);
	  ++cam->proj.iteration;

	  cam->xform.mat = ftl::matrixTransform( Vec3f( 0, 0, 2 ) );
	  ++cam->xform.iteration;
  }
  return true;
}


bool CgEngine::getDoStats() const
{
	return doStats;
}

void CgEngine::setDoStats(bool bDo)
{
	doStats = bDo;
}

void CgEngine::projPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	gluPerspective(fovy, aspect, zNear, zFar);
}


CGprogram CgEngine::loadShaderFromFile(CGprofile profile, std::string filename)
{
  if (!path.findFile(filename))
	  return 0;

  CGprogram shader = cgCreateProgramFromFile(context,
						CG_SOURCE,
						filename.c_str(),
						profile,
						NULL,
						compilerArgs);

  if (cgIsProgramCompiled(shader))
  {
    cgGLEnableProfile(profile);
    cgGLLoadProgram(shader);
    cgGLDisableProfile(profile);
    return shader;
  }
  else
  {
    std::cout << "ERROR IN SHADER "<<filename<<std::endl;
    std::string profname = cgGetProfileString(profile);
    std::string cmd = "cgc -profile "; cmd+=profname;
    cmd+=' '; cmd+=filename;
    std::cout << cmd << std::endl;

    FILE* f = popen(cmd.c_str(),"w");
    pclose(f);
    return 0;
  }
}

CGprogram CgEngine::loadShader(CGprofile profile, const char *source)
{

  CGprogram shader =
    cgCreateProgram(context,
		    CG_SOURCE,
		    source,
		    profile,
		    NULL,
		    compilerArgs);
  if (cgIsProgramCompiled(shader))
  {
    cgGLEnableProfile(profile);
    cgGLLoadProgram(shader);
    cgGLDisableProfile(profile);
    return shader;
  }
  else
  {
    std::cout << "ERROR IN SHADER:\n"<<source<<std::endl;
    std::string profname = cgGetProfileString(profile);
    std::string cmd = "cgc -profile "; cmd+=profname;
    std::cout << cmd << std::endl;
    FILE* f = popen(cmd.c_str(),"w");
    fwrite(source, strlen(source), 1, f);
    pclose(f);
    return 0;
  }
}


bool CgEngine::supportPixelShader(ShaderLanguage language)
{
  return (language == SHADER_ALL) or (language == SHADER_CG);
}

bool CgEngine::supportVertexShader(ShaderLanguage language)
{
  return (language == SHADER_ALL) or (language == SHADER_CG);
}

// ############################################################################
// GL STATE
// ############################################################################


void CgEngine::applyGLState(bool all)
{
 for( size_t n = 0; n < P_NB; ++n )
 {
	 if( all or glstate[n].modif() and glstate[n].m_setF )
	 {
		 (*glstate[n].m_setF)( glstate[n] );
		 glstate[n].validate();
	 }
 }
}

void CgEngine::resetGLState()
{
  for (int i=0;i<P_NB;++i)
	  glstate[i].reset();
}

void CgEngine::invalidGLState()
{
  for (int i=0;i<P_NB;++i)
    glstate[i].invalidate();
}

void CgEngine::dumpGLState()
{
	for( int n=0; n < P_NB; ++n )
		std::cout << "[" << glstate[n].name << "] = [" << glstate[n].val.i << "]" << std::endl;
}

// ############################################################################
// MAIN RENDER ROUTINES AND CALLBACKS FOR SCENE MANAGEMENT
// ############################################################################

void CgEngine::viewProjAndProjUpdate( const Mat4x4f &viewProj, const Mat4x4f &proj )
{
	// empty on purpose here, subclasses may define a behavior.
}


BaseEngine<CgData>::Scene *CgEngine::createTargetScene( unsigned int nTg )
{
	Scene *s;
	if(nTg == 0)
		s = new Scene;
	else
		s = new TextureTargetScene(this, ID(nTg) );

	Primitive *cam = primitives.get(ID_CAMERA);
	if(cam != NULL)
	{
		enforcePrimitiveAddConstraint( s, cam, (1<<nTg) );
		cam->targetGroups = cam->targetGroups bitor (1<<nTg);
	}
	return s;
}

void CgEngine::render( float aspectRatio )
{
	int it = primitives.lastIt()+1;
	GLint viewport[4];
	bool setViewport = false;

	// set-up GL state
	invalidGLState();

	for( TGTGRP::const_iterator cit = m_targetGroups.begin(); cit != m_targetGroups.end(); ++cit )
	{
		if( (*cit).second->enabled )
		{
			resetGLState();
			applyGLState();
			bool bDoRender = (*cit).second->preRender( aspectRatio, viewport, it, setViewport );

			if(bDoRender)
			{
				doRender( aspectRatio, *((*cit).second), (*cit).first, false );

				(*cit).second->postRender();

				if( setViewport )
					glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
			}
		}
	}
}


bool  CgEngine::beginRenderToTexture( Scene *s, ID ttype, float &aspectRatio, GLint viewport[4], int lastit, bool  &setViewport )
{
	setViewport = false;

	Texture* texture = textures.getByType(ttype);

	if (texture == NULL or !texture->created)
	{
		_ONCE(std::cerr << "ERROR: (RTT-0): Render texture 0x" << std::hex << ttype
					  << std::dec << " not found." << std::endl;)
		return setViewport;
	}
	else if (!texture->userData.glid or texture->nx == 0 or texture->ny == 0)
	{
		_ONCE(std::cerr << "ERROR: (RTT-1): Render texture 0x" << std::hex << ttype
				  << std::dec << " invalid -- (no glid or dimenion == (0|0))" << std::endl;)
		return setViewport;
	}

	if (GLEW_EXT_framebuffer_object)
	{
		CgData::Target& t = s->userData;
		if (!t.fbid)
		{
			glGenFramebuffersEXT(1, &t.fbid);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, t.fbid);

			if (texture->isDepth())
			{ // render to depth texture
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
						GL_DEPTH_ATTACHMENT_EXT, texture->userData.target,
						texture->userData.glid, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}
			else
			{
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
						GL_COLOR_ATTACHMENT0_EXT, texture->userData.target,
						texture->userData.glid, 0);

				if( (glstate[P_DEPTH_TEST].val.b == true) or (glstate[P_DEPTH_WRITE].val.b == true) )
				{
					// attach a depth-buffer to this FBO automatically
					glGenRenderbuffersEXT(1, &t.dbid);
					glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, t.dbid);
					glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, texture->nx, texture->ny );
					glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, t.dbid );

//					std::cout << "created automatic depthbuffer " << t.dbid << " for fbo " << t.fbid << std::endl;
				}
				GLERR();
			}

			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

			switch(status)
			{
				case GL_FRAMEBUFFER_COMPLETE_EXT:
				{
					std::cout << "Render texture 0x" << std::hex << ttype
							  << std::dec << " init OK." << std::endl;
					break;
				}
				case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				{
					_ONCE( std::cerr << "ERROR: Render texture 0x" << std::hex << ttype
							  << std::dec
							  << " format not supported by the graphics card."
							  << std::endl;
					)

					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
					glDeleteFramebuffersEXT(1, &t.fbid);
					t.fbid = 0;

					if( t.dbid )
					{
						glDeleteRenderbuffersEXT( 1, &t.dbid );
						t.dbid = 0;
					}
					return setViewport;
				}
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				{
					_ONCE( std::cerr << "ERROR: Render texture 0x" << std::hex << ttype
							  << std::dec << " missing attachment." << std::endl; )
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
					glDeleteFramebuffersEXT(1, &t.fbid);
					t.fbid = 0;
					if( t.dbid )
					{
						glDeleteRenderbuffersEXT( 1, &t.dbid );
						t.dbid = 0;
					}
					break;
				}
				default:
				{
					_ONCE( std::cerr << "ERROR: Render texture 0x" << std::hex << ttype
							  << std::dec << " invalid format." << std::endl; )
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
					glDeleteFramebuffersEXT(1, &t.fbid);
					t.fbid = 0;
					if( t.dbid )
					{
						glDeleteRenderbuffersEXT( 1, &t.dbid );
						t.dbid = 0;
					}
					return setViewport;
				}
			}
		}
		else
		{
			// Do not update the texture if nothing has changed
			if (!s->changedSince(lastit))
				return setViewport;

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, t.fbid);
		}
	}

	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, texture->nx, texture->ny);
	aspectRatio = float(texture->nx) / texture->ny;

	setViewport = true;
	return setViewport;
}

void CgEngine::endRenderToTexture(Scene *s, ID tid)
{
	Texture* texture = textures.getByType(tid);
	if(texture)
	{
		if (GLEW_EXT_framebuffer_object)
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			if (texture->isDepth())
			{ // render to depth texture
				glReadBuffer(GL_BACK);
				glDrawBuffer(GL_BACK);
			}
		}
		else
		{
			glBindTexture(texture->userData.target, texture->userData.glid);
			glCopyTexSubImage2D(texture->userData.target, 0, 0, 0, 0, 0, texture->nx, texture->ny);
			glBindTexture(texture->userData.target, 0);
		}
	}
}

void CgEngine::doBackground(const Primitive *camera)
{
	std::map<std::string, PrimitiveTemplate<CgData>::ParamOpenGL>::const_iterator it = camera->paramOpenGL.find("clear");
	if ( it != camera->paramOpenGL.end())
	{
		Vec4f c(0, 0, 0, 0);
		(*it).second.assign<Vec4f> (c);

		glClearColor(c[0], c[1], c[2], c[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

bool CgEngine::saveTexture(const Texture* tx, const std::string &filename) const
{
	const ChunkTexture *texture =  *tx->firstLevel();
	if(!texture)
		return false;

	ChunkRenderWriter w;
	return w.saveTexture( texture, filename );
}


void CgEngine::toggleScene(int index )
{
//	std::cout << "++++++++++++++++++++++++++++++++++++++++" << std::endl;
//	std::cout << "CgEngine::toggleScene(" << index << ")" << std::endl;

	Scene *scene = this->getScene(index);
	if(!scene)
	{
//		std::cerr << "This scene index does not exist! [" << index << "]" << std::endl;
//		std::cout << "++++++++++++++++++++++++++++++++++++++++" << std::endl;
		return;
	}

	scene->enabled = !scene->enabled;
//	std::cout << "scene (" << index << ") is now " << (scene->enabled ? "ENABLED" : "DISABLED") << std::endl;
}

void CgEngine::dumpScene( int index ) const
{
	std::cout << "++++++++++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << "CgEngine::dumpScene(" << index << ")" << std::endl;

	Scene *scene = this->getScene(index);
	if(!scene)
	{
		std::cerr << "This scene index does not exist! [" << index << "]" << std::endl;
		std::cout << "++++++++++++++++++++++++++++++++++++++++" << std::endl;
		return;
	}

	printPrimitive( (*scene).camera );

	std::for_each( (*scene).getPrimitives().begin(), (*scene).getPrimitives().end(), printPrimitive );

	std::cout << "++++++++++++++++++++++++++++++++++++++++" << std::endl;
}

void CgEngine::doRender(float aspectRatio, Scene &scene, int target, bool dump)
{
	if(dump)
		std::cout << "+++++++++++++++++++++++++++++++++++" << std::endl;

  // ----------------------------------------------------------------------
  // BACKGROUND
  // ----------------------------------------------------------------------
  // clear background according to property in camera
  const Primitive *camera = NULL;

  if( !scene.empty() and !scene.camera )
  {
	 _ONCE(
		  std::cout << "no camera found for this scene @" << &scene << std::endl;
		  std::for_each( scene.getPrimitives().begin(), scene.getPrimitives().end(), printPrimitive );
	  )

	  Primitive *defCam = primitives.get(ID_CAMERA);
	  if(defCam)
	  {
		  addToTargetGroups( defCam, 1<<target, true );
		  camera = defCam;
	  }
  }
  else if( scene.empty() )
	  return; // nothing to do

  camera = scene.camera; // simply cache camera pointer here

  doBackground(camera);

  // ----------------------------------------------------------------------
  // UNBIND VERTEX AND INDEX BUFFER MEMORY
  // ----------------------------------------------------------------------
#ifdef GL_ARB_vertex_buffer_object
  if (GLEW_ARB_vertex_buffer_object)
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB,0); // set vbo id to 0 (unbind current binding)
    currentVertexBufferGLID = 0;
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0); // set ibo id to 0 (unbind current binding)
    currentIndexBufferGLID  = 0;
  }
#endif

  currentPrimitiveRestartEnabled = false;
  currentPrimitiveRestartIndex   = 0;

  // set wireframe or solid mode for opengl code base
  // on the value of the debugMode flag
  if (debugMode == DEBUG_WIREFRAME)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // ... and initial transformations!
  curModel.identity();

  // first care about the projection matrix
  glMatrixMode(GL_PROJECTION);
  glPushMatrix(); // save current state

  if( camera->proj.iteration < 0 )
  {
	  // user did not (yet?) change projection, so assume a default
	  // projection and download that to the proj variable
	  glLoadIdentity();

	  float fov = sceneVertFOV;
	  float zmin = 0.001f;
	  float zmax = 1000.0f;
	  float aspect = aspectRatio;

	  assignFromParam<float>( camera, "fovy", fov );
	  assignFromParam<float>( camera, "zmin", zmin);
	  assignFromParam<float>( camera, "zmax", zmax );
	  assignFromParam<float>( camera, "aspect", aspect );

	  gluPerspective( fov, aspect, zmin, zmax );

	  // now download
	  glGetFloatv(GL_PROJECTION_MATRIX,curProj.ptr());

	  // GL -> flowvr-render
	  curProj.transpose();
	  curProjInv.invert(curProj);
  }
  else
  {
	  // user defined a projection, so use that
	  Mat4x4f proj;
	  proj.transpose(camera->proj.mat);
	  glLoadMatrixf( proj.ptr() );

	  // store in curProj and compute the inverse while being at it
	  curProj = camera->proj.mat;
	  curProjInv.invert(curProj);
  }

  // now for the modelview...

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix(); // save old state

  // did the user touch the camera xform?
  if( camera->xform.iteration < 0 )
  {
	  // well... no, assume unit-cube and z = 2
	  ftl::Mat4x4f f;
	  f.identity();
	  f[2][3] = 2;
	  curView = f;
	  curViewInv.invert(curView);
  }
  else
  {
	  // well, yes... use it (it is uploaded below)
	  // note that the flowvr-render 'camera' is assumed to be defined in world space
	  // that's why we use the inverted xform for the curView (this is camera space again)
	  curView.invert(camera->xform.mat);
	  curViewInv   = camera->xform.mat; // simply use the original one as the inverted one
  }

  ftl::Mat4x4f view; // to store the changed view and projection matrix
                     // we declare that here, as we are going to re-use the matrix
  	  	  	  	  	 // during the render loop as temporary storage

  view.transpose(curView); // convert from flowvr-render to GL
  glLoadMatrixf(view.ptr()); // load current view

  // create ViewProj matrix and its inverse
  curViewProj    = curProj    * curView;
  curViewProjInv = curViewInv * curProjInv;

  // that's a call-back hook (currently needed for backwards compatibility)
  // it allows sub-classes to capture the change of these matrices during the render loop.
  viewProjAndProjUpdate( curViewProj, curProj );

  /////////////////////////////////////////////////////////////////////
  // POINTSPRITES SUPPORT
  // @todo see a way to have  this enabled on demand (per primitive?)
  /////////////////////////////////////////////////////////////////////
  glDisable(GL_LIGHTING);
  glColor4f(1,1,1,1);
  glEnable(GL_DEPTH_CLAMP_NV);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
  glEnable(GL_POINT_SPRITE_NV);
  glTexEnvi(GL_POINT_SPRITE_NV, GL_COORD_REPLACE_NV, GL_TRUE);
  glAlphaFunc(GL_GEQUAL,0.1);

  // ----------------------------------------------------------------------
  // ENABLE AND BIND DEFAULT PROFILES
  // ----------------------------------------------------------------------

  if (defaultVertexShader)
  {
    cgGLEnableProfile(vertexProfile);
    cgGLBindProgram(defaultVertexShader);
    ++nbChangesVShader;
  }
  if (defaultPixelShader)
  {
    cgGLEnableProfile(pixelProfile);
    cgGLBindProgram(defaultPixelShader);
    ++nbChangesPShader;
  }


  // ----------------------------------------------------------------------
  // the primitive render loop itself
  // ----------------------------------------------------------------------


  // declare state the matrices here,
  // as we are going to re-use the memory in the loop
  ftl::Mat4x4f oldproj, oldprojinv, oldprojview,
               oldprojviewinv, oldview, oldviewinv;

  resetGLState();

  for(std::vector<Primitive*>::const_iterator i = scene.getPrimitives().begin(); i != scene.getPrimitives().end(); ++i)
  {
    const Primitive* p = *i;
    if ( p->visible and !p->vBuffer.empty() and p->iBuffer.valid() and p->id != camera->id )
    {
    	// ok, p wants to be drawn and has vBuffer and iBuffer attached AND it is
    	// not the camera object for this current scene.
    	// check for special attributes first (projection / transformation)
		glMatrixMode( GL_MODELVIEW );

		// adjust current model-view to this primitive's transformation matrix
		glPushMatrix();

		if( p->xform.override )
		{
			oldview    = curView;
			oldviewinv = curViewInv;

			view.transpose(p->xform.mat);

			glLoadMatrixf(view.ptr()); // load matrix as given

			curView.invert(p->xform.mat);
			curViewInv = p->xform.mat;

			// create ViewProj matrix and its inverse
			curViewProj    = curProj    * curView;
			curViewProjInv = curViewInv * curProjInv;
		}
		else
		{
			view.transpose(p->xform.mat); // p's matrix needs to be transposed
			glMultMatrixf(view.ptr()); // multiply with current state (relative transform)
		}

		if (p->proj.iteration >= 0)
		{
			// the projection of this primitive was modified
			// upload its projection, but before, store the old one to restore later
			oldproj        = curProj;
			oldprojinv     = curProjInv;
			oldprojview    = curViewProj;
			oldprojviewinv = curViewProjInv;

			// switch to projection mode
			glMatrixMode(GL_PROJECTION);
			glPushMatrix(); // save current matrix

			ftl::Mat4x4f mp;
			mp.transpose(p->proj.mat); // p's matrix needs to be transposed
			glLoadMatrixf(mp.ptr()); // upload

			curProj = p->proj.mat; // save current projection
			curProjInv.invert(curProj); // and invert this one for later use

			curViewProj    = curProj    * curView;
			curViewProjInv = curViewInv * curProjInv;

			// switch back to modelview-matrix mode
			glMatrixMode(GL_MODELVIEW);
		}

		curModel = p->xform.mat; // save the current model matrix (NOT the global state!)

		stateInit(p);
		renderPrimitive(p);
		stateExit(p);

		// check if the projection was touched by this primitive...
		if (p->proj.iteration >= 0)
		{
			curProj        = oldproj;     // save back projection state for shaders
			curProjInv     = oldprojinv; // save back projection state for shaders
			curViewProj    = oldprojview;
			curViewProjInv = oldprojviewinv;

			// ok, so above we set the primitive's projection matrix
			// so we have to reset that (be nice to other primitives)
			glMatrixMode(GL_PROJECTION);
			glPopMatrix(); // POP PROJECTION (the one that was pushed beforehand)
			glMatrixMode(GL_MODELVIEW);
		}

		if( p->xform.override )
		{
			curView    = oldview;
			curViewInv = oldviewinv;

			// create ViewProj matrix and its inverse
			curViewProj    = curProj    * curView;
			curViewProjInv = curViewInv * curProjInv;
		}

		glPopMatrix(); // POP MODELVIEW for this primitive
	} // render primitive
  } // for_each primitive


  // ----------------------------------------------------------------------
  // disable profiles again
  // ----------------------------------------------------------------------
  if (useShader)
  {
    cgGLDisableProfile(vertexProfile);
    cgGLDisableProfile(pixelProfile);
  }

  // ----------------------------------------------------------------------
  // do (conditional) debug render
  // ----------------------------------------------------------------------
  debugRender();

  // ----------------------------------------------------------------------
  // unlink all ARB buffers again
  // ----------------------------------------------------------------------

#ifdef GL_ARB_vertex_buffer_object
  if (GLEW_ARB_vertex_buffer_object)
  {
    if (currentVertexBufferGLID)
    {
      glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
      currentVertexBufferGLID = 0;
    }
    if (currentIndexBufferGLID)
    {
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
      currentIndexBufferGLID = 0;
    }
  }
#endif

  // ----------------------------------------------------------------------
  // reset global state
  // ----------------------------------------------------------------------
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}


CGprogram CgEngine::bindShader(CGprogram vs, CGprogram def, CGprofile prof, size_t &cnt )
{
	if( !vs )
	{
		if( !def )
			cgGLDisableProfile(prof);
		else
		{
			cgGLBindProgram( def );
			return def;
		}
	}
	else
	{
		cgGLBindProgram( vs );
		cgGLEnableProfile(prof);
	}
	++cnt;

	return vs;
}


void CgEngine::stateInit( const Primitive *p )
{
	//shaders init
	bindShader( (p->vShader ? p->vShader->userData.program : CGprogram(0)), defaultVertexShader, vertexProfile, nbChangesVShader );

	m_lastVShader = p->vShader; // can be NULL

	bindShader( (p->pShader ? p->pShader->userData.program : CGprogram(0)), defaultPixelShader, pixelProfile, nbChangesPShader );
	m_lastPShader = p->pShader; // can be NULL

	////////////////////////////////////////////////////
	// PIXEL SHADER PARAMETER
	////////////////////////////////////////////////////
	cgInitParameters(p->pShader);
	for (cParamIterator j = p->paramPShader.begin(); j != p->paramPShader.end(); j++)
		cgSetParameter(j->second);

	///////////////////////////////////////////////////
	// VERTEXT SHADER PARAMETER
	///////////////////////////////////////////////////
	cgInitParameters(p->vShader);
	for (cParamIterator j = p->paramVShader.begin(); j != p->paramVShader.end(); j++)
		cgSetParameter(j->second);

	///////////////////////////////////////////////////
	// TEXTURE PARAMETER INIT
	///////////////////////////////////////////////////
	for (std::map<std::string, CgParamTexture>::const_iterator j = p->textures.begin(); j != p->textures.end(); ++j)
		cgSetParameter(j->second);


	//////////////////////////////////////////////////
	// vertices parameters
	//////////////////////////////////////////////////
	if (m_lastVShader != NULL || (defaultVertexShader))
	{
		for (std::map<std::string, CgParamVBuffer>::const_iterator j = p->vBuffer.begin(); j != p->vBuffer.end(); ++j)
			cgSetParameter(j->second);
	}
	else
	{
		std::map<std::string, Primitive::VBData>::const_iterator ci = p->vBuffer.find("position");
		if (ci != p->vBuffer.end())
			cgSetParameter((*ci).second,false);

		// no CG Vertex Shader
		std::map<std::string, Primitive::ParamVShader>::const_iterator cit = p->paramVShader.find("color");
		if ( cit != p->paramVShader.end())
		{
			const CgParamShader& param = (*cit).second;
			switch (param.type())
			{
			case ftl::Type::Vec3f:
				glColor3fv((float*) param.data());
				break;
			case ftl::Type::Vec4f:
				glColor4fv((float*) param.data());
				break;
			}
		}
	}

	for (std::map<std::string, CgParamOpenGL>::const_iterator j = p->paramOpenGL.begin(); j != p->paramOpenGL.end(); ++j)
	{
		const CgParamOpenGL& o = j->second;
		if ((unsigned) o.userData.index < P_NB)
			glstate[o.userData.index].set(o);
	}

	applyGLState();

	bool b = false;
	if(b)
		dumpGLState();
}
void CgEngine::stateExit( const Primitive *p )
{
	for (std::map<std::string, CgParamTexture>::const_iterator j = p->textures.begin(); j != p->textures.end(); ++j)
		cgResetParameter(j->second);

	if (m_lastVShader != NULL || (defaultVertexShader))
	{
		for (std::map<std::string, CgParamVBuffer>::const_iterator j = p->vBuffer.begin(); j != p->vBuffer.end(); ++j)
			cgResetParameter(j->second);
	}

	// GL Parameters
	resetGLState();
}

void CgEngine::renderPrimitive( const Primitive *p )
{
	const ChunkIndexBuffer* buffer = p->iBuffer.buffer->levels[0];

	GLenum mode = GL_POINTS;

	bool ok = true;
	switch (buffer->primitive)
	{
	case ChunkIndexBuffer::Point:
		mode = GL_POINTS;
		break;
	case ChunkIndexBuffer::Line:
		mode = GL_LINES;
		break;
	case ChunkIndexBuffer::LineStrip:
		mode = GL_LINE_STRIP;
		break;
	case ChunkIndexBuffer::LineLoop:
		mode = GL_LINE_LOOP;
		break;
	case ChunkIndexBuffer::Triangle:
		mode = GL_TRIANGLES;
		break;
	case ChunkIndexBuffer::TriangleFan:
		mode = GL_TRIANGLE_FAN;
		break;
	case ChunkIndexBuffer::TriangleStrip:
		mode = GL_TRIANGLE_STRIP;
		break;
	case ChunkIndexBuffer::Quad:
		mode = GL_QUADS;
		break;
	case ChunkIndexBuffer::Polygon:
		mode = GL_POLYGON;
		break;
	default:
		ok = false;
	}

	int isize = ftl::Type::size(buffer->dataType);

	GLsizei count = 0;
	GLenum type   = GL_UNSIGNED_INT;
	bool useIndices = true;
	int offset = p->iBuffer.i0 * isize;
	const GLvoid* indices = ((const char*) buffer->data()) + offset;

	if (isize > 0)
		count = buffer->dataSize() / isize;

	if ((unsigned) p->iBuffer.nbi < (unsigned) count)
		count = p->iBuffer.nbi;

	switch (buffer->dataType)
	{
	case ftl::Type::Byte:
		type = GL_UNSIGNED_BYTE;
		break;
	case ftl::Type::Short:
		type = GL_UNSIGNED_SHORT;
		break;
	case ftl::Type::Int:
		type = GL_UNSIGNED_INT;
		break;
	case ftl::Type::Null:
		useIndices = false;
		count = p->iBuffer.nbi;
		for (std::map<std::string, CgParamVBuffer>::const_iterator vb = p->vBuffer.begin(); vb != p->vBuffer.end(); ++vb)
		{
			if ( vb->second.valid() )
			{
				int nbp = vb->second.buffer->levels[0]->dataSize() / vb->second.buffer->levels[0]->vertexSize;

				if (p->iBuffer.i0 >= nbp)
					ok = false;
				else if (count < 0 || p->iBuffer.i0 + count > nbp)
					count = nbp - p->iBuffer.i0;
			}
		}
		break;
	default:
		ok = false;
	}

	if (ok && count > 0)
	{
		if (useIndices)
		{
			// Setup index buffer
			#ifdef GL_ARB_vertex_buffer_object
			if (GLEW_ARB_vertex_buffer_object)
			{
				GLuint glid = p->iBuffer.buffer->userData.glid;
				if (glid != currentIndexBufferGLID)
				{
					glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, glid);
					currentIndexBufferGLID = glid;
				}
				if (glid)
					indices = ((const char*) NULL) + offset;
			}
			#endif // GL_ARB_vertex_buffer_object
		}

		// Setup primitive restart
		int restart = buffer->restart;

		#ifdef GL_NV_primitive_restart
		if (GLEW_NV_primitive_restart)
		{
			if (restart)
			{
				if (restart != currentPrimitiveRestartIndex)
				{
					currentPrimitiveRestartIndex = restart;
					glPrimitiveRestartIndexNV(currentPrimitiveRestartIndex);
				}
				if (!currentPrimitiveRestartEnabled)
				{
					currentPrimitiveRestartEnabled = true;
					glEnableClientState(GL_PRIMITIVE_RESTART_NV);
				}
			}
			else
			{
				if (currentPrimitiveRestartEnabled)
				{
					currentPrimitiveRestartEnabled = false;
					glDisableClientState(GL_PRIMITIVE_RESTART_NV);
				}
			}
		}
		else
		#endif // GL_NV_primitive_restart

		if (restart)
		{
			bool first = true;
			if (first)
			{
				std::cerr << "ERROR: GL_NV_primitive_restart not supported. Ignoring restart index."
						  << std::endl;
				first = false;
			}
		}

		if (useIndices)
			glDrawElements(mode, count, type, indices);
		else
			glDrawArrays(mode, p->iBuffer.i0, count);


		if( doStats )
		{
			switch (mode)
			{
				case GL_POINTS:
					nbPoints += count;
					break;
				case GL_LINES:
					nbLines += count / 2;
					break;
				case GL_LINE_STRIP:
					nbLines += count - 1;
					break;
				case GL_LINE_LOOP:
					nbLines += count;
					break;
				case GL_TRIANGLES:
					nbTriangles += count / 3;
					break;
				case GL_TRIANGLE_FAN:
					nbTriangles += count - 2;
					break;
				case GL_TRIANGLE_STRIP:
					nbTriangles += count - 2;
					break;
				case GL_QUADS:
					nbTriangles += count / 2;
					break;
				case GL_POLYGON:
					nbTriangles += count - 2;
					break;
				default:
					break;
			}
		}
	}
}


void CgEngine::debugRender()
{
  ///////////////////////////////////////////////////////////////////
  // DEBUG
  ///////////////////////////////////////////////////////////////////
  if (debugMode == DEBUG_BBOX || debugMode == DEBUG_AABB)
  {
	  resetGLState();
	  applyGLState(true);
		glColor4fv(bboxColor.ptr());
		for (listIterator i = primitives.begin(); i != primitives.end(); i++)
		{
			Primitive* p = &*i;
			if (p->visible && !p->bbox.isEmpty())
			{
				ftl::Mat4x4f view;
				glPushMatrix();
				view.transpose(p->xform.mat);
				glMultMatrixf(view.ptr());

				drawBBox(p->bbox);

				glPopMatrix();
			}
		}
		if (debugMode == DEBUG_AABB)
		{
			glColor4f(0.5, 0.5, 1, 1);
			for (listIterator i = primitives.begin(); i != primitives.end(); i++)
			{
				Primitive* p = &*i;
				if (p->visible && !p->aabb.isEmpty())
				{
					drawBBox(p->aabb);
				}
			}
		}

		if (!sceneBBox.isEmpty())
		{
			glColor4f(1, 0, 1, 1);
			drawBBox(sceneBBox);
		}
		glColor4f(1, 1, 1, 1);
	}
}
// ############################################################################
// ENGINE / CHUNKREADER OVERRIDES
// ############################################################################


bool CgEngine::addPrimitive(Primitive* prim, Primitive* model)
{
    return addToTargetGroups( prim, prim->targetGroups, false, model );
}

bool CgEngine::delPrimitive(Primitive* prim)
{
  return remFromTargetGroups( prim, prim->targetGroups );
}

bool CgEngine::setVShader(Primitive* prim, VertexShader* shader)
{
  cgNewVShader(prim);
  return true;
}

bool CgEngine::setPShader(Primitive* prim, PixelShader* shader)
{
  cgNewPShader(prim);
  return true;
}

bool CgEngine::setVBuffer(Primitive* prim, const char* name, Primitive::VBData* dest, VertexBuffer* buffer, bool create)
{
  if (create)
    cgNewVBParam(name, dest, prim);
  return true;
}

bool CgEngine::setTexture(Primitive* prim, const char* name, Primitive::TData* dest, Texture* texture, bool create)
{
  if (create)
    cgNewTParam(name, dest, prim);
  return true;
}

bool CgEngine::setParamVShader(Primitive* prim, const char* name, Primitive::ParamVShader* dest, const ftl::TypedArray& val, bool create)
{
  if (create)
    cgNewVParam(name, dest, prim);
  return true;
}

bool CgEngine::setParamPShader(Primitive* prim, const char* name, Primitive::ParamPShader* dest, const ftl::TypedArray& val, bool create)
{
  if (create)
    cgNewPParam(name, dest, prim);
  return true;
}

bool CgEngine::setParamOpenGL(Primitive* prim,
		                      const char* name,
		                      Primitive::ParamOpenGL* dest,
		                      const ftl::TypedArray& val,
		                      bool create)
{
  if (prim->tp == TP_CAMERA)
  {
    if (!strcmp(name,"fovy"))
    	return true;
    if (!strcmp(name,"aspect"))
    	return true;
    if (!strcmp(name,"clear"))
    	return true;
    if (!strcmp(name,"zmin"))
    	return true;
    if (!strcmp(name,"zmax"))
    	return true;
  }
  else
  {
    if (!strcmp(name,"shadowCaster"))
    	return true;
    if (!strcmp(name,"shadowReceiver"))
    	return true;
    if (!strcmp(name,"reflected"))
    	return true;
    if (!create)
    	return (dest->userData.index >= 0);

    for (int i=0;i<P_NB;++i)
    {
      if (!strcmp(glstate[i].name,name))
      {
		dest->userData.index = i;
		return true;
      }
    }
  }

  std::cerr << "OpenGL Parameter "<<name<<" not supported."<<std::endl;
  return false;
}

bool CgEngine::setParamDisplay(Primitive* prim, const char* name, Primitive::ParamDisplay* dest, const ftl::TypedArray& val, bool create)
{
  std::cerr << "Display Parameter "<<name<<" not supported."<<std::endl;
  return false;
}

bool CgEngine::updateTexture(Texture* texture, const ChunkTextureUpdate* data)
{
  return addTexture(texture, (const ChunkTexture*)data, false);
}

bool CgEngine::addTexture(Texture* texture, const ChunkTexture* data, bool create)
{
  bool first = false;

  bool update = (data->type == ChunkTextureUpdate::TYPE);

  if (create)
  {
    GLuint tid;
    glGenTextures(1, &tid);
    texture->userData.glid = tid;
    texture->userData.target = 0;
#if defined(DEBUG)
    std::cout << "New GLTexture "<<tid<<std::endl;
#endif
    first = true;
  }

  GLint internalFormat;
  GLenum format;
  GLenum type;

  bool isDepth = false;
  bool isCompressed = false;

  switch (data->imageType & ChunkTexture::MASK_TYPE)
  {
  case ChunkTexture::Grayscale:
    if (ftl::Type::nx(data->pixelType)<=1)
    {
      internalFormat = GL_INTENSITY;
      format = GL_LUMINANCE;
    }
    else if (ftl::Type::nx(data->pixelType)==2)
    {
      internalFormat = GL_LUMINANCE_ALPHA;
      format = GL_LUMINANCE_ALPHA;
    }
    else return false;
    break;
  case ChunkTexture::RGB: // RGBA
    if (ftl::Type::nx(data->pixelType)==3)
    {
      internalFormat = GL_RGB;
      format = GL_RGB;
//      std::cout << "internal format " << std::endl;
    }
    else if (ftl::Type::nx(data->pixelType)==4)
    {
      internalFormat = GL_RGBA;
      format = GL_RGBA;
    }
    else return false;
    break;
  case ChunkTexture::RGB32F_ARB:
	  if(ftl::Type::nx(data->pixelType)==3)
	  {
		  internalFormat = GL_RGB32F_ARB;
		  format = GL_RGB;
	  }
	  else if (ftl::Type::nx(data->pixelType)==4)
	  {
		internalFormat = GL_RGBA32F_ARB;
		format = GL_RGBA;
	  }
	  else return false;
  break;
  case ChunkTexture::BGR: // BGRA
    if (ftl::Type::nx(data->pixelType)==3)
    {
      internalFormat = GL_RGB;
      format = GL_BGR;
    }
    else if (ftl::Type::nx(data->pixelType)==4)
    {
      internalFormat = GL_RGBA;
      format = GL_BGRA;
    }
    else return false;
    break;
  case ChunkTexture::Depth:
    isDepth = true;
    if (ftl::Type::nx(data->pixelType)<=1)
    {
      if (ftl::Type::size(data->pixelType)==2)
	internalFormat = GL_DEPTH_COMPONENT16;
      else if (ftl::Type::size(data->pixelType)==3)
	internalFormat = GL_DEPTH_COMPONENT24;
      else if (ftl::Type::size(data->pixelType)==4)
	internalFormat = GL_DEPTH_COMPONENT32;
      else
	internalFormat = GL_DEPTH_COMPONENT24;
      format = GL_DEPTH_COMPONENT;
    }
    else
    	return false;
    break;
  default:
    return false;
  }
  
  bool isCubemap = ((data->imageType&ChunkTexture::CUBEMAP)!=0);
  
  bool clamp = ((data->imageType&ChunkTexture::CLAMP)!=0);
  bool nearest = ((data->imageType&ChunkTexture::NEAREST)!=0);
  bool rect = ((data->imageType&ChunkTexture::RECT)!=0);
  bool array = ((data->imageType&ChunkTexture::ARRAY)!=0);
  
  if (array)
  {
#ifdef GL_EXT_texture_array
    if (!GLEW_EXT_texture_array)
    {
      std::cerr << "ERROR in texture array: EXT_texture_array OpenGL extension not supported by the driver" << std::endl;
      return false;
    }
#else
    {
      std::cerr << "ERROR in texture array: EXT_texture_array OpenGL extension not supported during compilation" << std::endl;
      return false;
    }
#endif
  }
  
  char* convImg = NULL;

  const char* img = NULL;
  const char* subimg = NULL;
  int ux0=0;
  int uy0=0;
  int uz0=0;
  int unx=data->nx;
  int uny=data->ny;
  int unz=data->nz;

  if (update)
  {
    subimg = (const char*)((const ChunkTextureUpdate*)data)->data();
    ux0 = ((const ChunkTextureUpdate*)data)->ux0;
    uy0 = ((const ChunkTextureUpdate*)data)->uy0;
    uz0 = ((const ChunkTextureUpdate*)data)->uz0;
    unx = ((const ChunkTextureUpdate*)data)->unx;
    uny = ((const ChunkTextureUpdate*)data)->uny;
    unz = ((const ChunkTextureUpdate*)data)->unz;
  }
  else
    subimg = img = (const char*)data->data();
  int lineSize = data->lineSize;
  int blocSize = 1;
  if (isCompressed)
    type = 0;
  else
  switch (ftl::Type::toSingle(data->pixelType))
  {
  case ftl::Type::Byte:
    type = GL_UNSIGNED_BYTE;
    break;
  case ftl::Type::Short:
    type = GL_UNSIGNED_SHORT;
    break;
  case ftl::Type::Int:
    type = GL_UNSIGNED_INT;
    break;
#ifdef GL_HALF_FLOAT_NV
  case ftl::Type::Half:
    // TODO: Check for extension
    type = GL_HALF_FLOAT_NV;
    break;
#endif
  case ftl::Type::Float:
    type = GL_FLOAT;
    break;
  case ftl::Type::Bool:
  {
    type = GL_UNSIGNED_BYTE;
    int nbp = data->dataSize()*8;
    convImg = new char[nbp];
    int val = 0;
    const int* src = (const int*)img;
    for (int i=0;i<nbp;i++)
    {
      if (!(i&31))
    	  val = *(src++);

      convImg[i]=(char)(-(val&1));
      val>>=1;
    }
    img = convImg;
    lineSize*=8;
    break;
  }
  default:
    return false;
  }
  
  //TODO: lineSize support

  const char* tname;

  GLuint target = 0;

  if (isCubemap)
  {
    if (data->nz == 0)
    {
      if (convImg)
    	  delete[] convImg;

      return false;
    }
    target = GL_TEXTURE_CUBE_MAP_ARB;
    tname = "CUBE";
  }
  else
  {
    switch (texture->nbDim)
    {
    case 1:
    	target = GL_TEXTURE_1D;
    	tname="1D";
    	break;
    case 2:
    	target = GL_TEXTURE_2D;
    	tname="2D";
#ifdef GL_EXT_texture_array
      if (array)
      {
		target = GL_TEXTURE_1D_ARRAY_EXT;
		tname = "1D_ARRAY";
      }
      else
#endif
#ifdef GL_TEXTURE_RECTANGLE_NV
      if(rect)
      {
		target = GL_TEXTURE_RECTANGLE_NV;
		tname = "RECT";
      }
      else
#endif
      {
        target = GL_TEXTURE_2D;
        tname = "2D";
      }
      break;
    case 3:
#ifdef GL_EXT_texture_array
      if (array)
      {
        target = GL_TEXTURE_2D_ARRAY_EXT;
        tname = "2D_ARRAY";
      }
      else
#endif
      {
        target = GL_TEXTURE_3D;
        tname = "3D";
      }
      break;
    default:
      if (convImg)
    	  delete[] convImg;
      return false;
    }
  }
  
#ifdef GL_ARB_texture_compression
  if (isCompressed && !GLEW_ARB_texture_compression)
  {
    std::cerr << "ERROR in compressed texture: ARB_texture_compression OpenGL extension not supported" << std::endl;
    return false;
  }
#endif

  // this texture is used as FBO?
  if( texture->tp >= TP_RENDER_TEXTURE_BASE )
  {
  	// yes...
  	Scene *s = getScene( TargetGroups::txTp2Tg( texture->tp ) );
  	if( s )
  	{
  		bool touched = false;
  		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  		if( s->userData.dbid )
  		{
  			touched = true;
  			// detach depth-buffer
  			glDeleteRenderbuffersEXT( 1, &s->userData.dbid );
  			s->userData.dbid = 0;
  		}

  		if( s->userData.fbid )
  		{
  			// detach fbo
  			touched = true;
  			glDeleteFramebuffersEXT(1, &s->userData.fbid);
  			s->userData.fbid = 0;
  		}

  		if(touched)
  			++s->userData.iteration;
  	}
  }

  if (texture->userData.target != target)
  {
    if (texture->userData.target)
    {
      GLuint tid = texture->userData.glid;
#if defined(DEBUG)
      std::cout << "Deleting GLTexture "<<tid<<std::endl;
#endif

      glDeleteTextures(1,&tid);
      glGenTextures(1, &tid);
      texture->userData.glid = tid;

#if defined(DEBUG)
      std::cout << "New GLTexture "<<tid<<std::endl;
#endif
    }
    first = true;
    texture->userData.target = target;
  }

  if (texture->userData.nx != data->nx)
  {
    texture->userData.nx = data->nx;
    first = true;
  }

  if (texture->userData.ny != data->ny)
  {
    texture->userData.ny = data->ny;
    first = true;
  }

  if (texture->userData.nz != data->nz)
  {
    texture->userData.nz = data->nz;
    first = true;
  }

  if (texture->userData.internalFormat != internalFormat)
  {
    texture->userData.internalFormat = internalFormat;
    first = true;
  }

  if (texture->userData.type != type)
  {
    texture->userData.type = type;
    first = true;
  }

  glBindTexture(target, texture->userData.glid);

  if (isDepth && first)
  {
      glTexParameteri(target, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
      glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
  }

  if (data->level < 0
#ifdef GL_GENERATE_MIPMAP_SGIS
      && !GLEW_SGIS_generate_mipmap
#endif
      && !isCompressed && !array) // || (data->level==0 && data->gen < 0 && texture->levels.size()==1))
  { // CPU mipmaps building (not supported for compressed formats or texture arrays)
    if (first)
    {
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, nearest ? GL_NEAREST : GL_LINEAR);
    }

    if (isCubemap)
    {
      glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#if defined(DEBUG)
      std::cout << "Uploading " << tname
    		    << " mipmapped texture "
    		    << texture->userData.glid
    		    << ' '
    		    << data->nx
    		    << 'x'
    		    << data->ny
    		    << 'x'
    		    << data->nz
    		    << 'x'
    		    << ftl::Type::size(data->pixelType)*8
    		    << std::endl;
#endif
      int imgsize = lineSize * data->ny;
      for (int i=0;i<data->nz && i<6; i++)
      {
		gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB+i, internalFormat, data->nx, data->ny,
				  format, type, img+i*imgsize);
      }
    }
    else
    {
      if (first && clamp)
      {
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      }
      switch (texture->nbDim)
      {
      case 1:
#if defined(DEBUG)
		std::cout << "Uploading "<<tname<<" mipmapped texture "<<texture->userData.glid<<' '<<data->nx<<'x'<<ftl::Type::size(data->pixelType)*8<<std::endl;
#endif
		gluBuild1DMipmaps(target, internalFormat, data->nx,
				  format, type, img);
		break;
      case 2:
#if defined(DEBUG)
		std::cout << "Uploading "<<tname<<" mipmapped texture "<<texture->userData.glid<<' '<<data->nx<<'x'<<data->ny<<'x'<<ftl::Type::size(data->pixelType)*8<<std::endl;
#endif
		gluBuild2DMipmaps(target, internalFormat, data->nx, data->ny,
				  format, type, img);
		break;
      case 3:
#if defined(DEBUG)
		std::cout << "Uploading "<<tname<<" mipmapped texture "<<texture->userData.glid<<' '<<data->nx<<'x'<<data->ny<<'x'<<data->nz<<'x'<<ftl::Type::size(data->pixelType)*8<<std::endl;
#endif
		gluBuild3DMipmaps(target, internalFormat, data->nx, data->ny, data->nz,
			  format, type, img);
		break;
      }
    }
  }
  else
  { // single level upload + optional GPU mipmap generation
    int level = (data->level < 0) ? 0 : data->level;
    if (first)
    {
#ifdef GL_GENERATE_MIPMAP_SGIS
      if (data->level < 0 && GLEW_SGIS_generate_mipmap)
    	  glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

      glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
		  (texture->levels.size()>1
		  || (data->level < 0 && GLEW_SGIS_generate_mipmap) ?
		  ( nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR) : (nearest ? GL_NEAREST : GL_LINEAR)) );
#else
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, (texture->levels.size()>1) ?
    		                                         ( nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR)
    		                                         : (nearest ? GL_NEAREST : GL_LINEAR));
#endif
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, nearest ? GL_NEAREST : GL_LINEAR);
    }
    if (isCubemap)
    {
      if (first)
      {
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      }
#if defined(DEBUG)
      std::cout << "Uploading " << tname << " level "<<level
#ifdef GL_GENERATE_MIPMAP_SGIS
		<<((data->level < 0 && GLEW_SGIS_generate_mipmap)?" mipmapped":"")
#endif
		<<" texture "<<texture->userData.glid<<' '<<data->nx<<'x'<<data->ny<<'x'<<data->nz<<'x'<<Type::size(data->pixelType)*8<<std::endl;
#endif
      int imgsize = lineSize * (uny + blocSize-1)/blocSize;
      for (int i=0;i<unz && uz0+i<6; i++)
      {
	if (first)
        {
#ifdef GL_ARB_texture_compression
          if (isCompressed)
            glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB+uz0+i, level, internalFormat, data->nx, data->ny, 0,
                                   imgsize, img+i*imgsize);
          else
#endif
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB+uz0+i, level, internalFormat, data->nx, data->ny, 0,
                         format, type, img+i*imgsize);
        }
	else
        {
#ifdef GL_ARB_texture_compression
          if (isCompressed)
            glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB+uz0+i, level, ux0, uy0, unx, uny, internalFormat,
                                      imgsize, subimg+i*imgsize);
          else
#endif
              glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB+uz0+i, level, ux0, uy0, unx, uny,
                              format, type, subimg+i*imgsize);
        }
      }
    }
    else
    {
      if (first && clamp)
      {
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      }
      int subimgsize = lineSize * (uny==0?1:(uny + blocSize-1)/blocSize) * (unz==0?1:array?unz:(unz + blocSize-1)/blocSize);
      int imgsize = (img!=NULL)?subimgsize : ftl::Type::size(data->pixelType)*((data->nx+blocSize-1)/blocSize) * (data->ny==0?1:(data->ny + blocSize-1)/blocSize) * (data->nz==0?1:array?data->nz:(data->nz + blocSize-1)/blocSize);
      switch (texture->nbDim)
      {
      case 1:
	if (first)
	{
#if defined(DEBUG)
	  std::cout << "Uploading "<<tname<<" level "<<level
#ifdef GL_GENERATE_MIPMAP_SGIS
		    <<((data->level < 0 && GLEW_SGIS_generate_mipmap)?" mipmapped":"")
#endif
		    <<(isCompressed ? " compressed":"")
		    <<" texture "<<texture->userData.glid<<' '<<data->nx<<'x'<<Type::size(data->pixelType)*8<<std::endl;
#endif
#ifdef GL_ARB_texture_compression
          if (isCompressed)
            glCompressedTexImage1D(target, level, internalFormat, data->nx, 0,
                                   imgsize, img);
          else
#endif
            glTexImage1D(target, level, internalFormat, data->nx, 0,
                         format, type, img);
	}
	if (!first || update)
	{
#ifdef GL_ARB_texture_compression
          if (isCompressed)
            glCompressedTexSubImage1D(target, level, ux0, unx,
                                      format, subimgsize, subimg);
          else
#endif
            glTexSubImage1D(target, level, ux0, unx,
                            format, type, subimg);
	}
	break;
      case 2:
      {
	if (first)
	{
#if defined(DEBUG)
	  std::cout << "Uploading "<<tname<<" level "<<level
#ifdef GL_GENERATE_MIPMAP_SGIS
		    <<((data->level < 0 && GLEW_SGIS_generate_mipmap)?" mipmapped":"")
#endif
		    <<(isCompressed ? " compressed":"")
		    <<" texture "<<texture->userData.glid<<' '<<data->nx<<'x'<<data->ny<<'x'<<Type::size(data->pixelType)*8<<std::endl;
#endif
#ifdef GL_ARB_texture_compression
          if (isCompressed)
            glCompressedTexImage2D(target, level, internalFormat, data->nx, data->ny, 0,
                                   imgsize, img);
          else
#endif
            glTexImage2D(target, level, internalFormat, data->nx, data->ny, 0,
                         format, type, img);
	}
	if (!first || update)
	{
#ifdef GL_ARB_texture_compression
          if (isCompressed)
            glCompressedTexSubImage2D(target, level, ux0, uy0, unx, uny,
                                      format, subimgsize, subimg);
          else
#endif
            glTexSubImage2D(target, level, ux0, uy0, unx, uny,
                            format, type, subimg);
	}
	break;
      }
      case 3:
	if (first)
	{
#if defined(DEBUG)
	  std::cout << "Uploading "<<tname<<" level "<<level
#ifdef GL_GENERATE_MIPMAP_SGIS
		    <<((data->level < 0 && GLEW_SGIS_generate_mipmap)?" mipmapped":"")
#endif
		    <<(isCompressed ? " compressed":"")
		    <<" texture "<<texture->userData.glid<<' '<<data->nx<<'x'<<data->ny<<'x'<<data->nz<<'x'<<Type::size(data->pixelType)*8<<std::endl;
#endif
#ifdef GL_ARB_texture_compression
          if (isCompressed)
            glCompressedTexImage3D(target, level, internalFormat, data->nx, data->ny, data->nz, 0,
                                   imgsize, img);
          else
#endif
              glTexImage3D(target, level, internalFormat, data->nx, data->ny, data->nz, 0,
                           format, type, img);
	}
	if (!first || update)
	{
#ifdef GL_ARB_texture_compression
          if (isCompressed)
            glCompressedTexSubImage3D(target, level, ux0, uy0, uz0, unx, uny, unz,
                                      format, subimgsize, subimg);
          else
#endif
            glTexSubImage3D(target, level, ux0, uy0, uz0, unx, uny, unz,
			  format, type, subimg);
	}
	break;
      }
    }
  }

  glBindTexture(target, 0);
  if (convImg)
	  delete[] convImg;

  return true;
}

bool CgEngine::addVertexBuffer(VertexBuffer* vbuffer, const ChunkVertexBuffer* data, bool create)
{
#ifdef GL_ARB_vertex_buffer_object
  if (GLEW_ARB_vertex_buffer_object)
  {
    if (create)
    {
      glGenBuffersARB(1,&vbuffer->userData.glid);
#if defined(DEBUG)
      std::cout << "New GLVBuffer "<<vbuffer->userData.glid<<std::endl;
#endif
    }
    glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbuffer->userData.glid);
    if (data->dataSize()>vbuffer->userData.size)
    {
      glBufferDataARB(GL_ARRAY_BUFFER_ARB, data->dataSize(), data->data(),
		      (data->gen<0 ? GL_STATIC_DRAW_ARB : GL_STREAM_DRAW_ARB));
      vbuffer->userData.size = data->dataSize();
    }
    else if (data->dataSize()>0)
    {
      glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, data->dataSize(), data->data());
    }
    glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
    currentVertexBufferGLID = 0;
  }
#endif
  return true;
}

bool CgEngine::addIndexBuffer(IndexBuffer* ibuffer, const ChunkIndexBuffer* data, bool create)
{
#ifdef GL_ARB_vertex_buffer_object
  if (GLEW_ARB_vertex_buffer_object)
  {
    if (create)
    {
      glGenBuffersARB(1,&ibuffer->userData.glid);
#if defined(DEBUG)
      std::cout << "New GLIBuffer "<<ibuffer->userData.glid<<std::endl;
#endif
    }
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,ibuffer->userData.glid);

    if (data->dataSize()>ibuffer->userData.size)
    {
      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, data->dataSize(), data->data(),
		      (data->gen<0 ? GL_STATIC_DRAW_ARB : GL_STREAM_DRAW_ARB));
      ibuffer->userData.size = data->dataSize();
    }
    else if (data->dataSize()>0)
    {
      glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0, data->dataSize(), data->data());
    }

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
    currentIndexBufferGLID = 0;
  }
#endif
  return true;
}

bool CgEngine::addVertexShader(VertexShader* vshader, const ChunkVertexShader* data, bool create)
{
  if (!useShader)
	  return true;
  std::string sourceCode(data->data(), data->dataSize());
#if defined(DEBUG)
  std::cout << "Creating Vertex Shader."<<std::endl;
#endif

  vshader->userData.program = loadShader(vertexProfile, sourceCode.c_str());

  for (std::list< Primitive* >::iterator it=vshader->users.begin();it!=vshader->users.end();++it)
	  cgNewVShader(*it);
  return true;
}

bool CgEngine::addPixelShader(PixelShader* pshader, const ChunkPixelShader* data, bool create)
{
  if (!useShader)
	  return true;

  std::string sourceCode(data->data(), data->dataSize());
#if defined(DEBUG)
  std::cout << "Creating Pixel Shader."<<std::endl;
#endif
  pshader->userData.program = loadShader(pixelProfile, sourceCode.c_str());

  for (std::list< Primitive* >::iterator it=pshader->users.begin();it!=pshader->users.end();++it)
	  cgNewPShader(*it);
  return true;
}

bool CgEngine::delTexture(Texture* texture)
{
  if (texture->userData.glid != 0)
  {
	if( texture->tp >= TP_RENDER_TEXTURE_BASE )
	{
		Scene *s = getScene( TargetGroups::txTp2Tg( texture->tp ) );
		if(s)
		{
			glDeleteFramebuffersEXT(1, &s->userData.fbid);
			s->userData.fbid = 0;
			if( s->userData.dbid )
			{
				glDeleteRenderbuffersEXT(1, &s->userData.dbid );
				s->userData.dbid = 0;
			}

			// @todo delete/remove s?
		}
	}

	GLuint tid = texture->userData.glid;
#if defined(DEBUG)
	std::cout << "Deleting GLTexture "<<tid<<std::endl;
#endif
	glDeleteTextures(1,&tid);

  }
  return true;
}

bool CgEngine::delVertexBuffer(VertexBuffer* vbuffer)
{
  if (GLEW_ARB_vertex_buffer_object)
  {
    if (vbuffer->userData.glid)
    {
      glDeleteBuffersARB(1,&vbuffer->userData.glid);
      vbuffer->userData.glid = 0;
    }
  }
  return true;
}

bool CgEngine::delIndexBuffer(IndexBuffer* ibuffer)
{
  if (GLEW_ARB_vertex_buffer_object)
  {
    if (ibuffer->userData.glid)
    {
      glDeleteBuffersARB(1,&ibuffer->userData.glid);
      ibuffer->userData.glid = 0;
    }
  }
  return true;
}

bool CgEngine::delVertexShader(VertexShader* vshader)
{
  if (!useShader)
	  return true;

  if ((vshader->userData.program))
    cgDestroyProgram(vshader->userData.program);

  return true;
}

bool CgEngine::delPixelShader(PixelShader* pshader)
{
  if (!useShader)
	  return true;

  if ((pshader->userData.program))
    cgDestroyProgram(pshader->userData.program);

  return true;
}

// #################################################################################
// CG STUFF
// #################################################################################

void CgEngine::cgSetDefaults(CGprogram program)
{
  for (CGparameter param = cgGetFirstLeafParameter(program, CG_PROGRAM); param!=NULL; param=cgGetNextLeafParameter(param))
  {
    if (cgGetParameterVariability(param) == CG_UNIFORM)
    {
      if (cgGetParameterType(param) == CG_FLOAT4x4)
      {
		  // matrix
		  const char* name = cgGetParameterName(param);
		  bool model = false;
		  bool view = false;
		  bool proj = false;
		  bool inverse = false;
		  bool transpose = false;
		  const char* c = name;
		  if (!strncmp(c,"Model",5))
		  {
			  model = true;
			  c += 5;
		  }
		  if (!strncmp(c,"View",4))
		  {
			  view = true;
			  c += 4;
		  }
		  if (!strncmp(c,"Proj",4))
		  {
			  proj = true;
			  c += 4;
		  }
		  if (*c == 'I')
		  {
			  inverse = true;
			  ++c;
		  }
		  if (*c == 'T')
		  {
			  transpose = true;
			  ++c;
		  }

		  if (!*c) // the whole name was parsed
		  {
			  int mat;
			  if (model)
			  {
				  if (view)
				  {
					  if (proj)
						  mat = ChunkPrimParam::ModelViewProjection;
					  else
						  mat = ChunkPrimParam::ModelView;
				  }
				  else
					  mat = ChunkPrimParam::Model;
			  }
			  else
			  {
				  if (view)
				  {
					  if (proj)
						  mat = ChunkPrimParam::ViewProjection;
					  else
						  mat = ChunkPrimParam::View;
				  }
				  else
					  mat = ChunkPrimParam::Projection;
			  }
			  if (inverse)
				  mat += 2;

			  if (transpose)
				  mat += 1;

			  CgParamShader p;
			  p = ftl::TypedArray(ftl::Type::Enum, &mat, sizeof(mat));
			  p.userData.param = param;
			  cgSetParameter(p);
		  }
      }
      else
      {
        int nval = 0;
        const double* defaults = cgGetParameterValues(param,CG_DEFAULT,&nval);
        if (defaults != NULL && nval <=4)
        {
		  int i=0;

		  while (i<nval && defaults[i]==0.0)
			  ++i;

		  if (i<nval)
		  {
			switch (nval)
			{
			case 1: cgGLSetParameter1d(param, *defaults); break;
			case 2: cgGLSetParameter2dv(param, defaults); break;
			case 3: cgGLSetParameter3dv(param, defaults); break;
			case 4: cgGLSetParameter4dv(param, defaults); break;
			}
		  }
		}
      }
    }
  }
}

void CgEngine::cgInitParameters(VertexShader* shader)
{
  if (!useShader)
	  return;

  if (shader!=NULL && shader->userData.program)
    cgSetDefaults(shader->userData.program);
  else if (defaultVertexShader)
    cgSetDefaults(defaultVertexShader);

}

void CgEngine::cgInitParameters(PixelShader* shader)
{
  if (!useShader)
	  return;

  if (shader!=NULL && shader->userData.program)
    cgSetDefaults(shader->userData.program);
  else if (defaultPixelShader)
    cgSetDefaults(defaultPixelShader);

}

void CgEngine::cgSetParameter(const CgParamShader& param, bool cg)
{
  if (!useShader)
	  return;


  CGparameter paramid = param.userData.param;

  if (param.empty())
	  return;

  if(!cgIsParameter(paramid))
	  return;

  switch (param.type())
  {
  case ftl::Type::Int:
    if (param.n()==1)
      cgGLSetParameter1f(paramid, (float)*(int*)param.data());
    else
    {
      float tmp[param.n()];
      for (unsigned int i=0;i<param.n();i++) tmp[i] = (float)((int*)param.data())[i];
      cgGLSetParameterArray1f(paramid, 0, param.n(), tmp);
    }
    break;
  case ftl::Type::Float:
    if (param.n()==1)
      cgGLSetParameter1fv(paramid, (float*)param.data());
    else
      cgGLSetParameterArray1f(paramid, 0, param.n(), (float*)param.data());
    break;

  case ftl::Type::Double:
    if (param.n()==1)
      cgGLSetParameter1dv(paramid, (double*)param.data());
    else
      cgGLSetParameterArray1d(paramid, 0, param.n(), (double*)param.data());
    break;

  case ftl::Type::Vec2f:
    if (param.n()==1)
      cgGLSetParameter2fv(paramid, (float*)param.data());
    else
      cgGLSetParameterArray2f(paramid, 0, param.n(), (float*)param.data());
    break;

  case ftl::Type::Vec2d:
    if (param.n()==1)
      cgGLSetParameter2dv(paramid, (double*)param.data());
    else
      cgGLSetParameterArray2d(paramid, 0, param.n(), (double*)param.data());
    break;

  case ftl::Type::Vec3f:
    if (param.n()==1)
      cgGLSetParameter3fv(paramid, (float*)param.data());
    else
      cgGLSetParameterArray3f(paramid, 0, param.n(), (float*)param.data());
    break;

  case ftl::Type::Vec3d:
    if (param.n()==1)
      cgGLSetParameter3dv(paramid, (double*)param.data());
    else
      cgGLSetParameterArray3d(paramid, 0, param.n(), (double*)param.data());
    break;

  case ftl::Type::Vec4f:
    if (param.n()==1)
      cgGLSetParameter4fv(paramid, (float*)param.data());
    else
      cgGLSetParameterArray4f(paramid, 0, param.n(), (float*)param.data());
    break;

  case ftl::Type::Vec4d:
    if (param.n()==1)
      cgGLSetParameter4dv(paramid, (double*)param.data());
    else
      cgGLSetParameterArray4d(paramid, 0, param.n(), (double*)param.data());
    break;

  case ftl::Type::Enum:
  {
    CGGLenum matrix;
    CGGLenum transform;
    switch(*(int*)param.data())
    {
    case ChunkPrimParam::Model:
    {
    	cgGLSetMatrixParameterfr(paramid, curModel.ptr());
    	return;
    }
    case ChunkPrimParam::Model_Trans:
    {
    	cgGLSetMatrixParameterfc(paramid, curModel.ptr());
    	return;
    }
    case ChunkPrimParam::Model_Inv:
    {
    	Mat4x4f m; m.invert(curModel);
    	cgGLSetMatrixParameterfr(paramid, m.ptr());
    	return;
    }
    case ChunkPrimParam::Model_InvTrans:
    {
    	Mat4x4f m; m.invert(curModel);
    	cgGLSetMatrixParameterfc(paramid, m.ptr());
    	return;
    }
    case ChunkPrimParam::View:
    {
		cgGLSetMatrixParameterfr(paramid, curView.ptr());
		return;
    }
    case ChunkPrimParam::View_Trans:
    {
    	cgGLSetMatrixParameterfc(paramid, curView.ptr());
    	return;
    }
    case ChunkPrimParam::View_Inv:
    {
    	cgGLSetMatrixParameterfr(paramid, curViewInv.ptr());
    	return;
    }
    case ChunkPrimParam::View_InvTrans:
    {
    	// note the function is ...Parameterfc (passing transposed / column-wise)
    	cgGLSetMatrixParameterfc(paramid, curViewInv.ptr());
    	return;
    }
    case ChunkPrimParam::ViewProjection:
    {
    	cgGLSetMatrixParameterfr(paramid, curViewProj.ptr());
    	return;
    }
    case ChunkPrimParam::ViewProjection_Trans:
    {
    	// note the function is ...Parameterfc (passing transposed / column-wise)
    	cgGLSetMatrixParameterfc(paramid, curViewProj.ptr());
    	return;
    }
    case ChunkPrimParam::ViewProjection_Inv:
    {
    	cgGLSetMatrixParameterfr(paramid, curViewProjInv.ptr());
    	return;
    }
    case ChunkPrimParam::ViewProjection_InvTrans:
    {
    	cgGLSetMatrixParameterfc(paramid, curViewProjInv.ptr());
    	return;
    }

    case ChunkPrimParam::ModelView:
    	matrix = CG_GL_MODELVIEW_MATRIX;
    	transform = CG_GL_MATRIX_IDENTITY;
    	break;
    case ChunkPrimParam::ModelView_Trans:
    	matrix = CG_GL_MODELVIEW_MATRIX;
    	transform = CG_GL_MATRIX_TRANSPOSE;
    	break;
    case ChunkPrimParam::ModelView_Inv:
    	matrix = CG_GL_MODELVIEW_MATRIX;
    	transform = CG_GL_MATRIX_INVERSE;
    	break;
    case ChunkPrimParam::ModelView_InvTrans:
    	matrix = CG_GL_MODELVIEW_MATRIX;
    	transform = CG_GL_MATRIX_INVERSE_TRANSPOSE;
    	break;
    case ChunkPrimParam::Projection:
    	matrix = CG_GL_PROJECTION_MATRIX;
    	transform = CG_GL_MATRIX_IDENTITY;
    	break;
    case ChunkPrimParam::Projection_Trans:
    	matrix = CG_GL_PROJECTION_MATRIX;
    	transform = CG_GL_MATRIX_TRANSPOSE;
    	break;
    case ChunkPrimParam::Projection_Inv:
    	matrix = CG_GL_PROJECTION_MATRIX;
    	transform = CG_GL_MATRIX_INVERSE;
    	break;
    case ChunkPrimParam::Projection_InvTrans:
    	matrix = CG_GL_PROJECTION_MATRIX;
    	transform = CG_GL_MATRIX_INVERSE_TRANSPOSE;
    	break;
    case ChunkPrimParam::ModelViewProjection:
    	matrix = CG_GL_MODELVIEW_PROJECTION_MATRIX;
    	transform = CG_GL_MATRIX_IDENTITY;
    	break;
    case ChunkPrimParam::ModelViewProjection_Trans:
    	matrix = CG_GL_MODELVIEW_PROJECTION_MATRIX;
    	transform = CG_GL_MATRIX_TRANSPOSE;
    	break;
    case ChunkPrimParam::ModelViewProjection_Inv:
    	matrix = CG_GL_MODELVIEW_PROJECTION_MATRIX;
    	transform = CG_GL_MATRIX_INVERSE;
    	break;
    case ChunkPrimParam::ModelViewProjection_InvTrans:
    	matrix = CG_GL_MODELVIEW_PROJECTION_MATRIX;
    	transform = CG_GL_MATRIX_INVERSE_TRANSPOSE;
    	break;
    default:
    	return;
    }
    cgGLSetStateMatrixParameter(paramid, matrix, transform);
    break;
  }
  default:
    if (ftl::Type::isMatrix(param.type()) && ftl::Type::toSingle(param.type())==ftl::Type::Float)
    {
      if (param.n()==1)
    	  cgGLSetMatrixParameterfr(paramid, (float*)param.data());
      else
    	  cgGLSetMatrixParameterArrayfr(paramid, 0, param.n(), (float*)param.data());
      break;
    }
    else if (ftl::Type::isMatrix(param.type()) && ftl::Type::toSingle(param.type())==ftl::Type::Double)
    {
      if (param.n()==1)
    	  cgGLSetMatrixParameterdr(paramid, (double*)param.data());
      else
    	  cgGLSetMatrixParameterArraydr(paramid, 0, param.n(), (double*)param.data());
      break;
    }
    else
    {
      // TODO: Conversions?
      std::cerr << "Unsupported Cg parameter type "<< ftl::Type::name(param.type()) << std::endl;
      break;
    }
  }
}

void CgEngine::cgSetParameter(const CgParamTexture& param, bool cg)
{
  if (!useShader)
	  return;

  if ( param.texture == NULL || !param.texture->created)
	 return;

  CGparameter paramid = param.userData.param;
  if(!cgIsParameter(paramid))
	  return;


  if (param.userData.unit!=0)
  {
    glActiveTexture(GL_TEXTURE0+param.userData.unit);
    glClientActiveTexture(GL_TEXTURE0+param.userData.unit);
  }

  glBindTexture(param.texture->userData.target, param.texture->userData.glid);
  glEnable(param.texture->userData.target);

  if (param.userData.unit!=0)
  {
    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
  }
}

void CgEngine::cgResetParameter(const CgParamTexture& param, bool cg)
{
  if (!useShader)
	  return;

  if (param.texture == NULL or !param.texture->created)
	  return;

  CGparameter paramid = param.userData.param;

  if(!cgIsParameter(paramid))
	  return;

  if (param.userData.unit!=0)
  {
    glActiveTexture(GL_TEXTURE0+param.userData.unit);
    glClientActiveTexture(GL_TEXTURE0+param.userData.unit);
  }

  glBindTexture(param.texture->userData.target, 0);
  glDisable(param.texture->userData.target);

  if (param.userData.unit!=0)
  {
    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
  }
}

void CgEngine::cgSetParameter(const CgParamVBuffer& param, bool cg)
{
  if (param.buffer == NULL || !param.buffer->created)
	  return;

  if (param.buffer->empty())
	  return;

  if (param.buffer->levels[0].empty())
	  return;

  const ChunkVertexBuffer* buffer = param.buffer->levels[0];

  if (param.numData >= buffer->nbData)
	  return;

  CGparameter paramid = param.userData.param;
  if(cg && useShader && !cgIsParameter(paramid))
	  return;

  GLint fsize = ftl::Type::nx(buffer->dataType[param.numData]);
  GLenum type;

  switch (ftl::Type::toSingle(buffer->dataType[param.numData]))
  {
  case ftl::Type::Byte:
    type = GL_UNSIGNED_BYTE;
    break;
  case ftl::Type::Short:
    type = GL_SHORT;
    break;
  case ftl::Type::Int:
    type = GL_INT;
    break;
#ifdef GL_HALF_FLOAT_NV
  case ftl::Type::Half:
    // TODO: Check for extension
    type = GL_HALF_FLOAT_NV;
    break;
#endif
  case ftl::Type::Float:
    type = GL_FLOAT;
    break;
  case ftl::Type::Double:
    type = GL_DOUBLE;
    break;
  default:
    // TODO: Conversions?
    return;
  }
  GLsizei stride = buffer->vertexSize;
  int offset = param.v0*stride;
  for (int i=0;i<param.numData;i++)
    offset += ftl::Type::size(buffer->dataType[i]);
  const GLvoid* pointer = ((const char*)buffer->data())+offset;

#ifdef GL_ARB_vertex_buffer_object
  if (GLEW_ARB_vertex_buffer_object)
  {
    GLuint glid = param.buffer->userData.glid;
    if (glid != currentVertexBufferGLID)
    {
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, glid);
      currentVertexBufferGLID = glid;
    }
    if (glid)
      pointer = ((const char*)NULL)+offset;
  }
#endif

  if (cg && useShader)
  {
    cgGLSetParameterPointer(paramid, fsize, type, stride, (GLvoid*)pointer);
    cgGLEnableClientState(paramid);
  }
  else
  {
    glVertexPointer(fsize, type, stride, pointer);
    glEnableClientState(GL_VERTEX_ARRAY);
  }
}

void CgEngine::cgResetParameter(const CgParamVBuffer& param, bool cg)
{
  if (param.buffer == NULL or !param.buffer->created)
	  return;

  if (param.buffer->empty())
	  return;

  if (param.buffer->levels[0].empty())
	  return;
  const ChunkVertexBuffer* buffer = param.buffer->levels[0];
  if (param.numData >= buffer->nbData)
	  return;

  CGparameter paramid = param.userData.param;
  if(cg && useShader && !cgIsParameter(paramid))
	  return;

  if (cg && useShader)
  {
    cgGLDisableClientState(paramid);
  }
}

void CgEngine::cgNewVBParam(const std::string& name, CgParamVBuffer* param, CgPrimitive* p)
{
  if (p->vShader != NULL && p->vShader->created && p->vShader->userData.program)
    param->userData.param = cgGetNamedParameter( p->vShader->userData.program, name.c_str() );
  else if (defaultVertexShader)
    param->userData.param = cgGetNamedParameter( defaultVertexShader, name.c_str() );

  if (!param->userData.param)
	  std::cerr << "Unknown vertex attribute "<<name<<std::endl;
}

void CgEngine::cgNewVParam(const std::string& name, CgParamVShader* param, CgPrimitive* p)
{
  if (name.empty())
	  return;

  if (p->vShader != NULL && p->vShader->created && p->vShader->userData.program)
    param->userData.param = cgGetNamedParameter( p->vShader->userData.program, name.c_str() );
  else if (defaultVertexShader)
    param->userData.param = cgGetNamedParameter( defaultVertexShader, name.c_str() );

  if (!param->userData.param)
	  std::cerr << "Unknown vertex shader parameter "<<name<<std::endl;
//  else
//    std::cout << "VParam "<<name<<": type="<<cgGetTypeString(cgGetParameterType(param->userData.param))
//	      <<" semantic="<<cgGetParameterSemantic(param->userData.param)<<std::endl;
}

void CgEngine::cgNewPParam(const std::string& name, CgParamPShader* param, CgPrimitive* p)
{
  if (name.empty())
	  return;

  if (p->pShader != NULL && p->pShader->created && p->pShader->userData.program)
    param->userData.param = cgGetNamedParameter( p->pShader->userData.program, name.c_str() );
  else if (defaultPixelShader)
    param->userData.param = cgGetNamedParameter( defaultPixelShader, name.c_str() );

  if (!param->userData.param)
	  std::cerr << "Unknown pixel shader parameter "<<name<<std::endl;
//  else
//	  std::cout << "PParam "<<name<<": type="<<cgGetTypeString(cgGetParameterType(param->userData.param))
//		 <<" semantic="<<cgGetParameterSemantic(param->userData.param)<<std::endl;
}

void CgEngine::cgNewTParam(const std::string& name, CgParamTexture* param, CgPrimitive* p)
{
   if (name.empty())
	   return;

	if (p->pShader != NULL && p->pShader->created && p->pShader->userData.program)
		param->userData.param = cgGetNamedParameter(p->pShader->userData.program, name.c_str());
	else if (defaultPixelShader)
		param->userData.param = cgGetNamedParameter(defaultPixelShader,name.c_str());

	if (!param->userData.param)
		std::cerr << "Unknown texture parameter " << name << " in pixel shader"
				  << std::endl;
	else
	{
		param->userData.unit = cgGLGetTextureEnum(param->userData.param) - GL_TEXTURE0;
#if defined(DEBUG)
		std::cout << "Texture Parameter " << name << " on unit "
				  << param->userData.unit << std::endl;
#endif
		return;
	}

	// try with vertex shader
	if (p->vShader != NULL && p->vShader->created && p->vShader->userData.program)
		param->userData.param = cgGetNamedParameter(p->vShader->userData.program, name.c_str());
	else if (defaultVertexShader)
		param->userData.param = cgGetNamedParameter(defaultVertexShader,name.c_str());

	if (!param->userData.param)
		std::cerr << "Unknown texture parameter " << name
				  << " in vertex shader" << std::endl;
//	else
//	{
//		std::cout << "Texture Parameter " << name << " on unit " << param->userData.unit << std::endl;
//		param->userData.unit = cgGLGetTextureEnum(param->userData.param) - GL_TEXTURE0;
//	}
}

void CgEngine::cgDelVBParam(const std::string& name, CgParamVBuffer* param, CgPrimitive* p)
{
}

void CgEngine::cgDelVParam(const std::string& name, CgParamVShader* param, CgPrimitive* p)
{
}

void CgEngine::cgDelPParam(const std::string& name, CgParamPShader* param, CgPrimitive* p)
{
}

void CgEngine::cgDelTParam(const std::string& name, CgParamTexture* param, CgPrimitive* p)
{
}

void CgEngine::cgNewVShader(CgPrimitive* p)
{
  for (std::map<std::string,CgParamVBuffer>::iterator j=p->vBuffer.begin();j!=p->vBuffer.end();++j)
  {
    cgDelVBParam(j->first,&j->second,p);
    cgNewVBParam(j->first,&j->second,p);
  }

  for (paramIterator j=p->paramVShader.begin();j!=p->paramVShader.end();++j)
  {
    cgDelVParam(j->first,&j->second,p);
    cgNewVParam(j->first,&j->second,p);
  }
}

void CgEngine::cgNewPShader(CgPrimitive* p)
{
  for (paramIterator j=p->paramPShader.begin();j!=p->paramPShader.end();++j)
  {
    cgDelPParam(j->first,&j->second,p);
    cgNewPParam(j->first,&j->second,p);
  }

  for (std::map<std::string,CgParamTexture>::iterator j=p->textures.begin();j!=p->textures.end();++j)
  {
    cgDelTParam(j->first,&j->second,p);
    cgNewTParam(j->first,&j->second,p);
  }
}


void CgEngine::reloadShaders()
{
	if (!useShader)
	  return;

	for (PixelShaderList::iterator it = pShaders.begin(); it != pShaders.end(); ++it)
	{
		if (!it->second.created)
			continue;
		if (it->second.empty())
			continue;

		MsgChunk<ChunkPixelShader> data = it->second.firstLevel();
		if (data.valid())
		{
			std::string source0(data->data(), data->dataSize());
			std::string::size_type filebegin = source0.find("//file://");
			if (filebegin != std::string::npos)
			{
				std::string filename(source0, filebegin + 9, source0.find('\n',filebegin + 9) - (filebegin + 9));
				std::cout << "Trying to reload " << filename << std::endl;

				std::string source(source0, 0, filebegin);
				source += "//file://" + filename + "\n";
				if (!ChunkRenderWriter::readShader(source, filename))
					std::cout << "Open failed" << std::endl;
				else
				{
					if (it->second.userData.program)
						source0 = cgGetProgramString(it->second.userData.program, CG_PROGRAM_SOURCE);

					if (source == source0)
						std::cout << "Source code not changed" << std::endl;
					else
					{
						CGprogram program = loadShader(pixelProfile,source.c_str());
						if (program)
						{
							if (it->second.userData.program)
								cgDestroyProgram(it->second.userData.program);

							it->second.userData.program = program;

							for (std::list<Primitive*>::iterator p = it->second.users.begin(); p!= it->second.users.end(); ++p)
								cgNewPShader(*p);

							std::cout << "Shader " << filename
									  << " updated (used by " << (*it).second.users.size()
									  << " primitives)" << std::endl;
						}
					}
				}
			}
		}
	  }

	  for (VertexShaderList::iterator it=vShaders.begin(); it!=vShaders.end(); ++it)
	  {
	    if (!it->second.created)
	    	continue;

	    if (it->second.empty())
	    	continue;

	    MsgChunk<ChunkVertexShader> data = it->second.firstLevel();

	    if (data.valid())
	    {
	      std::string source0(data->data(), data->dataSize());
	      std::string::size_type filebegin = source0.find("//file://");
	      if (filebegin != std::string::npos)
	      {
			std::string filename(source0, filebegin + 9, source0.find('\n',filebegin + 9) - (filebegin + 9));
			std::cout << "Trying to reload " << filename << std::endl;

			std::string source(source0, 0, filebegin);
			source += "//file://" + filename + "\n";

			if (!ChunkRenderWriter::readShader(source, filename))
				std::cout << "Open failed" << std::endl;
			else
			{
				if (it->second.userData.program)
					source0 = cgGetProgramString(it->second.userData.program, CG_PROGRAM_SOURCE);

				if (source == source0)
					std::cout << "Source code not changed" << std::endl;
				else
				{
					CGprogram program = loadShader(vertexProfile,
							source.c_str());
					if (program)
					{
						if (it->second.userData.program)
							cgDestroyProgram(it->second.userData.program);

						it->second.userData.program = program;

						for (std::list<Primitive*>::iterator p = it->second.users.begin(); p!= it->second.users.end(); ++p)
							cgNewVShader(*p);

						std::cout << "Shader "            << filename
								  << " updated (used by " << (*it).second.users.size()
								  << " primitives)"       << std::endl;
					}
				}
			}
	      }
		}
	  }
}


} // namespace cg

} // namespace render

} // namespace flowvr
