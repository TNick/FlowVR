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
* File: ./src/viewers/grid.cpp                                    *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include <flowvr/module.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <flowvr/utils/cmdline.h>

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

// In order not to type flowvr::render everywhere, include the whole namespace
using namespace flowvr::render;

typedef int Line[2];

int main(int argc, char** argv)
{

  int nx = 11;
  int ny = 11;
  float step = 1.0f;
  float pointSize = 0.2f;
  float lineSize = 0.1f;

  flowvr::utils::CmdLine cmd("Planar grid viewer");
  cmd.opt("nx",'x',"number of lines in the X direction",&nx);
  cmd.opt("ny",'y',"number of lines in the Y direction",&ny);
  cmd.opt("step",'s',"distance between each line",&step);
  cmd.opt("psize",'p',"point size",&pointSize);
  cmd.opt("lsize",'l',"line size",&lineSize);
  bool error=false;
  if (!cmd.parse(argc,argv,&error))
    return error?1:0;

  std::cout << "nx="<<nx<<" ny="<<ny<<" step="<<step<<std::endl;

  // Declare a FlowVR Render output port to send our scene description.
  SceneOutputPort pOut("scene");

  // Data input ports

  // Initialize FlowVR
  std::vector<flowvr::Port*> ports;
  ports.push_back(&pOut);
  flowvr::ModuleAPI* module = flowvr::initModule(ports);
  if (module == NULL)
    return 2;

  // Helper class to construct primitives update chunks
  flowvr::render::ChunkRenderWriter scene;

  // Get IDs for all our primitives and resources
  ID idPoint = module->generateID();
  ID idLine = module->generateID();
  ID idVBPoint = module->generateID();
  ID idIBPoint = module->generateID();
  ID idVBLine = module->generateID();
  ID idIBLine = module->generateID();

  // Create vertex+index buffers for a sphere
  scene.addMeshSphere(idVBPoint, idIBPoint,8,4);

  // Create vertex+index buffers for a cylinder
  scene.addMeshCylinder(idVBLine, idIBLine,8);

  // Create new primitives
  scene.addPrimitive(idPoint, "Point");
  scene.addPrimitive(idLine, "Line");

  // Set vertex buffers
  scene.addParamID(idPoint, ChunkPrimParam::VBUFFER_ID, "position", idVBPoint);
  scene.addParamID(idPoint, ChunkPrimParam::VBUFFER_ID, "normal", idVBPoint);
  scene.addParam(idPoint, ChunkPrimParam::VBUFFER_NUMDATA, "normal", int(1));

  scene.addParamID(idLine, ChunkPrimParam::VBUFFER_ID, "position", idVBLine);
  scene.addParamID(idLine, ChunkPrimParam::VBUFFER_ID, "normal", idVBLine);
  scene.addParam(idLine, ChunkPrimParam::VBUFFER_NUMDATA, "normal", int(1));

  // Set index buffer
  scene.addParamID(idPoint, ChunkPrimParam::IBUFFER_ID, "", idIBPoint);

  scene.addParamID(idLine, ChunkPrimParam::IBUFFER_ID, "", idIBLine);

  // Set shaders parameters
  // Note: We do not redefine the shader for this primitive so these
  // parameters correspond to the default shaders.
  // See data/shaders/default_v.cg and data/shaders/default_p.cg
  Vec3f light(1,3,2); light.normalize();

  scene.addParamEnum(idPoint, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);
  scene.addParamEnum(idPoint, ChunkPrimParam::PARAMVSHADER, "ModelView", ChunkPrimParam::ModelView);
  scene.addParamEnum(idPoint, ChunkPrimParam::PARAMVSHADER, "ModelViewIT", ChunkPrimParam::ModelView_InvTrans);
  scene.addParam(idPoint, ChunkPrimParam::PARAMVSHADER, "color", Vec4f(1,1,1,1));
  scene.addParam(idPoint, ChunkPrimParam::PARAMPSHADER, "lightdir", light);
  scene.addParam(idPoint, ChunkPrimParam::TRANSFORM_SCALE, "", pointSize);

  scene.addParamEnum(idLine, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);
  scene.addParamEnum(idLine, ChunkPrimParam::PARAMVSHADER, "ModelView", ChunkPrimParam::ModelView);
  scene.addParamEnum(idLine, ChunkPrimParam::PARAMVSHADER, "ModelViewIT", ChunkPrimParam::ModelView_InvTrans);
  scene.addParam(idLine, ChunkPrimParam::PARAMVSHADER, "color", Vec4f(1,1,0,1));
  scene.addParam(idLine, ChunkPrimParam::PARAMPSHADER, "lightdir", light);

  scene.addParam(idPoint, ChunkPrimParam::VISIBLE,"",false);
  scene.addParam(idLine, ChunkPrimParam::VISIBLE,"",false);

  std::vector<ID> idPoints;
  std::vector<ID> idLines;

  Vec3f p0(-step*(nx-1)*0.5f,-step*(ny-1)*0.5f,0);
  for (int x=0;x<nx;x++)
    for (int y=0;y<ny;y++)
    {
      Vec3f p = p0+Vec3f(x*step,y*step,0);
      ID id = module->generateID();
      idPoints.push_back(id);
      scene.addPrimitive(id, "", idPoint);
      scene.addParam(id, ChunkPrimParam::VISIBLE,"",true);
      scene.addParam(id, ChunkPrimParam::TRANSFORM_POSITION, "", p);
    }
  for (int x=0;x<nx;x++)
    for (int y=0;y<ny-1;y++)
    {
      Vec3f p1 = p0+Vec3f(x*step,y*step,0);
      Vec3f p2 = p0+Vec3f(x*step,(y+1)*step,0);
  
      ID id = module->generateID();
      idLines.push_back(id);
      scene.addPrimitive(id, "", idLine);
      scene.addParam(id, ChunkPrimParam::VISIBLE,"",true);

      Vec3f pz = (p2-p1)*0.5;
      Vec3f px = cross(pz, Vec3f(0,0,1));
      if (px.norm2()<0.0001)
	px = cross(pz, Vec3f(0,1,0));
      Vec3f py = cross(px, pz);
      px *= lineSize/px.norm();
      py *= lineSize/py.norm();

      Mat3x3f m1;
      m1[0] = px;
      m1[1] = py;
      m1[2] = pz;
      Mat3x3f tm; tm.transpose(m1);
      Mat4x4f transform; transform.identity();
      Vec3f p = ((p1+p2)*0.5);
      transform = tm;
      transform[0][3] = p[0];
      transform[1][3] = p[1];
      transform[2][3] = p[2];

      scene.addParam(id, ChunkPrimParam::TRANSFORM, "", transform);

    }
  for (int x=0;x<nx-1;x++)
    for (int y=0;y<ny;y++)
    {
      Vec3f p1 = p0+Vec3f(x*step,y*step,0);
      Vec3f p2 = p0+Vec3f((x+1)*step,y*step,0);
  
      ID id = module->generateID();
      idLines.push_back(id);
      scene.addPrimitive(id, "", idLine);
      scene.addParam(id, ChunkPrimParam::VISIBLE,"",true);

      Vec3f pz = (p2-p1)*0.5;
      Vec3f px = cross(pz, Vec3f(0,0,1));
      if (px.norm2()<0.0001)
	px = cross(pz, Vec3f(0,1,0));
      Vec3f py = cross(px, pz);
      px *= lineSize/px.norm();
      py *= lineSize/py.norm();

      Mat3x3f m1;
      m1[0] = px;
      m1[1] = py;
      m1[2] = pz;
      Mat3x3f tm; tm.transpose(m1);
      Mat4x4f transform; transform.identity();
      Vec3f p = ((p1+p2)*0.5);
      transform = tm;
      transform[0][3] = p[0];
      transform[1][3] = p[1];
      transform[2][3] = p[2];

      scene.addParam(id, ChunkPrimParam::TRANSFORM, "", transform);

    }

  scene.put(&pOut);
  
  module->wait();

  module->close();

  return 0;
}
