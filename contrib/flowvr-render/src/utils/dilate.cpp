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
* File: ./src/utils/dilate.cpp                                    *
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
  float dist=0.1;
  flowvr::utils::CmdLine cmd("Usage: flowvr-dilate [options] mesh.input mesh.output");
  cmd.opt("dist",'d',"dilatation distance: all points will be moved by this factor times the surface normal vector",&dist);
  bool error=false;
  if (!cmd.parse(argc,argv,&error))
    return error?1:0;

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

  std::cout << "Mesh bbox="<<obj.calcBBox()<<std::endl;

  std::cout << "Computing dilate..."<<std::endl;
  obj.dilate(dist);
  std::cout << "Saving result..."<<std::endl;
  obj.save(file_out.c_str());
  return 0;
}
