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
*                                                                 *

* File: .//FlowVRRender/vtkFlowVRRenderWindow.cxx                 *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include "vtkFlowVRRenderWindow.h"
#include "vtkObjectFactory.h"
#include "vtkFlowVRModule.h"

#include <flowvr/module.h>

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFlowVRRenderWindow, "$Revision: 5485 $");
vtkStandardNewMacro(vtkFlowVRRenderWindow);

vtkFlowVRRenderWindow::vtkFlowVRRenderWindow()
: Port("scene"), Module(NULL)
{
  cout << "FlowVR Render Window opened.\n";
  bool createdByMe = false;
  if (Module != NULL) return;
  Module = vtkFlowVRModule::DefaultModule;
  if (Module == NULL) {
    Module = vtkFlowVRModule::New ();
    cout << "No default module found: creating one\n";
    createdByMe = true;
   }

  if (Module != NULL)
  {
    Module->Register(this);
    Module->AddOutputPort(&Port);
    cout << "FlowVR Render Window initialized.\n";
    if (createdByMe)
      Module->Initialize ();
  }	  
}

vtkFlowVRRenderWindow::~vtkFlowVRRenderWindow()
{
  Finalize();
}


// Description:
// Initialize the rendering window.  This will setup all system-specific
// resources.  This method and Finalize() must be symmetric and it
// should be possible to call them multiple times, even changing WindowId
// in-between.  This is what WindowRemap does.

// Description:
// "Deinitialize" the rendering window.  This will shutdown all system-specific
// resources.  After having called this, it should be possible to destroy
// a window that was used for a SetWindowId() call without any ill effects.
void vtkFlowVRRenderWindow::Finalize(void)
{
  if (Module != NULL)
  {
    Module->UnRegister(this);
    Module = NULL;
  }
}

void vtkFlowVRRenderWindow::SetPortName(const char* name)
{
  if (name!=NULL)
  {
    this->Port.name = name;
    this->Modified();
  }
}

const char* vtkFlowVRRenderWindow::GetPortName()
{
  return Port.name.c_str();
}

// Description:
// Generate a new ID
flowvr::ID vtkFlowVRRenderWindow::GenerateID()
{
  flowvr::ModuleAPI* module = Port.getModule();
  if (module == NULL)
    return 0;
  return module->generateID();
} 

// Description:
// Initialize the rendering process.
void vtkFlowVRRenderWindow::Start()
{
  //std::cout << "Start"<<std::endl;
}

// Description:
// A termination method performed at the end of the rendering process
// to do things like swapping buffers (if necessary) or similar actions.
void vtkFlowVRRenderWindow::Frame()
{
  if (Module == NULL) return;

  if(Scene.isDirty())
	  Scene.put(&Port);
}
