/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                   Balzac FlowVR render frontend                 *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA.  ALL RIGHTS RESERVED.                                    *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING file for further information.                           *
*                                                                 *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: bgpicture.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>
#include <flowvr/portutils/portmodule.h>
#include <flowvr/portutils/portparameter.h>

#include <flowvr/render/balzac/servicelayer/servicelayer.h>
#include <flowvr/render/balzac/servicelayer/display.h>

#include <flowvr/render/chunkrenderwriter.h>

#include <limits>

using namespace flowvr::render::balzac;
using namespace flowvr;
using namespace flowvr::render;
using namespace flowvr::portutils;
using namespace ftl;


extern "C" void getParameters( flowvr::portutils::ARGS &args )
{
	// add parameters here
	args["IMAGENAME"] = Parameter(Parameter::P_STRING, Parameter::PS_NONE, "image file to load for texture" );
}

namespace
{

	class bgpicturePlugHandler : public flowvr::portutils::NoPortHandler
	{
	public:
		bgpicturePlugHandler( ServiceLayer *pService, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::NoPortHandler()
		, m_service(pService)
		, m_strTextureName()
		{
			try
			{
				m_strTextureName = args("IMAGENAME").getValueString();
			}
			catch( std::exception &e )
			{
			}
		}

		virtual bool once(PortComm::PortMap &portMap, flowvr::Allocator &alloc)
		{
			ChunkRenderWriter scene;
			ModuleAPI &module = *m_service->getParent().getModuleAPI();

			// create mirror geometry and target groups
			// Get IDs for all our primitives and resources
			ID id     = module.generateID(); // Id of the primitive
			ID idT    = module.generateID(); // Id of the Texture
			ID idVB   = module.generateID(); // Id of the Vertex Buffer
			ID idIB   = module.generateID(); // Id of the Index Buffer
			ID idVS   = module.generateID(); // Id of the Vertex Shader
			ID idPS   = module.generateID(); // Id of the Pixel Shader


			if(!scene.loadTexture(idT, m_strTextureName ))
			{
				std::cerr << "background image [" << m_strTextureName << "] was NOT FOUND" << std::endl;
				return false;
			}


			// Create vdata+index buffers for a quad
			// We use  two vdata buffers (position and uv texture position). This array provides the number of vdata buffers and their types
			int vdataBufferTypes[2] = {Type::Vec3f, Type::Vec2f};
			// Create the vdata buffers (4 points and 2 buffers)
			ChunkVertexBuffer* vb = scene.addVertexBuffer(idVB, 4, 2, vdataBufferTypes);
			// Create the index buffer (1 Quad = 4 values of type Byte)
			int indexBufferType = Type::Byte;
			ChunkIndexBuffer* ib = scene.addIndexBuffer(idIB, 4, indexBufferType, ChunkIndexBuffer::Quad);

			// A structure defining the type of vertex we use with position and uv texture position
			struct Vertex
			{
				Vec3f pos;
				Vec2f uv;
			};

			// Fill vdata and index buffers
			Vertex* vertex = (Vertex*)vb->data();
			vertex[0].pos = Vec3f(-1,-1,0); vertex[0].uv = Vec2f(0,1);
			vertex[1].pos = Vec3f( 1,-1,0); vertex[1].uv = Vec2f(1,1);
			vertex[2].pos = Vec3f( 1, 1,0); vertex[2].uv = Vec2f(1,0);
			vertex[3].pos = Vec3f(-1, 1,0); vertex[3].uv = Vec2f(0,0);

			Vec<4,char>* idata = (Vec<4,char>*) ib->data();
			idata[0] = Vec<4,char>(0, 1, 2, 3);

			// Set shaders parameters
			// See data/shaders/texture_v.cg and data/shaders/texture_p.cg
			scene.loadVertexShader(idVS, "shaders/texture_v.cg");
			scene.loadPixelShader(idPS,  "shaders/texture_p.cg");

			// Create new primitive
			scene.addPrimitive(id, "background", 0, TP_OBJECT, 1<<0 );

			scene.addParamID(id, ChunkPrimParam::VSHADER,"",idVS);
			scene.addParamID(id, ChunkPrimParam::PSHADER,"",idPS);
			// Link vdata buffer idVB to primitive id
			// Position, UV Texture position are given by vertex data buffer idVB
			scene.addParamID(id, ChunkPrimParam::VBUFFER_ID,"position", idVB);
			scene.addParamID(id, ChunkPrimParam::VBUFFER_ID,"texcoord0",idVB);
			// Position is 1rst value for each point. Its offset is 0 (default case)
			// UV Texture is the 2nd value for each point. Its offset is 1
			scene.addParamID(id, ChunkPrimParam::VBUFFER_NUMDATA,"texcoord0",1);

			// Link index buffer idIB to primitive id
			scene.addParamID(id, ChunkPrimParam::IBUFFER_ID, "", idIB);

			// Add a shader parameter
			scene.addParamEnum(id, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);

			// Link the texture to the primitive id
			scene.addParamID(id, ChunkPrimParam::TEXTURE, "texture", idT);

			Mat4x4f m;
			m.identity();
			scene.addParam(id, ChunkPrimParam::TRANSFORM,"", m );
			scene.addParam(id, ChunkPrimParam::TRANSFORM_OVERRIDE,"", true );

			float left    = -1;
			float right   =  1;
			float top     =  1;
			float bottom  = -1;
			float zNear   = -1;
			float zFar    =  1;

			float tx = - (right + left) / (right - left);
			float ty = - (top + bottom) / (top - bottom);
			float tz = - (zFar + zNear) / (zFar - zNear);

			m[0][0] =   2.0f / ( right - left   );
			m[1][1] =   2.0f / ( top   - bottom );
			m[2][2] = - 2.0f / ( zFar  - zNear  );
			m[0][3] =   tx;
			m[1][3] =   ty;
			m[2][3] =   tz;
			m[3][3] =    1;

			scene.addParam(id, ChunkPrimParam::PROJECTION, "", m );
			scene.addParam(id, ChunkPrimParam::ORDER, "", std::numeric_limits<int>::min() );

			scene.addParam(id, ChunkPrimParam::PARAMOPENGL, "DepthTest", false );
			scene.addParam(id, ChunkPrimParam::PARAMOPENGL, "DepthWrite", false );

			MessageWrite mw = scene.dump<Allocator>(&alloc);

			if( mw.data.valid() )
				m_service->getDisplay()->getWorldCgEngine()->process(mw);

			return true;
		}


		virtual eState handleNoMessage()
		{
			return E_REMOVE;
		}

		std::string   m_strTextureName;
		ServiceLayer *m_service;
	};
}


DEFIMP_PORTPLUG( bgpicturePlugHandler, ServiceLayer, bgpicture )

