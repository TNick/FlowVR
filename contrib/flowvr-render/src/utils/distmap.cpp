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
* File: ./src/utils/distmap.cpp                                   *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <flowvr/utils/cmdline.h>
#include <flowvr/render/mesh.h>

using namespace flowvr::render;

int main(int argc, char** argv)
{
  int res = 16;
  int rx=0,ry=0,rz=0;
  float border = 0.25;
  bool normalize = false;
  flowvr::utils::CmdLine cmd("Usage: flowvr-distmap [options] mesh.input mesh.output");
  cmd.opt("res",'r',"resolution of distmap",&res);
  cmd.opt("rx",'x',"X resolution of distmap",&rx);
  cmd.opt("ry",'y',"Y resolution of distmap",&ry);
  cmd.opt("rz",'z',"Z resolution of distmap",&rz);
  cmd.opt("border",'b',"border size relative to the object's BBox size (or negative for exact size)",&border);
  cmd.opt("normalize",'n',"transform points so that the center is at <0,0,0> and the max coodinate is 1",&normalize);
  bool error=false;
  if (!cmd.parse(argc,argv,&error))
    return error?1:0;

  if (!rx) rx = res;
  if (!ry) ry = res;
  if (!rz) rz = res;

  if (cmd.args.size()!=2)
  {
    std::cerr << cmd.help() << std::endl;
    return 1;
  }

  std::string file_in = cmd.args[0];
  std::string file_out = cmd.args[1];

  Mesh obj;

  if (!obj.load(file_in.c_str()))
  {
    std::cerr << "Failed to read "<<file_in<<std::endl;
    return 1;
  }

  if (normalize)
  {
    BBox bb = obj.calcBBox();
    std::cout << "Mesh bbox="<<bb<<std::endl;
    float size = 0;
    for (int i=0;i<3;i++)
    {
      float d = bb.b[i]-bb.a[i];
      if (d>size) size=d;
    }
    Vec3f center = (bb.a+bb.b)*0.5;
    float sc = 2/size;
    Mat4x4f m; m.identity();
    m(0,0)=sc; m(0,3) = -center[0]*sc;
    m(1,1)=sc; m(1,3) = -center[1]*sc;
    m(2,2)=sc; m(2,3) = -center[2]*sc;
    for (int i=0;i<obj.nbp();i++)
      obj.PP(i) = transform(m,obj.getPP(i));
  }

  std::cout << "Mesh bbox="<<obj.calcBBox()<<std::endl;

  std::cout << "Flipping mesh..."<<std::endl;
  obj.calcFlip();
  obj.calcEdges();
  bool closed = obj.isClosed();
  std::cout << "Mesh is "<<(closed?"":"NOT ")<<"closed."<<std::endl;
  if (!closed)
  {
    std::cout << "Closing mesh..."<<std::endl;
    obj.close();
    std::cout << "Mesh is "<<(obj.isClosed()?"":"NOT ")<<"closed."<<std::endl;
  }
  std::cout << "Computing "<<rx<<'x'<<ry<<'x'<<rz<<" DistMap..."<<std::endl;
  obj.calcDistMap(rx,ry,rz,(border<0 ? -border : obj.calcBBox().size()*border));
  std::cout << "Saving result..."<<std::endl;
  obj.save(file_out.c_str());
  return 0;
}
