/******* COPYRIGHT ************************************************
*                                                                 *
*                         FlowVR Render                           *
*                   Parallel Rendering Library                    *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
*  ALL RIGHTS RESERVED.                                           *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING-LIB file for further information.                       *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Clement Menier.                                              *
*                                                                 *
*******************************************************************
*                                                                 *
* File: ./include/flowvr/render/cg/cgengine.h                     *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_CG_CGENGINE_H
#define FLOWVR_RENDER_CG_CGENGINE_H

#include "flowvr/render/engine.h"

#include <GL/glew.h>
#include <Cg/cgGL.h>

#include <ftl/type.h>
#include <flowvr/utils/filepath.h>


namespace flowvr
{

namespace render
{

namespace cg
{

	class CgData
	{
	public:

	  struct VertexBuffer
	  {
		GLuint glid; ///< OpenGL VBO ID
		int size; ///< Current buffer size
		VertexBuffer() : glid(0), size(0) {}
	  };

	  struct IndexBuffer
	  {
		GLuint glid; ///< OpenGL VBO ID
		int size; ///< Current buffer size
		IndexBuffer() : glid(0), size(0) {}
	  };

	  struct VertexShader
	  {
		CGprofile profile;
		CGprogram program;
		VertexShader() : profile((CGprofile)0), program((CGprogram)0) {}
	  };

	  struct PixelShader
	  {
		CGprofile profile;
		CGprogram program;
		PixelShader() : profile((CGprofile)0), program((CGprogram)0) {}
	  };

	  struct Texture
	  {
		GLuint glid; ///< OpenGL texture ID
		GLuint target; ///< GL_TEXTURE_[1-3]D, GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_RECTANGLE_NV
		GLint internalFormat;
		GLenum type;
		int nx;
		int ny;
		int nz;
		Texture() : glid(0), target(0), internalFormat(0), type(0), nx(0), ny(0), nz(0) {}
	  };

	  struct Primitive
	  {
	  };

	  struct PrimitiveVertexBuffer
	  {
		CGparameter param;
		PrimitiveVertexBuffer() : param((CGparameter)0) {}
	  };

	  struct PrimitiveIndexBuffer
	  {
	  };

	  struct PrimitiveTexture
	  {
		CGparameter param;
		int unit;
		PrimitiveTexture() : param((CGparameter)0), unit(0) {}
	  };

	  struct ParamShader
	  {
		CGparameter param;
		ParamShader() : param((CGparameter)0) {}
	  };

	  typedef ParamShader ParamVShader;
	  typedef ParamShader ParamPShader;

	  struct ParamOpenGL
	  {
		int index;
		ParamOpenGL() : index(-1) {}
	  };

	  struct ParamDisplay
	  {
		ParamDisplay() {}
	  };

	  struct Target
	  {
		GLuint fbid, dbid;
		int iteration;
		Target() : fbid(0), dbid(0), iteration(-1) {}
	  };

	}; // end class CgData


	// ------------------------------------------------------------------------
	// typedefs to simplify some typing
	// ------------------------------------------------------------------------
	typedef PixelShaderTemplate<CgData> CgPixelShader;
	typedef VertexShaderTemplate<CgData> CgVertexShader;
	typedef VertexBufferTemplate<CgData> CgVertexBuffer;
	typedef IndexBufferTemplate<CgData> CgIndexBuffer;
	typedef TextureTemplate<CgData> CgTexture;
	typedef PrimitiveTemplate<CgData> CgPrimitive;
	typedef CgPrimitive::VBData CgParamVBuffer;
	typedef CgPrimitive::TData CgParamTexture;
	typedef ParamVal<CgData::ParamShader> CgParamShader;
	typedef ParamVal<CgData::ParamVShader> CgParamVShader;
	typedef ParamVal<CgData::ParamPShader> CgParamPShader;
	typedef ParamVal<CgData::ParamOpenGL> CgParamOpenGL;
	typedef ParamVal<CgData::ParamDisplay> CgParamDisplay;


	/**
	 * @brief the render engine implementation based on Cg.
	 */
	class CgEngine : public BaseEngine<CgData>
	{
	 public:
	  CgEngine();
	  ~CgEngine();

	  virtual bool init();

	  // ------------------------------------------------------------
	  // StateGL stuff
	  // ------------------------------------------------------------

	  enum IndexParam
	  {
		P_COLOR_WRITE = 0,
		P_BLEND,
		P_DEPTH_TEST,
		P_DEPTH_WRITE,
		P_CULL_FACE,
		P_STENCIL_TEST,
		P_DEPTH_FUNC,
		P_BLEND_MODE,
		P_FLAT,
		P_ALPHA_TEST,
		P_POLYGON_OFFSET,
		P_CLIP_PLANES,
		P_FOG,
		P_LIGHTING,
		P_TEX_GEN_S,
		P_TEX_GEN_R,
		P_TEX_GEN_T,
		P_SCISSOR,
		P_NB
	  };

	  struct StateGL
	  {
		StateGL()
		: name ("")
		, type(Type::Bool)
		, ok(false)
		, nbChanges(0)
		, m_setF(NULL)
		, glid(0)
		{
		  val.i = cur.i = def.i = 0;
		}


		const char* name;
		int  type;
		int  nbChanges;
		bool ok;
		GLenum glid;

		typedef void (*setStateCb)( const StateGL &state );
		setStateCb m_setF;



		union Val
		{
		  bool b;
		  int i;
		  float f;
		  float v[4];
		} val, cur, def;



		bool modif() const
		{
		  return !ok || (val.i != cur.i);
		}

		void init(const char* n, bool b, setStateCb cb, GLenum id = 0)
		{
		  name = n;
		  type = Type::Bool;
		  def.b = b;
		  m_setF = cb;
		  glid = id;
		}

		void init(const char* n, int i, setStateCb cb, GLenum id = 0)
		{
		  name = n;
		  type = Type::Int;
		  def.i = i;
		  m_setF = cb;
		  glid = id;
		}

		void init(const char* n, float f, setStateCb cb, GLenum id = 0)
		{
		  name  = n;
		  type  = Type::Float;
		  def.f = f;
		  m_setF = cb;
		  glid = id;
		}

		void init( const char *n, float v4[4], setStateCb cb, GLenum id = 0)
		{
			  name  = n;
			  type  = Type::Vec4f;
			  memcpy( def.v, v4, 4*sizeof(float) );
			  m_setF = cb;
			  glid = id;
		}

		void reset()
		{
		  val = def;
		}

		void validate()
		{
		  cur = val;
		  ok = true;
		  ++nbChanges;
		}

		void invalidate()
		{
		  ok = false;
		}

		bool set(const TypedArray& v)
		{
		  switch (type)
		  {
		  case Type::Bool:	return v.assign(val.b);
		  case Type::Int:	return v.assign(val.i);
		  case Type::Float:	return v.assign(val.f);
		  case Type::Vec4f:
			  {
				  val.v[0] = v.get<float>(0);
				  val.v[1] = v.get<float>(1);
				  val.v[2] = v.get<float>(2);
				  val.v[3] = v.get<float>(3);
			    return true;
			  }
		  }
		  return false;
		}


	  } glstate[P_NB];

	  void applyGLState(bool all=false);
	  void resetGLState();
	  void invalidGLState();
	  void dumpGLState();


	  // ------------------------------------------------------------

	  Mat4x4f curModel;
	  Mat4x4f curView, curViewInv;
	  Mat4x4f curProj, curProjInv;
	  Mat4x4f curViewProj, curViewProjInv;

	  virtual void viewProjAndProjUpdate( const Mat4x4f &viewProj, const Mat4x4f &proj );

	  typedef void (*bgrender)(void*);
	  void setBackgroundRenderHook( bgrender f, void *data ) { m_bgrender = f; m_bgdata = data; }
	  bgrender getBackgroundRenderHook() const { return m_bgrender; }

	  bgrender m_bgrender;
	  void    *m_bgdata;


	  // ------------------------------------------------------------
	  // RENDER STUFF
	  // ------------------------------------------------------------
	  virtual void render(float aspectRatio = 1);

	  // ------------------------------------------------------------
	  // STATISTICS FOR NON-SHADER BASED RENDERING!
	  // ------------------------------------------------------------

	  size_t nbPoints;    ///< Number of points rendered
	  size_t nbLines;     ///< Number of lines rendered
	  size_t nbTriangles; ///< Number of triangles rendered

	  size_t nbChangesVShader;
	  size_t nbChangesPShader;

	  bool getDoStats() const;
	  void setDoStats(bool bDo);

	  // ------------------------------------------------------------
	  // DEBUG MODE
	  // ------------------------------------------------------------
	  enum
	  {
		DEBUG_STD = 0,
		DEBUG_BBOX,
		DEBUG_AABB,
		DEBUG_WIREFRAME,
		NBDEBUGMODES
	  };

	  int  debugMode;



	  // ------------------------------------------------------------
	  // UTILITY
	  // ------------------------------------------------------------
	  virtual void projPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
	  virtual void reloadShaders();

	 protected:

	  /// @name User-defined callbacks
	  /// @{
	  virtual bool addPrimitive(Primitive* prim, Primitive* model);
	  virtual bool delPrimitive(Primitive* prim);
	  virtual bool setVShader(Primitive* prim, VertexShader* shader);
	  virtual bool setPShader(Primitive* prim, PixelShader* shader);
	  virtual bool setVBuffer(Primitive* prim, const char* name, Primitive::VBData* dest, VertexBuffer*buffer, bool create);
	  virtual bool setTexture(Primitive* prim, const char* name, Primitive::TData* dest, Texture* texture, bool create);
	  virtual bool setParamVShader(Primitive* prim, const char* name, Primitive::ParamVShader* dest, const TypedArray& val, bool create);
	  virtual bool setParamPShader(Primitive* prim, const char* name, Primitive::ParamPShader* dest, const TypedArray& val, bool create);
	  virtual bool setParamOpenGL(Primitive* prim, const char* name, Primitive::ParamOpenGL* dest, const TypedArray& val, bool create);
	  virtual bool setParamDisplay(Primitive* prim, const char* name, Primitive::ParamDisplay* dest, const TypedArray& val, bool create);
	  virtual bool addTexture(Texture* texture, const ChunkTexture* data, bool create);
	  virtual bool updateTexture(Texture* texture, const ChunkTextureUpdate* data);
	  virtual bool addVertexBuffer(VertexBuffer* vbuffer, const ChunkVertexBuffer* data, bool create);
	  virtual bool addIndexBuffer(IndexBuffer* ibuffer, const ChunkIndexBuffer* data, bool create);
	  virtual bool addVertexShader(VertexShader* vshader, const ChunkVertexShader* data, bool create);
	  virtual bool addPixelShader(PixelShader* pshader, const ChunkPixelShader* data, bool create);
	  virtual bool delTexture(Texture* texture);
	  virtual bool delVertexBuffer(VertexBuffer* vbuffer);
	  virtual bool delIndexBuffer(IndexBuffer* ibuffer);
	  virtual bool delVertexShader(VertexShader* vshader);
	  virtual bool delPixelShader(PixelShader* pshader);
	  /// @}


	  virtual bool supportPixelShader(ShaderLanguage language);
	  virtual bool supportVertexShader(ShaderLanguage language);



	  // ------------------------------------------------------------
	  // CG SPECIAL STUFF
	  // ------------------------------------------------------------
	  void cgInitParameters(VertexShader* shader);
	  void cgInitParameters(PixelShader* shader);
	  void cgSetParameter(const CgParamTexture& param, bool cg=true);
	  void cgResetParameter(const CgParamTexture& param, bool cg=true);
	  void cgSetParameter(const CgParamShader& param, bool cg=true);
	  void cgSetParameter(const CgParamVBuffer& param, bool cg=true);
	  void cgResetParameter(const CgParamVBuffer& param, bool cg=true);
	  void cgSetDefaults(CGprogram program);

	  void cgNewVBParam(const std::string& name, CgParamVBuffer* param, CgPrimitive* p);
	  void cgNewVParam(const std::string& name, CgParamVShader* param, CgPrimitive* p);
	  void cgNewPParam(const std::string& name, CgParamPShader* param, CgPrimitive* p);
	  void cgNewTParam(const std::string& name, CgParamTexture* param, CgPrimitive* p);
	  void cgDelVBParam(const std::string& name, CgParamVBuffer* param, CgPrimitive* p);
	  void cgDelVParam(const std::string& name, CgParamVShader* param, CgPrimitive* p);
	  void cgDelPParam(const std::string& name, CgParamPShader* param, CgPrimitive* p);
	  void cgDelTParam(const std::string& name, CgParamTexture* param, CgPrimitive* p);
	  void cgNewVShader(CgPrimitive* p);
	  void cgNewPShader(CgPrimitive* p);

	  /// @}

	   CGcontext context;
	   CGprofile vertexProfile;
	   CGprofile pixelProfile;

	   CGprogram defaultVertexShader;
	   CGprogram defaultPixelShader;

	   CGprogram loadShaderFromFile(CGprofile profile, std::string filename);
	   CGprogram loadShader(CGprofile profile, const char *source);

	   virtual Scene *createTargetScene( unsigned int nSceneTp );

	 public:
	   void dumpScene( int index ) const;
	   void toggleScene( int index );

	 private:
	   friend class TextureTargetScene;

	   virtual bool beginRenderToTexture( Scene *s, ID tid, float &aspectRatio, GLint vp[4], int lastIt, bool &setVp );
	   virtual void doBackground(const Primitive *camera);


	   virtual void doRender( float aspectRatio, Scene &, int target, bool dump );
	   virtual void endRenderToTexture(Scene *s, ID tid);

	   virtual void debugRender();
	   virtual void renderPrimitive( const Primitive *p );
	   virtual CGprogram bindShader(CGprogram vs, CGprogram def, CGprofile prof, size_t &cnt );

	   virtual void stateInit( const Primitive *p );
	   virtual void stateExit( const Primitive *p );

	   virtual bool saveTexture( const Texture *, const std::string &sFileName ) const;

	   GLuint currentVertexBufferGLID;
	   GLuint currentIndexBufferGLID;
	   bool   currentPrimitiveRestartEnabled;
	   GLuint currentPrimitiveRestartIndex;

	   bool useShader, doStats;

		std::string m_resolvedPath;
		flowvr::utils::FilePath path;
		VertexShader *m_lastVShader;
		PixelShader  *m_lastPShader;
	};

} // namespace cg

} // namespace render

} // namespace flowvr

#endif
