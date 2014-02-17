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
* File: src/modules/writeobj.cpp                                  *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <flowvr/module.h>
#include <flowvr/utils/cmdline.h>
#include <ftl/vec.h>
#include <ftl/mat.h>
#include "flowvr/render/chunkrenderreader.h"
#include "flowvr/render/mesh.h"

using namespace ftl;
using namespace flowvr;
using namespace flowvr::render;

ModuleAPI* module;

InputPort portScene("scene");
// Same port as renderer
InputPort portDt("dt");
InputPort portKeys("keys");
InputPort portMouse("mouse");

flowvr::render::ChunkRenderReader<> scene;

flowvr::utils::Option<int> optFreq("freq",'f',"interval between files",1);
flowvr::utils::Option<int> optScene("scene",'s',"number of scene ports (named scene-0 scene-1 ...)",0);

int main(int argc, char** argv)
{
  flowvr::utils::CmdLine cmd("OBJ Writer\nUsage: flowvr-writeobj [options] dest [primname=newmesh.obj]...");
  bool error=false;
  if (!cmd.parse(argc,argv,&error))
    return error?1:0;
  if (cmd.args.size()<1)
  {
    std::cerr << cmd.help() << std::endl;
    return 1;
  }

  std::string dest = cmd.args[0];

  std::map<std::string,Mesh> meshes;

  for (unsigned int i=1;i<cmd.args.size();i++)
  {
    std::string name=cmd.args[i];
    Mesh mesh;
    int eqpos = name.find('=');
    if ((unsigned)eqpos<name.size())
    {
      std::string file(name, eqpos+1);
      name.resize(eqpos);
      meshes[name].load(file.c_str());
    }
    else
      meshes[name].clear();
  }

  std::vector<Port*> ports;
  ports.push_back(&portScene);
  ports.push_back(&portDt);
  ports.push_back(&portKeys);
  ports.push_back(&portMouse);

  std::vector<InputPort*> portScenes;
  for (int i=0;i<optScene;i++)
  {
    char buf[16];
    snprintf(buf,sizeof(buf),"scene-%d",i);
    InputPort* p = new InputPort(buf);
    portScenes.push_back(p);
    ports.push_back(p);
  }

  module = flowvr::initModule(ports);
  if (!module) return 1;

  int it = 0;

  while (module->wait())
  {

    Message m;
    module->get(&portScene,m);
    scene.process(m);
    for (unsigned int i=0;i<portScenes.size();i++)
    {
      module->get(portScenes[i],m);
      scene.process(m);
    }
    if ((it%optFreq)==0)
    {
      char buf[256];
      snprintf(buf, sizeof(buf), "%s%04d.obj",dest.c_str(),it);
      std::cout << "Output sequence file "<<buf<<std::endl;
      FILE* f = fopen(buf,"wb+");
      int ind0 = 1;
      for (flowvr::render::ChunkRenderReader<>::iterator it = scene.begin(); it!=scene.end(); it++)
      {
	      flowvr::render::ChunkRenderReader<>::Primitive* p = &(*it);
	if (!p->visible) continue;
	std::cout << p->name << std::endl;
	if (meshes.find(p->name) != meshes.end())
	{
	  Mesh& mesh = meshes[p->name];
	  if (mesh.nbp()>0)
	  {
	    fprintf(f,"g %s\n",p->name.c_str());
	    mesh.saveObj(f,ind0);
	  }
	  continue;
	}
	if (p->vBuffer.find("position") == p->vBuffer.end()) continue;
	flowvr::render::ChunkRenderReader<>::VertexBuffer* position = p->vBuffer["position"].buffer;
	int numData = p->vBuffer["position"].numData;
	int v0 = p->vBuffer["position"].v0;
	if (position == NULL || position->empty()) continue;
	const ChunkVertexBuffer* vb = position->firstLevel();
	if (numData >= vb->nbData) continue;
	int nbp = vb->dataSize()/vb->vertexSize - v0;
	if (p->nbv>=0 && p->nbv<nbp)
	  nbp=p->nbv;
	if (nbp <= 0) continue;

	flowvr::render::ChunkRenderReader<>::IndexBuffer* indices = p->iBuffer.buffer;
	if (indices==NULL || indices->empty()) continue;
	int i0 = p->iBuffer.i0;
	const ChunkIndexBuffer* ib = indices->firstLevel();
	int nbi = ib->dataSize()/Type::size(ib->dataType) - i0;
	if (p->iBuffer.nbi>=0 && p->iBuffer.nbi < nbi)
	  nbi = p->iBuffer.nbi;
	if (nbi<=0) continue;

	std::cout << nbp << " vertex, "<<nbi<<" indices" << std::endl;

	fprintf(f,"g %s\n",p->name.c_str());
	Mat4x4d mat ( p->xform.mat.ptr() );
	for (int i=0;i<nbp;i++)
	{
	  const void* data = vb->dataNum(numData,v0+i);
	  Vec3d pos;
	  if (data!=NULL)
	    ftl::Type::assign(pos,vb->dataType[numData],data);
	  pos = transform(mat,pos);
	  fprintf(f,"v %f %f %f\n",pos[0],pos[1],pos[2]);
	}

	switch (ib->primitive)
	{
	case ChunkIndexBuffer::Line:
	{
	  for (int i=0;i<nbi;i+=2)
	  {
	    int p[2];
	    for (int pi=0;pi<2;pi++)
	    {
	      p[pi]=i0+i+pi;
	      const void* data = ib->data(i0+i+pi);
	      if (data!=NULL)
		ftl::Type::assign(p[pi],ib->dataType,data);
	    }
	    if (p[0]>=nbp || p[1]>=nbp || p[2]>=nbp)
	      std::cerr << "Invalid line "<<i/2<<std::endl;
	    else
	      fprintf(f,"l %d %d\n",p[0]+ind0,p[1]+ind0);
	  }
	  break;
	}
	case ChunkIndexBuffer::Triangle:
	{
	  for (int i=0;i<nbi;i+=3)
	  {
	    int p[3];
	    for (int pi=0;pi<3;pi++)
	    {
	      p[pi]=i0+i+pi;
	      const void* data = ib->data(i0+i+pi);
	      if (data!=NULL)
		ftl::Type::assign(p[pi],ib->dataType,data);
	    }
	    if (p[0]>=nbp || p[1]>=nbp || p[2]>=nbp)
	      std::cerr << "Invalid triangle "<<i/3<<std::endl;
	    else
	      fprintf(f,"f %d %d %d\n",p[0]+ind0,p[1]+ind0,p[2]+ind0);
	  }
	  break;
	}
	case ChunkIndexBuffer::Quad:
	{
	  for (int i=0;i<nbi;i+=4)
	  {
	    int p[4];
	    for (int pi=0;pi<4;pi++)
	    {
	      p[pi]=i0+i+pi;
	      const void* data = ib->data(i0+i+pi);
	      if (data!=NULL)
		ftl::Type::assign(p[pi],ib->dataType,data);
	    }
	    if (p[0]>=nbp || p[1]>=nbp || p[2]>=nbp || p[3]>=nbp)
	      std::cerr << "Invalid quad "<<i/4<<std::endl;
	    else
	      fprintf(f,"f %d %d %d %d\n",p[0]+ind0,p[1]+ind0,p[2]+ind0,p[3]+ind0);
	  }
	  break;
	}
	case ChunkIndexBuffer::Point:
	default:
	{
	  for (int i=0;i<nbi;i++)
	  {
	    const void* data = ib->data(i0+i);
	    int p=i0+i;
	    if (data!=NULL)
	      ftl::Type::assign(p,ib->dataType,data);
	    if (p>=nbp)
	      std::cerr << "Invalid point "<<i<<std::endl;
	    else
	      fprintf(f,"p %d\n",p+ind0);
	  }
	  break;
	}
	}

	ind0 += nbp;
	
      }

      fclose(f);
    }
    ++it;
  }

  return 0;
}
