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
// Simple cube

#include <flowvr/module.h>
#include <flowvr/render/chunkrenderwriter.h>

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

// In order not to type flowvr::render everywhere, include the whole namespace
using namespace flowvr::render;

int main(int argc, char** argv)
{
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
  ID idVB = module->generateID(); // Id of the Vertex Buffer
  ID idIB = module->generateID(); // Id of the Index Buffer

  // Create vertex+index buffers for a cube
  // We use  three vertex buffers (position, normal and color). This array provides the number of vertex buffers and their types
  int vertexBufferTypes[3] = {Type::Vec3f, Type::Vec3f, Type::Vec3f};
  // Create the vertex buffers (8 points and 2 buffers)
  ChunkVertexBuffer* vb = scene.addVertexBuffer(idVB, 8, 3, vertexBufferTypes);
  // Create the index buffer (12 triangles = 12*3 values of type Byte)
  int indexBufferType = Type::Byte;
  ChunkIndexBuffer* ib = scene.addIndexBuffer(idIB, 12*3, indexBufferType, ChunkIndexBuffer::Triangle);
 
  // Fill vertex and index buffers 
  Vec3f* vdata = (Vec3f*) vb->data();
  // Positions = vdata[3*i] values
  // Normal = vdata[3*i + 1] values
  // Colors = vdata[3*i + 2] values
  vdata[3*0] = Vec3f(-1.0, -1.0, -1.0); 
  vdata[3*1] = Vec3f(1.0, -1.0, -1.0); 
  vdata[3*2] = Vec3f(1.0, 1.0, -1.0); 
  vdata[3*3] = Vec3f(-1.0, 1.0, -1.0); 
  vdata[3*4] = Vec3f(-1.0, -1.0, 1.0); 
  vdata[3*5] = Vec3f(1.0, -1.0, 1.0); 
  vdata[3*6] = Vec3f(1.0, 1.0, 1.0); 
  vdata[3*7] = Vec3f(-1.0, 1.0, 1.0); 

  // Normals = Positions for this example
  for(unsigned int i = 0; i != 8; ++i)
  {
    vdata[3*i +1] = vdata[3*i];
    vdata[3*i +1].normalize();
  }

  // A different color for each vertices ...
  for(unsigned int i = 0; i != 8; ++i)
  {
    vdata[3*i+2] = Vec3f((float)(i%2), (float)((i>>1)%2), (float)((i>>2)%2));
  }


  //

  // Index buffer. We set values of the 12 triangles
  Vec<3,char>* idata = (Vec<3,char>*) ib->data();
  idata[0] = Vec<3,char>(0, 2, 1); 
  idata[1] = Vec<3,char>(0, 3, 2); 
  idata[2] = Vec<3,char>(0, 1, 4); 
  idata[3] = Vec<3,char>(1, 5, 4); 
  idata[4] = Vec<3,char>(3, 7, 0); 
  idata[5] = Vec<3,char>(0, 7, 4); 
  idata[6] = Vec<3,char>(4, 6, 5); 
  idata[7] = Vec<3,char>(4, 7, 6); 
  idata[8] = Vec<3,char>(1, 5, 2); 
  idata[9] = Vec<3,char>(2, 5, 6); 
  idata[10] = Vec<3,char>(2, 3, 6); 
  idata[11] = Vec<3,char>(6, 3, 7); 

  // Create new primitive
  scene.addPrimitive(idPrim, "Cube");

  // Link vertex buffer idVB to primitive idPrim
  // Position, normal and color are given by vertexbuffer idVB
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID, "position", idVB);
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID,"normal",idVB);
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID,"color0",idVB);
  // Position is 1rst value for each point. Its offset is 0 (default case)
  // Normal is the 2nd value for each point. Its offset is 1
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_NUMDATA,"normal",1);
  // Color is the 3rd value for each point. Its offset is 2
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_NUMDATA,"color0",2);

  // Link index buffer idIB to primitivie idPrim
  scene.addParamID(idPrim, ChunkPrimParam::IBUFFER_ID, "", idIB);

  // Set shaders parameters
  // Note: We do not redefine the shader for this primitive so these
  // parameters correspond to the default shaders.
  // See data/shaders/default_v.cg and data/shaders/default_p.cg
  // See also shaders examples in this tutorial 
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewIT", ChunkPrimParam::ModelView_InvTrans);
  Vec3f light(1,3,2); light.normalize();
  scene.addParam(idPrim, ChunkPrimParam::PARAMPSHADER, "lightdir", light);


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
