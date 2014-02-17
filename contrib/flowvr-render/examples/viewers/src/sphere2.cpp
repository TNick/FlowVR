// Basic FlowVR Render example viewer module
// Rotating sphere with custom texture and shaders

#include <flowvr/module.h>
#include <flowvr/render/chunkrenderwriter.h>

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
  ChunkTexture* texture = 0;
  if (argc<2 || !(texture=scene.loadTexture(idTexture,argv[1])))
    texture=scene.addDefaultTexture(idTexture);
  if (!texture)
    return 1;

  // Create vertex+index buffers for a sphere
  scene.addMeshSphere(idVB, idIB);

  // Load custom shaders
  scene.loadVertexShader(idVS, "shaders/sphere_v.cg");
  scene.loadPixelShader(idPS, "shaders/sphere_p.cg");

  // Create new primitive
  scene.addPrimitive(idPrim, "Sphere");

  // Set shaders
  scene.addParamID(idPrim, ChunkPrimParam::VSHADER,"",idVS);
  scene.addParamID(idPrim, ChunkPrimParam::PSHADER,"",idPS);

  // Set vertex buffers
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID, "position", idVB);
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID, "normal", idVB);
  scene.addParam(idPrim, ChunkPrimParam::VBUFFER_NUMDATA, "normal", int(1));
  scene.addParamID(idPrim, ChunkPrimParam::TEXTURE, "texture", idTexture);
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID, "texcoord0", idVB);
  scene.addParam(idPrim, ChunkPrimParam::VBUFFER_NUMDATA, "texcoord0", int(2));

  // Set index buffer
  scene.addParamID(idPrim, ChunkPrimParam::IBUFFER_ID, "", idIB);

  // Set shaders parameters
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelView", ChunkPrimParam::ModelView);
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewIT", ChunkPrimParam::ModelView_InvTrans);
  scene.addParam(idPrim, ChunkPrimParam::PARAMVSHADER, "color0", Vec3f(1,0.5,1));
  Vec3f light(1,3,2); light.normalize();
  scene.addParam(idPrim, ChunkPrimParam::PARAMPSHADER, "lightdir", light);
  if (texture->nx < 1.5*texture->ny)
    scene.addParam(idPrim, ChunkPrimParam::PARAMVSHADER, "mapscale", Vec2f(2,1));

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
