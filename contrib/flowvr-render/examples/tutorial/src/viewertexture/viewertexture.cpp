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
*******************************************************************/

// Basic FlowVR Render example viewer module
// Simple Quad with texture

#include <flowvr/module.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <flowvr/render/primitive.h>

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

// In order not to type flowvr::render everywhere, include the whole namespace
using namespace flowvr::render;

int main(int argc, char** argv)
{

  //Get the texture name from the command line
  const char* TextureFileName = "images/earth.jpg";
  if (argc>=2) TextureFileName = argv[1];	
	
  // Declare a FlowVR Render output port to send our scene description.
  SceneOutputPort pOut("scene");

  // Initialize FlowVR
  std::vector<flowvr::Port*> ports;
  ports.push_back(&pOut);
  flowvr::ModuleAPI* module = flowvr::initModule(ports);
  if (module == NULL)
    return 1;

  // Helper class to construct primitives 
  flowvr::render::ChunkRenderWriter scene;

  // Get IDs for all our primitives and resources
  ID idPrim = module->generateID(); // Id of the primitive
  ID idT = module->generateID(); // Id of the Texture Buffer
  ID idVB = module->generateID(); // Id of the Vertex Buffer
  ID idIB = module->generateID(); // Id of the Index Buffer
  ID idVS = module->generateID(); // Id of the Vertex Shader
  ID idPS = module->generateID(); // Id of the Pixel Shader

  //Texture Creation
  // Load a texture from a file.
  ChunkTexture* texture;
  if(!(texture=scene.loadTexture(idT,TextureFileName)))
	texture = scene.addDefaultTexture(idT);
  else
  	texture->level = 0; 

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
  
  // Index buffer. We set values of the 12 triangles
  Vec<4,char>* idata = (Vec<4,char>*) ib->data();
  idata[0] = Vec<4,char>(0, 1, 2, 3);

  // Set shaders parameters
  // See data/shaders/texture_v.cg and data/shaders/texture_p.cg
  scene.loadVertexShader(idVS, "shaders/texture_v.cg");
  scene.loadPixelShader(idPS, "shaders/texture_p.cg");

  // Create new primitive
  scene.addPrimitive(idPrim, "Quad");

  // Link the shaders to the primitive idPrim
  scene.addParamID(idPrim, ChunkPrimParam::VSHADER,"",idVS);
  scene.addParamID(idPrim, ChunkPrimParam::PSHADER,"",idPS);
  // Link vdata buffer idVB to primitive idPrim
  // Position, UV Texture position are given by vertex data buffer idVB
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID,"position", idVB);
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID,"texcoord0",idVB);
  // Position is 1rst value for each point. Its offset is 0 (default case)
  // UV Texture is the 2nd value for each point. Its offset is 1
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_NUMDATA,"texcoord0",1);

  // Link index buffer idIB to primitivie idPrim
  scene.addParamID(idPrim, ChunkPrimParam::IBUFFER_ID, "", idIB);

  // Add a shader parameter
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);

  // Link the texture to the primitive idPrim
  scene.addParamID(idPrim, ChunkPrimParam::TEXTURE, "texture", idT);

  // Main FlowVR loop. Contains the animations of the scene
  while (module->wait())
  {
    // Update scene
    // Nothing in this simple example...

    // Send message
    scene.put(&pOut);
    sleep(1);
  }

  module->close();

  return 0;
}
