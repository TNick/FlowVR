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
* File: .//FlowVRRender/vtkFlowVRRenderer.cxx                                        *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
#include "vtkFlowVRRenderer.h"
#include "vtkFlowVRRenderWindow.h"
#include "vtkObjectFactory.h"
#include "vtkFlowVRModule.h"

#include <flowvr/module.h>

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFlowVRRenderer, "$Revision: 5485 $");
vtkStandardNewMacro(vtkFlowVRRenderer);

vtkFlowVRRenderer::vtkFlowVRRenderer()
{
}

vtkFlowVRRenderer::~vtkFlowVRRenderer()
{
}

// vtkRenderer virtual methods
void vtkFlowVRRenderer::DeviceRender()
{
  UpdateGeometry();
}

flowvr::render::ChunkWriter* vtkFlowVRRenderer::GetSceneWriter()
{
  if (GetRenderWindow()!=NULL
      && GetRenderWindow()->IsA("vtkFlowVRRenderWindow"))
    return static_cast<vtkFlowVRRenderWindow*>(GetRenderWindow())->GetSceneWriter();
}

flowvr::ID vtkFlowVRRenderer::GenerateID()
{
  if (GetRenderWindow()!=NULL
      && GetRenderWindow()->IsA("vtkFlowVRRenderWindow"))
    return static_cast<vtkFlowVRRenderWindow*>(GetRenderWindow())->GenerateID();
}
