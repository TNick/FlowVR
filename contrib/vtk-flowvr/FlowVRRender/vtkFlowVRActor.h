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
* File: .//FlowVRRender/vtkFlowVRActor.h                                             *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
// .NAME vtkFlowVRActor - FlowVRActor class for vtk
// .SECTION Description
// None.

#ifndef __vtkFlowVRActor_h
#define __vtkFlowVRActor_h

#include "vtkActor.h"

class VTK_RENDERING_EXPORT vtkFlowVRActor : public vtkActor
{
public:
  static vtkFlowVRActor *New();
  vtkTypeRevisionMacro(vtkFlowVRActor,vtkActor);

  // Description:
  // Actual actor render method.
  void Render(vtkRenderer *ren, vtkMapper *mapper);

protected:
  vtkFlowVRActor() {};
  ~vtkFlowVRActor() {};

private:
  vtkFlowVRActor(const vtkFlowVRActor&);  // Not implemented.
  void operator=(const vtkFlowVRActor&);  // Not implemented.
};

#endif 
