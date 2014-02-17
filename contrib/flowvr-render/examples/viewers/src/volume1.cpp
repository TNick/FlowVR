// Basic FlowVR Render example volume rendering module

#include <flowvr/module.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <flowvr/render/primitive.h>

#include <math.h>
#include <stdlib.h>

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

  // Helper class to construct primitives update chunks
  flowvr::render::ChunkRenderWriter scene;

  // Get IDs for all our primitives and resources
  ID idPrim = module->generateID();
  ID idTexture = module->generateID();
  ID idVB = module->generateID();
  ID idIB = module->generateID();
  ID idVS = module->generateID();
  ID idPS = module->generateID();

  // Load texture or use default one
  ChunkTexture* texture;
  if (argc<2 || !(texture=scene.loadTexture(idTexture,argv[1])))
    texture=scene.addDefaultTexture(idTexture);

  // Create vertex+index buffers
  {
    int dataType[1] = { Type::Vec3f };
    ChunkVertexBuffer* vb = scene.addVertexBuffer(idVB, 4, 1, dataType, BBox(Vec3f(0,0,0),Vec3f(1,1,1)));
    Vec3f* data = (Vec3f*) vb->data();
    float z = -1;
    data[0] = Vec3f(-2,-2,z);
    data[1] = Vec3f( 2,-2,z);
    data[2] = Vec3f( 2, 2,z);
    data[3] = Vec3f(-2, 2,z);
    scene.addIndexBuffer(idIB, -1, Type::Null, ChunkIndexBuffer::Quad);
  }

  // Load custom shaders
  scene.loadVertexShader(idVS, "shaders/volume_v.cg");
  scene.loadPixelShader(idPS, "shaders/volume_p.cg");

  // Create new primitive
  scene.addPrimitive(idPrim, "Volume");

  // Set shaders
  scene.addParamID(idPrim, ChunkPrimParam::VSHADER,"",idVS);
  scene.addParamID(idPrim, ChunkPrimParam::PSHADER,"",idPS);

  // Set vertex buffers
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID, "position", idVB);

  // Set index buffer
  scene.addParamID(idPrim, ChunkPrimParam::IBUFFER_ID, "", idIB);
  scene.addParam(idPrim, ChunkPrimParam::IBUFFER_NBI, "", 4);

  // Set parameters
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewProjI", ChunkPrimParam::ModelViewProjection_Inv);
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewI", ChunkPrimParam::ModelView_Inv);
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "Proj", ChunkPrimParam::Projection);
  scene.addParam(idPrim, ChunkPrimParam::PARAMPSHADER, "steps", 32);
  scene.addParam(idPrim, ChunkPrimParam::PARAMPSHADER, "opacity", 64.0f);

  scene.addParam(idPrim, ChunkPrimParam::PARAMOPENGL, "DepthWrite", false);
  scene.addParam(idPrim, ChunkPrimParam::PARAMOPENGL, "DepthTest", false);
  scene.addParam(idPrim, ChunkPrimParam::PARAMOPENGL, "Blend", true);
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMOPENGL, "BlendMode", flowvr::render::BLEND_PREMULT);
  scene.addParam(idPrim, ChunkPrimParam::ORDER, "", 10);

  // Send initial scene
  scene.put(&pOut);

  // Main FlowVR loop. Contains the animations of the scene
  int it=0;
  while (module->wait())
  {
    // Update scene
    scene.rotatePrimitive(idPrim, it);

    // Send message
    scene.put(&pOut);

    ++it;
  }

  module->close();

  return 0;
}
