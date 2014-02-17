/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
                          flowvr-petaflow
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
* File: mirror.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/render/chunkrenderwriter.h>

#include <set>
#include <algorithm>
#include <iterator>

using namespace flowvr::portutils;
using namespace flowvr;
using namespace flowvr::render;
using namespace ftl;

extern "C" void getParameters( flowvr::portutils::ARGS &args )
{
	// insert parameter here
	args["POSITION"] = Parameter("0,0,0", "Position of the mirror in world space", Parameter::P_LIST, Parameter::PS_NUMBER );
	args["CLEARCOLOR"] = Parameter("0,0,0,0", "Clear color to use for mirror background", Parameter::P_LIST, Parameter::PS_NUMBER );
}

namespace
{

	class mirrorHandler : public flowvr::portutils::PipeHandler
	{
	public:
		mirrorHandler( const flowvr::portutils::ARGS &args )
		: flowvr::portutils::PipeHandler()
		, m_mirrorID(0)
		, m_camId(0)
		, m_vPos(Vec3f(0,0,-2) )
		, m_vColor( Vec4f(0,0,0,0) )
		{
			try
			{
				// parse arguments here
				Parameter::ParVecN v = args("POSITION").getValue<Parameter::ParVecN>();
				if( v.size() == 3 )
				{
					m_vPos[0] = v[0];
					m_vPos[1] = v[1];
					m_vPos[2] = v[2];
				}
				else
					std::cerr << "not enough entries given for POSITION! (expecting 3)" << std::endl;

				v = args("CLEARCOLOR").getValue<Parameter::ParVecN>();
				if( v.size() == 4 )
				{
					m_vColor[0] = v[0];
					m_vColor[1] = v[1];
					m_vColor[2] = v[2];
					m_vColor[3] = v[3];
				}
				else
					std::cerr << "not enough entries given for POSITION! (expecting 3)" << std::endl;
			}
			catch( std::exception &e )
			{
			}
		}


		virtual bool once( ModuleAPI &module, MessageWrite &m_out, StampList *sl_out, Allocator &alloc )
		{
			// create mirror geometry and target groups
			// Get IDs for all our primitives and resources
			m_mirrorID = module.generateID(); // Id of the primitive
			ID idT    = module.generateID();  // Id of the Texture
			ID idVB   = module.generateID(); // Id of the Vertex Buffer
			ID idIB   = module.generateID(); // Id of the Index Buffer
			ID idVS   = module.generateID(); // Id of the Vertex Shader
			ID idPS   = module.generateID(); // Id of the Pixel Shader

			// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// ADD AN FBO TO THE SCENE
			// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			scene.addTexture(idT,
					         ChunkTexture::RGBA,
					         ftl::Type::vector(ftl::Type::Byte, 4),
					         640, 480, 0,
					         TargetGroups::FBO2txTp(0) );

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
			vertex[0].pos = Vec3f(-1,-1,0); vertex[0].uv = Vec2f(1,0);
			vertex[1].pos = Vec3f( 1,-1,0); vertex[1].uv = Vec2f(0,0);
			vertex[2].pos = Vec3f( 1, 1,0); vertex[2].uv = Vec2f(0,1);
			vertex[3].pos = Vec3f(-1, 1,0); vertex[3].uv = Vec2f(1,1);

			// Index buffer. We set values of the 12 triangles
			Vec<4,char>* idata = (Vec<4,char>*) ib->data();
			idata[0] = Vec<4,char>(0, 1, 2, 3);

			// Set shaders parameters
			// See data/shaders/texture_v.cg and data/shaders/texture_p.cg
			scene.loadVertexShader(idVS, "shaders/texture_v.cg");
			scene.loadPixelShader(idPS,  "shaders/texture_p.cg");

			// Create new primitive
			scene.addPrimitive(m_mirrorID, "Mirror Geometry");

			// Link the shaders to the primitive m_mirrorID
			scene.addParamID(m_mirrorID, ChunkPrimParam::VSHADER,"",idVS);
			scene.addParamID(m_mirrorID, ChunkPrimParam::PSHADER,"",idPS);
			// Link vdata buffer idVB to primitive m_mirrorID
			// Position, UV Texture position are given by vertex data buffer idVB
			scene.addParamID(m_mirrorID, ChunkPrimParam::VBUFFER_ID,"position", idVB);
			scene.addParamID(m_mirrorID, ChunkPrimParam::VBUFFER_ID,"texcoord0",idVB);
			// Position is 1rst value for each point. Its offset is 0 (default case)
			// UV Texture is the 2nd value for each point. Its offset is 1
			scene.addParamID(m_mirrorID, ChunkPrimParam::VBUFFER_NUMDATA,"texcoord0",1);

			// Link index buffer idIB to primitive m_mirrorID
			scene.addParamID(m_mirrorID, ChunkPrimParam::IBUFFER_ID, "", idIB);

			// Add a shader parameter
			scene.addParamEnum(m_mirrorID, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);

			// Link the texture to the primitive m_mirrorID
			scene.addParamID(m_mirrorID, ChunkPrimParam::TEXTURE, "texture", idT);

			scene.addParam(m_mirrorID, ChunkPrimParam::TRANSFORM,"", matrixTransform(m_vPos) * matrixScale( Vec3f(1.33,1,1) * 0.25f ) );

			// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// ADD A CAMERA TO THE SCENE
			// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			m_camId = module.generateID();
			scene.addPrimitive(m_camId, "mirror cam", 0, TP_CAMERA,  (unsigned int)ChunkPrimParam::TG(TargetGroups::FBO2Tg(0)) );

			float zmin, zmax, fov, aspect;
			zmin   = 0.001f;
			zmax   = 100.0f;
			fov    = 60.0f;
			aspect = 4.0f/3.0f;

			scene.addParam(m_camId, ChunkPrimParam::PARAMOPENGL, "zmax", &zmax, Type::Float );
			scene.addParam(m_camId, ChunkPrimParam::PARAMOPENGL, "zmin", &zmin, Type::Float );
			scene.addParam(m_camId, ChunkPrimParam::PARAMOPENGL, "fovy", &fov, Type::Float );
			scene.addParam(m_camId, ChunkPrimParam::PARAMOPENGL, "aspect", &aspect, Type::Float );
			scene.addParam(m_camId, ChunkPrimParam::PARAMOPENGL, "clear", &m_vColor, Type::Vec4f );

			scene.addParam(m_camId, ChunkPrimParam::TRANSFORM, "", matrixTransform<float>(m_vPos-Vec3f(0,0,0.125) , 180, Vec3f(0,1,0) ) );


			// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// OUTPUT THE STUFF
			// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			m_out = scene.dump<Allocator>( &alloc );

			m_activeSet.insert( m_camId );
			m_activeSet.insert( m_mirrorID );
			return true;
		}


		static void printId( ID id )
		{
			std::cout << "[" << id << "] " << std::endl;
		}


		class _addToMirror
		{
		public:
			_addToMirror( ChunkRenderWriter &_scene, std::set<ID> &activeSet )
			: scene( _scene )
			, m_activeSet(activeSet)
			{

			}

			ChunkRenderWriter &scene;
			std::set<ID> &m_activeSet;

			void operator()( ID id )
			{
				scene.addParam( id, ChunkPrimParam::TARGETGROUP, "", ChunkPrimParam::TG(0) + ChunkPrimParam::TG(TargetGroups::FBO2Tg(0)) );
				m_activeSet.insert(id);
				std::cout << "adding [" << id << "] to mirror. [tg = "
						  << (ChunkPrimParam::TG(0) + ChunkPrimParam::TG(TargetGroups::FBO2Tg(0)))
						  << "]"
						  << std::endl;
			}
		};

		virtual eState handleMessagePipe( const flowvr::Message &m_in,
				                          const flowvr::StampList *sl_in,
				                          flowvr::MessageWrite &m_out,
				                          flowvr::StampList *sl_out,
				                          flowvr::Allocator &allocator )
		{
			// in we get a list of ids for the current scene, we mask out our id
			// and the one of the camera and ID_CAMERA, but add all others to the TG(1) group

			int num = long(m_in.data.getSize()) / sizeof(ID);

			const ID *ids = m_in.data.getRead<ID>();

			std::set<ID> currentSet, differenceSet;

			for( size_t n = 0; n < num; ++n )
				currentSet.insert( ids[n] );

			// we assume an incremental scene
			std::set_difference( currentSet.begin(), currentSet.end(),
					m_activeSet.begin(), m_activeSet.end(),
					         std::inserter(differenceSet, differenceSet.end() ) );

			std::for_each( differenceSet.begin(), differenceSet.end(), _addToMirror(scene, m_activeSet ) );

			if( scene.isDirty() )
				m_out = scene.dump<Allocator>(&allocator);

			return E_OK;
		}

		ChunkRenderWriter scene;
		ID m_mirrorID, m_camId;

		std::set<ID> m_activeSet;
		Vec3f m_vPos;
		Vec4f m_vColor;
	};
}


DEFIMP_PORTPLUG_NOSERVICE( mirrorHandler, mirror )

