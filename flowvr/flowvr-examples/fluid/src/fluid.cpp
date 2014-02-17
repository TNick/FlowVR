/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                     Application Library                         *
*                                                                 *
*-----------------------------------------------------------------*
 * COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
* ALL RIGHTS RESERVED.	                                          *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING file for further information.                           *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Thomas Arcila,                                               *
*    Jean-Denis Lesage.                                           *	
*    Clement Menier,                                              *
*    Bruno Raffin                                                 *
*                                                                 *
*******************************************************************
*                                                                 *
* File: ./flowvr.cpp                                              *
*                                                                 *
* Contacts:                                                       *
*  2001-2004  Jeremie Allard <Jeremie.Allard@imag.fr>             *
*                                                                 *
******************************************************************/

#include "fluid/std.h"
#include "fluid/mytime.h"
#include "fluid/Turbulent.h"

#include "flowvr/module.h"
#include "ftl/chunkevents.h"

using namespace flowvr;


#define NX0 256
#define NY0 256

Turbulent *sim=NULL;


int  appInit(int argc, char **argv)
{
  int nx=NX0;
  int ny=NY0;
  for (int i=0;i<argc;i++)
    printf("argv[%d]=\"%s\"\n",i,argv[i]);

  if (argc >= 3 && argv[argc-2][0]!='-' && argv[argc-1][0]!='-')
  {
    nx = atoi(argv[argc-2]);
    ny = atoi(argv[argc-1]);
    if (nx<=0) nx=NX0;
    if (ny<=0) ny=NY0;
  }
  sim=new Turbulent(nx,ny);
  sim->Init();
  return 0;
}

void appUpdate()
{
  sim->update();
}

int  appQuit()
{
  delete sim;
  sim=NULL;
  return 0;
}



int nb_proc,local_proc;

int get_nb_proc()
{
  return flowvr::Parallel::getNbProc();
}

int get_local_proc()
{
  return flowvr::Parallel::getRank();
}

namespace flowvrmod
{ // FLOWVR DATA
flowvr::ModuleAPI* Module;
flowvr::InputPort PortPositions("positions");
flowvr::StampList PortOutStamps;
flowvr::StampInfo StampP("P",flowvr::TypeArray::create(2,flowvr::TypeInt::create()));
flowvr::StampInfo StampN("N",flowvr::TypeArray::create(2,flowvr::TypeInt::create()));
flowvr::OutputPort PortDensity("density",&PortOutStamps);
flowvr::OutputPort PortVelocity("velocity",&PortOutStamps);
flowvr::OutputPort PortPosition("position");
flowvr::BufferPool OutPool(10);
flowvr::BufferPool OutPosPool(10);
};

struct MPos
{
  float x,y,z;
  int bt;
};

int f0 = 0;
mytime t0 = 0;
mytime lastt = 0;

int main(int argc, char **argv)
{
  int r;

#ifdef USE_MPI
  MPI_Init(&argc,&argv);

  // retreive  the total number of module instance started
  MPI_Comm_size(MPI_COMM_WORLD,&nb_proc);

  // retreive  the MPI rank of the current module
  MPI_Comm_rank(MPI_COMM_WORLD,&local_proc);

  // Forward these values to flowvr that uses them to build the module name. 
  flowvr::Parallel::init(local_proc, nb_proc);

#else
  // Running in  a sequential mode  without MPI
  nb_proc=1; 
  local_proc=0;
#endif



  bool inputPosition=true;

  float sensibility = 1;
  bool clip = false;

  while (argc>1)
  {
    if (!strcmp(argv[1],"--nopos"))
    {
      --argc; ++argv;
      inputPosition=false;
    }
    else if (!strcmp(argv[1],"--clip"))
    {
      --argc; ++argv;
      clip = true;
    }
    else if (!strncmp(argv[1],"--sens=",7))
    {
      sensibility=atof(argv[1]+7);
      if (sensibility==0) sensibility=1;
      --argc; ++argv;
    }
    else
      break;
  }

  std::vector<flowvr::Port*> ports;
  // PortOutStamps is the stamps list of both output ports
  flowvrmod::PortOutStamps.add(&flowvrmod::StampP);
  flowvrmod::PortOutStamps.add(&flowvrmod::StampN);

  if (inputPosition)
    ports.push_back(&flowvrmod::PortPositions);

  ports.push_back(&flowvrmod::PortDensity);
  ports.push_back(&flowvrmod::PortVelocity);
  ports.push_back(&flowvrmod::PortPosition);

  // This will automatically adds "/X" at the end of the module name (X = local_proc).
  flowvrmod::Module = flowvr::initModule(ports);

  if (flowvrmod::Module == NULL)
  {
#ifdef USE_MPI
    MPI_Finalize();
#endif
    return 1;
  }

  if ((r=appInit(argc,argv))!=0)
  {
#ifdef USE_MPI
    MPI_Finalize();
#endif
    return r; /* application error */
  }
  std::cout << "Simulation local grid at <" << sim->getLPx() << ',' << sim->getLPy() << "> size <" << sim->getLNx() << ',' << sim->getLNy() << ">" << std::endl;
  {
    int datasize = sim->getLNx()*sim->getLNy()*2; //*sizeof(float);
    int it=0;
    int lastbt = 0;
    while (flowvrmod::Module->getStatus()) // loop until error
    {

      // Send Velocity
      if (flowvrmod::PortVelocity.isConnected())
      {
		flowvr::MessageWrite msgvel;
		msgvel.stamps.write(flowvrmod::StampP[0],sim->getLPx());
		msgvel.stamps.write(flowvrmod::StampP[1],sim->getLPy());
		msgvel.stamps.write(flowvrmod::StampN[0],sim->getLNx());
		msgvel.stamps.write(flowvrmod::StampN[1],sim->getLNy());
		msgvel.data = flowvrmod::OutPool.alloc(flowvrmod::Module->getAllocator(),datasize);
		sim->getLocalVelocity(msgvel.data.getWrite<unsigned char>(0));
		flowvrmod::Module->put(&flowvrmod::PortVelocity,msgvel);
      }
      // Send Density
      if (flowvrmod::PortDensity.isConnected())
      {
		flowvr::MessageWrite msgdens;
		msgdens.stamps.write(flowvrmod::StampP[0],sim->getLPx());
		msgdens.stamps.write(flowvrmod::StampP[1],sim->getLPy());
		msgdens.stamps.write(flowvrmod::StampN[0],sim->getLNx());
		msgdens.stamps.write(flowvrmod::StampN[1],sim->getLNy());
		msgdens.data = flowvrmod::OutPool.alloc(flowvrmod::Module->getAllocator(),datasize);
		sim->getLocalDensity(msgdens.data.getWrite<unsigned char>(0));
		flowvrmod::Module->put(&flowvrmod::PortDensity,msgdens);
      }

      // Send pointer position
      if (flowvrmod::PortPosition.isConnected())
      {
        flowvr::MessageWrite msg;
        msg.data = flowvrmod::OutPosPool.alloc(flowvrmod::Module->getAllocator(),sizeof(MPos));
        MPos* pos = msg.data.getWrite<MPos>();
        pos->x = sim->getCurrentPos().x;
        pos->y = sim->getCurrentPos().y;
        pos->z = 0;
        pos->bt = lastbt;
        flowvrmod::Module->put(&flowvrmod::PortPosition,msg);
      }

      flowvrmod::Module->wait(); // ITERATION STEP

      // Read Positions if turned on
      if (inputPosition)
      {

        // Variable that will hold the message that contain the list of chunks
        flowvr::Message msgpos;

        // Gets the message from flowvr system
        flowvrmod::Module->get(&flowvrmod::PortPositions,msgpos);

        // Vector used to interact with the fluid
        Vec2D *pos;

        // Looks for each chunk in the list of chunks received
        for (ftl::ChunkIterator it = ftl::chunkBegin(msgpos) ; it != ftl::chunkEnd(msgpos) ; it++)
        {

          const ftl::Chunk* c = (const ftl::Chunk*) it;	// Convert the iterator in a chunk base structure

          switch (c->type & 0x0F) 			// Look for each type of IO input
          {

            case ftl::ChunkEvent::MOUSE:		// In the case of a mouse input

              // Fills the vector used to interact with the fluid
              pos = new Vec2D(
                        sim->getNx() * ((ftl::ChunkEventMouse *)c)->mouseTranslation[0] * sensibility + 1,
                        sim->getNy() * ((ftl::ChunkEventMouse *)c)->mouseTranslation[1] * sensibility + 1
                             );

              if (clip)
              {
                static Vec2D dpos(0,0);
                if (pos->x+dpos.x < 1) dpos.x = 1-pos->x;
                else if (pos->x+dpos.x > sim->getNx()) dpos.x = sim->getNx()-pos->x;
                if (pos->y+dpos.y < 1) dpos.y = 1-pos->y;
                else if (pos->y+dpos.y > sim->getNy()) dpos.y = sim->getNy()-pos->y;
                pos->x += dpos.x; pos->y += dpos.y; 
              }

              /// Gets the mouse status from the chunk
              sim->MouseMotion(*pos,(int)((ftl::ChunkEventMouse *)c)->mouseKeys);
              lastbt = (int)((ftl::ChunkEventMouse *)c)->mouseKeys;

              break;

          }

        }  // end of chunks look looping

      } // end of input if 

      // Compute new velocity & density
      appUpdate();

      if (it==0)
	lastt = t0 = gettime();
      else
      if (!(it%100))
      {
	mytime t = gettime();
	if (local_proc==0)
	{
	  std::cout << "fps "<<it/100<<" = "<<100/time2s(t-lastt)<<"  mean = "<<it/time2s(t-t0)<<std::endl;
	}
	lastt=t;
      }
      ++it;
    }
  }
  r=appQuit();

  flowvrmod::Module->close();

#ifdef USE_MPI
  MPI_Finalize();
#endif

//  delete flowvrmod::Module;

  return r;
}

