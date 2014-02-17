// Basic FlowVR Render example viewer module
// Simple text viewer

#include <flowvr/module.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <flowvr/render/font.h>

#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// In order not to type flowvr::render everywhere, include the whole namespace
using namespace flowvr::render;

int main(int argc, char** argv)
{

  std::string text = "Hello,\nWorld!";
  const char* fontname = "fonts/default.txf";

  if (argc>=2) text = argv[1];
  if (argc>=3) fontname = argv[2];

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

  // Font

  Font font(&scene, module);

  font.load(fontname);
  //scene.saveTexture(font.load(fontname),"font.png");

  // Get IDs for all our primitives and resources
  ID idPrim = module->generateID();
  ID idVB = module->generateID();

  // Create the text primitive
  // The 2 last values are horizontal and vertical alignement (0.5 means centered).
  font.addText(idPrim, idVB, text, 0.5, 0.5, Font::Aligned);

  // Change color
  scene.addParam(idPrim, ChunkPrimParam::PARAMVSHADER, "color", Vec4f(1,1,0.5,1));

  // Add a sphere to give 3D context
  {
    ID idPrim = module->generateID();
    ID idTexture = module->generateID();
    ID idVB = module->generateID();
    ID idIB = module->generateID();
    ID idVS = module->generateID();
    ID idPS = module->generateID();
    scene.loadTexture(idTexture, "images/earth.jpg");
    scene.addMeshSphere(idVB, idIB);
    scene.loadVertexShader(idVS, "shaders/sphere_v.cg");
    scene.loadPixelShader(idPS, "shaders/sphere_p.cg");
    scene.addMeshSphere(idVB, idIB);
    scene.addDefaultPrimitive(idPrim, "Sphere", idTexture, idVB, idIB, idVS, idPS);
  }

  // Send initial scene
  scene.put(&pOut);

  // Main FlowVR loop. Contains the animations of the scene
  int it=0;
  while (module->wait())
  {
    // Read one char from keyboard (blocking version)
    // text += c;
    // Read one char from keyboard (non-blocking version)
    fd_set fds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    FD_ZERO(&fds);
    FD_SET(0,&fds);
    if (select(1, &fds, NULL, NULL, &tv)>0)
    {
      char c=' ';
      if (read(0, &c, 1) == 1) {
	text += c;
	font.setText(idVB, text, 0.5, 0.5);
      }
    }

    // Rotate the text around the sphere
    scene.addParam(idPrim, ChunkPrimParam::TRANSFORM, "", matrixRotation(it*0.5f,Vec3f(0,0,1))*matrixTranslation(Vec3f(0,-2,0))*matrixRotation(-90.0f,Vec3f(1,0,0)));

    // Send message
    scene.put(&pOut);

    ++it;
  }

  module->close();

  return 0;
}
