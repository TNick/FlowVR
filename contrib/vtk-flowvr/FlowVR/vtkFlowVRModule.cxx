/******* COPYRIGHT ************************************************
*                                                                 *
*                         vtk-flowvr                              *
*          Distributed rendering for VTK using FlowVR Render      *
*                                                                 *
*-----------------------------------------------------------      *
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
* ALL RIGHTS RESERVED.                                            *
*                                                                 *
* This source is covered by the GNU GPL, please refer to the      *
* COPYING file for further information.                           *
*                                                                 *
*----------------------------------------------------------       *
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Thomas Arcila.                                               *
*                                                                 *
*******************************************************************
*                                                                                                          *
* File: .//FlowVR/vtkFlowVRModule.cxx                                                *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
#include "vtkFlowVRModule.h"
#include "vtkObjectFactory.h"
#include "vtkCommand.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <flowvr/module.h>
#include <flowvr/utils/timing.h>

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFlowVRModule, "$Revision: 5485 $");
vtkStandardNewMacro(vtkFlowVRModule);

vtkFlowVRModule *vtkFlowVRModule::DefaultModule = NULL;

vtkFlowVRModule::vtkFlowVRModule()
: Module(NULL), Suffix(NULL)
{
  DefaultModule = this;
}

vtkFlowVRModule::~vtkFlowVRModule()
{
}

void vtkFlowVRModule::AddInputPort(flowvr::InputPort* port)
{
  if (Module!=NULL)
  {
    vtkErrorMacro (<< "AddInputPort should be called before Init");
    return;
  }
  Ports.push_back(port);
}

void vtkFlowVRModule::AddOutputPort(flowvr::OutputPort* port)
{
  if (Module!=NULL)
  {
    vtkErrorMacro (<< "AddOutputPort should be called before Init");
    return;
  }
  Ports.push_back(port);
}

void vtkFlowVRModule::Initialize()
{
  if (Module!=NULL)
  {
    vtkErrorMacro (<< "Init should only be called once");
    return;
  }
  Module = flowvr::initModule(Ports, (Suffix==NULL)?"":Suffix);
}

int vtkFlowVRModule::GetStatus()
{
  if (Module == NULL) return 0;
  return Module->getStatus();
}

int vtkFlowVRModule::Wait()
{
  if (Module == NULL) return 0;
  InvokeEvent(vtkCommand::EnterEvent);
  int res = Module->wait();
  InvokeEvent(vtkCommand::LeaveEvent);
  return res;
}

void vtkFlowVRModule::Start()
{
  while (Wait())
    flowvr::utils::microsleep(1000);
}

void vtkFlowVRModule::Start(vtkRenderWindow* win)
{
  while (Wait())
  {
    win->Render();
    flowvr::utils::microsleep(1000);
  }
  exit(0);
}
