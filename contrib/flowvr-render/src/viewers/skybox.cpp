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
*******************************************************************
*                                                                 *
* File: ./src/viewers/skybox.cpp                                  *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include <flowvr/module.h>
#include <flowvr/render/chunk.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <ftl/vec.h>
#include <ftl/quat.h>

#include <math.h>
#include <stdlib.h>

#include <flowvr/utils/cmdline.h>

using namespace flowvr::render;
using namespace flowvr::utils;
using namespace flowvr;


Option<ID> id("id", 'I' , "Force the first ID of the created primitives", ID(0), false);
Option<std::string> fn("filename", 'f' , "texturefilename to load", ID(0), false);


int main(int argc, char** argv)
{

    CmdLine cmd(std::string("SkyboxViewer:" ) );
    bool error=false;
    if (!cmd.parse(argc,argv,&error))
    {
    	if(error)
    		std::cerr << "pasing error on cmdline!" << std::endl;

    	std::cout << cmd.help();

        return error?1:0;
    }



  SceneOutputPort pOut("scene");
  std::vector<flowvr::Port*> ports;
  ports.push_back(&pOut);

  flowvr::ModuleAPI* module = flowvr::initModule(ports); 
  if (module == NULL)
  {
    return 1;
  }

  flowvr::render::ChunkRenderWriter scene;

  ID idP = id.count == 0 ? module->generateID() : id.value();

  ID idT = module->generateID();
  ID idVB = module->generateID();
  ID idIB = module->generateID();
  ID idVS = module->generateID();
  ID idPS = module->generateID();

  std::string filename = "images/cube0001.jpg";
  if (fn.count)
	  filename = fn.value();

  // Load texture.
  ChunkTexture* texture = scene.loadTextureCubemap(idT,filename);
  if (texture == NULL)
    scene.addDefaultTexture(idT);
  else texture->level = 0; // no mipmap

  int dataType[1] = { Type::Vec3f };
  ChunkVertexBuffer* vb = scene.addVertexBuffer(idVB, 4, 1, dataType, BBox());
  Vec3f* vertex = (Vec3f*)vb->data();
  vertex[0] = Vec3f(-2,-2,-1);
  vertex[1] = Vec3f( 2,-2,-1);
  vertex[2] = Vec3f( 2, 2,-1);
  vertex[3] = Vec3f(-2, 2,-1);

  ChunkIndexBuffer* ib = scene.addIndexBuffer(idIB, 4, Type::Byte, ChunkIndexBuffer::Quad);
  unsigned char* ind = (unsigned char*)ib->data();
  ind[0] = 0;
  ind[1] = 1;
  ind[2] = 2;
  ind[3] = 3;

  scene.loadVertexShader(idVS, "shaders/skybox_v.cg");
  scene.loadPixelShader(idPS, "shaders/skybox_p.cg");

  scene.addPrimitive(idP,"SkyBox");
  scene.addParam(idP, ChunkPrimParam::ORDER,"",-10);
  scene.addParamID(idP, ChunkPrimParam::VSHADER,"",idVS);
  scene.addParamID(idP, ChunkPrimParam::PSHADER,"",idPS);
  scene.addParamID(idP, ChunkPrimParam::VBUFFER_ID,"position",idVB);
  scene.addParamID(idP, ChunkPrimParam::IBUFFER_ID,"",idIB);
  scene.addParamEnum(idP, ChunkPrimParam::PARAMVSHADER, "Proj", ChunkPrimParam::Projection);
  scene.addParamEnum(idP, ChunkPrimParam::PARAMVSHADER, "ModelViewI", ChunkPrimParam::ModelView_Inv);
  scene.addParamID(idP, ChunkPrimParam::TEXTURE, "texture", idT);
  scene.addParam(idP,ChunkPrimParam::PARAMOPENGL,"DepthTest",false);
  scene.addParam(idP,ChunkPrimParam::PARAMOPENGL,"DepthWrite",false);
  Quat q;
  q.fromAngAxis(DEG2RAD(-90.0f),Vec3f(1,0,0));
  Quat q2;
  q2.fromAngAxis(DEG2RAD(-90.0f),Vec3f(0,0,1));
  scene.addParam(idP,ChunkPrimParam::TRANSFORM_ROTATION,"",q*q2);
  scene.addParam(idP, ChunkPrimParam::ORDER,"",-10);

  scene.put(&pOut);

  module->wait();

  module->close();

  return 0;
}
