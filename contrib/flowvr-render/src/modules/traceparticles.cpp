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
* File: ./src/modules/traceparticles.cpp                          *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <flowvr/utils/cmdline.h>
#include <flowvr/module.h>
#include <ftl/vec.h>
#include <ftl/mat.h>
#include <ftl/rmath.h>

using namespace flowvr;
using namespace ftl;

flowvr::utils::Option<int> optNX("nx",'x',"number of particles per line",16);
flowvr::utils::Option<int> optNY("ny",'y',"number of particles per column",16);
flowvr::utils::Option<float> optSpeed("speed",'s',"speed scaling factor",1.0f);
flowvr::utils::Option<Vec3f> optPos("pos",'p',"translation",Vec3f(0,0,0));
flowvr::utils::Option<Vec3f> optP00("p00",'a',"position of top-left particle",Vec3f(-0.9,-0.9,0.5));
flowvr::utils::Option<Vec3f> optP01("p01",'b',"position of top-right particle",Vec3f(0.9,-0.9,0.5));
flowvr::utils::Option<Vec3f> optP10("p10",'c',"position of bottom-left particle",Vec3f(-0.9,0.9,0.5));
flowvr::utils::Option<Vec3f> optP11("p11",'d',"position of bottom-right particle",Vec3f(0.9,0.9,0.5));
flowvr::utils::Option<Vec3f> optC00("c00",'A',"color of top-left particle",Vec3f(1,0,0));
flowvr::utils::Option<Vec3f> optC01("c01",'B',"color of top-right particle",Vec3f(0,1,0));
flowvr::utils::Option<Vec3f> optC10("c10",'C',"color of bottom-left particle",Vec3f(0,0,1));
flowvr::utils::Option<Vec3f> optC11("c11",'D',"color of bottom-right particle",Vec3f(1,0,0));
flowvr::utils::FlagOption optStag("stag",'S',"velocity grid is staggered (i.e. values correspond to cell borders)");
flowvr::utils::FlagOption optStart("start",'s',"send the initial position of particles before the first iteration");

ModuleAPI* module;

class GridInputPort : public flowvr::InputPort
{
public:
  StampInfo stampSizes;
  StampInfo  stampType;
  GridInputPort(const char*name="grid")
  : InputPort(name),
    stampSizes("sizes", TypeArray::create(3,TypeInt::create())),
    stampType("type", TypeString::create())
  {
    stamps->add(&stampSizes);
    stamps->add(&stampType);
  }
};
class GridOutputPort : public flowvr::OutputPort
{
public:
  StampInfo stampSizes;
  StampInfo  stampType;
  GridOutputPort(const char*name="grid")
  : OutputPort(name),
    stampSizes("sizes", TypeArray::create(3,TypeInt::create())),
    stampType("type", TypeString::create())
  {
    stamps->add(&stampSizes);
    stamps->add(&stampType);
  }
};

template<class T>
class Grid
{
public:
  flowvr::Buffer data;
  int nx,ny,nz,nxny;
  T def;

  Grid() : nx(0), ny(0), nz(0), nxny(0) {}

  void clear()
  {
    data.clear();
    nx=0;
    ny=0;
    nz=0;
    nxny=0;
  }

  bool read(const GridInputPort* port, const flowvr::Message& msg)
  {
    if (port==NULL || !msg.valid()) return false;
    clear();
    msg.stamps.read(port->stampSizes[0],nz);
    msg.stamps.read(port->stampSizes[1],ny);
    msg.stamps.read(port->stampSizes[2],nx);
    std::string typestr;
    msg.stamps.read(port->stampType,typestr);
    unsigned int t = ftl::Type::fromName(typestr);
    if (t != Type::get<T>(T()))
    {
      std::cerr << "ERROR: Received grid message with invalid type "<<typestr<<" != "<<Type::name(Type::get<T>(T()))<<std::endl;
      clear();
      return false;
    }
    nxny=nx*ny;
    if ((unsigned)msg.data.getSize()>nxny*nz*sizeof(T))
    {
      std::cerr << "WARNING: Received grid message with "<<msg.data.getSize()-(nxny*nz*sizeof(T))<<" extra bytes"<<std::endl;
    }
    else if ((unsigned)msg.data.getSize()<nxny*nz*sizeof(T))
    {
      std::cerr << "WARNING: Received grid message missing "<<(nxny*nz*sizeof(T))-msg.data.getSize()<<" bytes"<<std::endl;
      clear();
      return false;
    }
    data = Buffer(msg.data,0,nxny*nz*sizeof(T));
    return true;
  }

  bool get(GridInputPort* port, flowvr::Message& msg)
  {
    if (!port->isConnected())
    {
      std::cerr << "ERROR: Port "<<port->name<<" is not connected\n";
      return false;
    }
    if (!port->getModule()->get(port,msg))
    {
      std::cerr << "ERROR: get on port "<<port->name<<" failed\n";
      return false;
    }
    return read(port, msg);
  }

  bool get(GridInputPort* port)
  {
    flowvr::Message msg;
    return get(port,msg);
  }

  const T& operator()(int x, int y, int z)
  {
    if ((unsigned)x>=(unsigned)nx || (unsigned)y>=(unsigned)ny || (unsigned)z>=(unsigned)nz)
      return def;
    // The following line crashes GCC 3.4
    //return data.template getRead<T>()[x+y*nx+z*nxny];
    // but this one doesn't...
    return *(data.template getRead<T>()+(x+y*nx+z*nxny));
  }

  T interp(const Vec3f& pos)
  {
    int x = rfloor(pos[0]); float fx = pos[0]-x;
    int y = rfloor(pos[1]); float fy = pos[1]-y;
    int z = rfloor(pos[2]); float fz = pos[2]-z;
    if ((unsigned)x>=(unsigned)(nx-1) || (unsigned)y>=(unsigned)(ny-1) || (unsigned)z>=(unsigned)(nz-1))
    {
      std::cerr << "pos "<<pos<<" outside of grid "<<nx<<'x'<<ny<<'x'<<nz<<" ("<<Vec3i(x,y,z)<<")\n";
      return def;
    }
    else
    {
      const T* base = data.template getRead<T>()+(x+y*nx+z*nxny);
      return rlerp(rlerp(rlerp(base[0        ], base[1        ], fx),
		         rlerp(base[  nx     ], base[1+nx     ], fx), fy),
		   rlerp(rlerp(base[0   +nxny], base[1   +nxny], fx),
		         rlerp(base[  nx+nxny], base[1+nx+nxny], fx), fy), fz);
    }
  }

  template<int C>
  typename T::value_type interpC(const Vec3f& pos)
  {
    int x = rfloor(pos[0]); float fx = pos[0]-x;
    int y = rfloor(pos[1]); float fy = pos[1]-y;
    int z = rfloor(pos[2]); float fz = pos[2]-z;
    if ((unsigned)x>=(unsigned)(nx-1) || (unsigned)y>=(unsigned)(ny-1) || (unsigned)z>=(unsigned)(nz-1))
    {
      std::cerr << "pos"<<C<<" "<<pos<<" outside of grid "<<nx<<'x'<<ny<<'x'<<nz<<" ("<<Vec3i(x,y,z)<<")\n";
      return def[C];
    }
    else
    {
      const T* base = data.template getRead<T>()+(x+y*nx+z*nxny);
      return rlerp(rlerp(rlerp(base[0        ][C], base[1        ][C], fx),
		         rlerp(base[  nx     ][C], base[1+nx     ][C], fx), fy),
		   rlerp(rlerp(base[0   +nxny][C], base[1   +nxny][C], fx),
		         rlerp(base[  nx+nxny][C], base[1+nx+nxny][C], fx), fy), fz);
    }
  }

  T interpStag(const Vec3f& pos)
  {
    return T(interpC<0>(Vec3f(pos[0]+0.5f,pos[1],pos[2])),
	     interpC<1>(Vec3f(pos[0],pos[1]+0.5f,pos[2])),
	     interpC<2>(Vec3f(pos[0],pos[1],pos[2]+0.5f)));
  }

  T operator()(const Vec3f& pos)
  {
    if (optStag)
      return interpStag(pos);
    else
      return interp(pos);
  }
};

GridInputPort portVelocity("velocity");
InputPort portMatrix("matrix");
InputPort portDt("dt");
OutputPort portPoints("points");
BufferPool poolPoints;
OutputPort portColors("colors");
BufferPool poolColors;

Grid<Vec3f> vel;
float dt;
Mat4x4f mat;
Mat4x4f inv_mat;

struct Particle
{
  Vec3f p;
  Vec3f c;
};

std::vector<Particle> particles;
int nbparticles = 0;

void partInit()
{
  mat.identity();
  inv_mat.identity();
  dt = 0.01;
  nbparticles = optNX*optNY;
  particles.resize(nbparticles);
  int p=0;
  for (int y=0;y<optNY;y++) 
  {
    float fy = float(y)/(optNY-1);
    Vec3f p0 = rlerp((Vec3f)optP00,(Vec3f)optP10,fy);
    Vec3f p1 = rlerp((Vec3f)optP01,(Vec3f)optP11,fy);
    Vec3f c0 = rlerp((Vec3f)optC00,(Vec3f)optC10,fy);
    Vec3f c1 = rlerp((Vec3f)optC01,(Vec3f)optC11,fy);
    for (int x=0;x<optNX;x++,p++)
    {
      float fx = float(x)/(optNX-1);
      particles[p].p = rlerp(p0,p1,fx)+optPos;
      particles[p].c = rlerp(c0,c1,fx);
    }
  }
}

void partStep()
{
  float maxv=0.0000001;
  // find max vel
  for (int z=0;z<vel.nz;z++)
  for (int y=0;y<vel.ny;y++)
  for (int x=0;x<vel.nx;x++)
  {
    float v = vel(x,y,z).norm();
    if (v>maxv) maxv=v;
  }
  std::cout << "maxv="<<maxv<<std::endl;
  for (int p=0;p<nbparticles;p++)
  {
    Vec3f pos = particles[p].p;
    // Mid-point euler integration
    Vec3f v = vel(transform(inv_mat,pos));
    Vec3f p1_2 = pos+v*(optSpeed*dt*0.5f);
    v = vel(transform(inv_mat,p1_2));
    pos += v*(optSpeed*dt);
    particles[p].p = pos;
    //particles[p].c.fill(v.norm()/maxv);
  }
}

void partPut()
{
  MessageWrite mpoints, mcolors;
  mpoints.data = poolPoints.alloc(module->getAllocator(),nbparticles*sizeof(Vec3f));
  mcolors.data = poolColors.alloc(module->getAllocator(),nbparticles*sizeof(Vec3f));
  for (int p=0;p<nbparticles;p++)
  {
    mpoints.data.getWrite<Vec3f>()[p] = particles[p].p;
    mcolors.data.getWrite<Vec3f>()[p] = particles[p].c;
  }
  module->put(&portPoints,mpoints);
  module->put(&portColors,mcolors);
}

int main(int argc, char** argv)
{
  flowvr::utils::CmdLine cmd("Particles tracer");
  bool error=false;
  if (!cmd.parse(argc,argv,&error))
    return error?1:0;

  std::vector<Port*> ports;
  ports.push_back(&portVelocity);
  ports.push_back(&portMatrix);
  ports.push_back(&portDt);
  ports.push_back(&portPoints);
  ports.push_back(&portColors);
  module = flowvr::initModule(ports);
  if (!module) return 1;

  partInit();

  int it = 0;

  if (optStart)
    partPut();

  while (module->wait())
  {
    if (portMatrix.isConnected())
    {
      flowvr::Message msg;
      module->get(&portMatrix,msg);
      if (msg.data.getSize()>=(int)sizeof(Mat4x4f))
      {
	mat = *msg.data.getRead<Mat4x4f>();
	inv_mat.invert(mat);
      }
    }
    if (portDt.isConnected())
    {
      flowvr::Message msg;
      module->get(&portDt,msg);
      if (msg.data.getSize() == sizeof(float)) dt = *msg.data.getRead<float>();
      else if (msg.data.getSize() == sizeof(double)) dt = *msg.data.getRead<double>();
      else std::cerr << "Invalid dt message size "<<msg.data.getSize()<<std::endl;
    }
    if (vel.get(&portVelocity))
    {
      partStep();
    }
    else std::cerr << "get velocity failed!\n"<<std::endl;
    partPut();
    ++it;
  }

  return 0;
}
