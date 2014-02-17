// Basic FlowVR Render example viewer module
// Simple sphere

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
  ID idVB = module->generateID();
  ID idIB = module->generateID();

  // Create vertex+index buffers for a sphere
  scene.addMeshSphere(idVB, idIB);

  // Create new primitive
  scene.addPrimitive(idPrim, "Sphere");

  // Set vertex buffers
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID, "position", idVB);
  scene.addParamID(idPrim, ChunkPrimParam::VBUFFER_ID, "normal", idVB);
  scene.addParam(idPrim, ChunkPrimParam::VBUFFER_NUMDATA, "normal", int(1));

  // Set index buffer
  scene.addParamID(idPrim, ChunkPrimParam::IBUFFER_ID, "", idIB);

  // Set shaders parameters
  // Note: We do not redefine the shader for this primitive so these
  // parameters correspond to the default shaders.
  // See data/shaders/default_v.cg and data/shaders/default_p.cg
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);
  scene.addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewIT", ChunkPrimParam::ModelView_InvTrans);
  scene.addParam(idPrim, ChunkPrimParam::PARAMVSHADER, "color0", Vec3f(1,0.5,1));
  Vec3f light(1,3,2); light.normalize();
  scene.addParam(idPrim, ChunkPrimParam::PARAMPSHADER, "lightdir", light);

  // Send initial scene
  scene.put(&pOut);

  // Main FlowVR loop. Contains the animations of the scene
  int it=0;
  while (module->wait())
  {
    // Update scene
    // Nothing in this simple example...

    // Send message
    scene.put(&pOut);

    ++it;
  }

  module->close();

  return 0;
}
