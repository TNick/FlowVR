/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
                          flowvr-balzac-migrate
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
* File: multitextureViewerPlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>
#include <flowvr/render/chunkrenderwriter.h>

using namespace flowvr::render;

namespace
{

	class multitextureViewerPlugHandler : public flowvr::portutils::SourcePortHandler
	{
	public:
		multitextureViewerPlugHandler( const flowvr::portutils::ARGS &args )
		: flowvr::portutils::SourcePortHandler()
		{
		}

		virtual bool once(flowvr::ModuleAPI &module, flowvr::MessageWrite &mw, flowvr::StampList *sl, flowvr::Allocator &alloc )
		{
			flowvr::render::ChunkRenderWriter scene;
			// create mirror geometry and target groups
			// Get IDs for all our primitives and resources
			ID iQuad   = module.generateID(); // Id of the primitive
			ID idT0    = module.generateID(); // Id of the Texture0
			ID idT1    = module.generateID(); // Id of the Texture1
			ID idVB    = module.generateID(); // Id of the Vertex Buffer
			ID idIB    = module.generateID(); // Id of the Index Buffer
			ID idVS    = module.generateID(); // Id of the Vertex Shader
			ID idPS    = module.generateID(); // Id of the Pixel Shader


			//////////////////////////////////////////////////////////////////////
			// CREATE THE QUAD
			//////////////////////////////////////////////////////////////////////
			// Create new primitive
			scene.addPrimitive(iQuad, "Multi-textured quad");


			  //Texture Creation
			// Load a texture from a file.
			ChunkTexture* texture;
			texture = scene.loadTexture(idT0, "images/wood.jpg");
			if(texture)
			{
				texture->level = -1; // no level of detail used
				texture->gen = -1;   // this is a static resource

				// we add this texture as the texture that will later appear in texture unit 0
				// it is the order of declaration that counts
				// the variable name will be 'solidTexture0'
				// see 'shaders/multitexture_p.cg' for more information
				scene.addParamID(iQuad, ChunkPrimParam::TEXTURE, "solidTexture0", idT0);
			}
			else
				return false;

			texture = scene.loadTexture(idT1, "images/light.png");
			if(texture)
			{
				texture->level = -1; // no level of detail used
				texture->gen = -1;   // this is a static resource

				// we add this texture as the texture that will later appear in texture unit 1
				// it is the order of declaration that counts
				// the variable name will be 'lightTexture1'
				// see 'shaders/multitexture_p.cg' for more information
				scene.addParamID(iQuad, ChunkPrimParam::TEXTURE, "lightTexture1", idT1);
			}
			else
				return false;

			// now produce a simple quad that will show the texture later.

			// We use  two vdata buffers (position and uv texture position), we do not want any color here,
			// so we do not pass one. (we take all color from solidTexture0)
			// see 'shaders/multitexture_v.cg' for more information
			int vdataBufferTypes[2] = {Type::Vec3f, Type::Vec2f};
			// Create the vdata buffers (4 points and 2 buffers)
			ChunkVertexBuffer* vb = scene.addVertexBuffer(idVB, 4, 2, vdataBufferTypes);
			// Create the index buffer (1 Quad = 4 values of type Byte)
			int indexBufferType  = Type::Byte; // we just have 4 points in the vertex buffer, so a byte-index is wide enough
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

			// index the 4 vertices (not really needed for this scene, but for the sake of example, we do it here).
			Vec<4,char>* idata = (Vec<4,char>*) ib->data();
			idata[0] = Vec<4,char>(0, 1, 2, 3);

			// Set shaders parameters
			scene.loadVertexShader(idVS, "shaders/multitexture_v.cg");
			scene.loadPixelShader(idPS,  "shaders/multitexture_p.cg");

			// Link the shaders to the primitive iQuad
			scene.addParamID(iQuad, ChunkPrimParam::VSHADER,"",idVS);
			scene.addParamID(iQuad, ChunkPrimParam::PSHADER,"",idPS);
			// Link vdata buffer idVB to primitive iQuad
			// Position, UV Texture position are given by vertex data buffer idVB
			scene.addParamID(iQuad, ChunkPrimParam::VBUFFER_ID,"position", idVB);
			scene.addParamID(iQuad, ChunkPrimParam::VBUFFER_ID,"texcoord0",idVB);
			// Position is 1rst value for each point. Its offset is 0 (default case)
			// UV Texture is the 2nd value for each point. Its offset is 1
			scene.addParamID(iQuad, ChunkPrimParam::VBUFFER_NUMDATA,"texcoord0",1);

			// Link index buffer idIB to primitive iQuad
			scene.addParamID(iQuad, ChunkPrimParam::IBUFFER_ID, "", idIB);

			// upload the modelview projection to the shader to transform all points in the vertex buffer accordingly
			scene.addParamEnum(iQuad, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);

			// this is important:
			// add as many TEXTURE parameters as you want to use textures, mind the order!
			// solidTexture0 will be used in unit 0 (position that counts, not name!)
			scene.addParamID(iQuad, ChunkPrimParam::TEXTURE, "solidTexture0", idT0);
			// lightTexture1 will be used in unit 1 (position that counts, not name!)
			scene.addParamID(iQuad, ChunkPrimParam::TEXTURE, "lightTexture1", idT1);

			// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// OUTPUT THE STUFF
			// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			mw = scene.dump<Allocator>( &alloc );

			return true;

		}

		virtual eState handleMessage( flowvr::MessageWrite &out, 
                                      flowvr::StampList *stampsOut,
                                      flowvr::Allocator &allocate )
		{
			return E_REMOVE;
		}
	};
}


DEFIMP_PORTPLUG_NOSERVICE_NOPRM( multitextureViewerPlugHandler, multitextureViewerPlug )

