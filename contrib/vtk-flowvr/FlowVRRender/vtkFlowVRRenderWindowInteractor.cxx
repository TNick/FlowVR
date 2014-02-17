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
* File: .//FlowVRRender/vtkFlowVRRenderWindowInteractor.cxx                          *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
#include "vtkFlowVRRenderWindowInteractor.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro (vtkFlowVRRenderWindowInteractor, "$Revision: 5485 $");
vtkStandardNewMacro (vtkFlowVRRenderWindowInteractor);

vtkFlowVRRenderWindowInteractor::vtkFlowVRRenderWindowInteractor()
{
  cout << "Interactor" << endl;
  cout << __PRETTY_FUNCTION__ << endl;
}

vtkFlowVRRenderWindowInteractor::~vtkFlowVRRenderWindowInteractor()
{
  cout << __PRETTY_FUNCTION__ << endl;
}

void
vtkFlowVRRenderWindowInteractor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void
vtkFlowVRRenderWindowInteractor::Initialize() 
{
  cout << __PRETTY_FUNCTION__ << endl;
}
