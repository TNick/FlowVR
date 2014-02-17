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
* File: .//FlowVRRender/vtkFlowVRActor.cxx                                           *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
#include "vtkFlowVRActor.h"
#include "vtkObjectFactory.h"

#include "vtkMapper.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkFlowVRRenderer.h"
#include "vtkProperty.h"

#include <flowvr/module.h>

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFlowVRActor, "$Revision: 5485 $");
vtkStandardNewMacro(vtkFlowVRActor);

// Actual actor render method.
void vtkFlowVRActor::Render(vtkRenderer *ren, vtkMapper *mapper)
{
  // send a render to the mapper; update pipeline
  mapper->Render(ren,this);
}

