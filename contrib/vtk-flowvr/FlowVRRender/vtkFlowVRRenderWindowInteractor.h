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
* File: .//FlowVRRender/vtkFlowVRRenderWindowInteractor.h                            *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
// .NAME vtkFlowVRRenderWindowInteractor - flowvr render window interaction not yet including picking and frame rate control.

// .SECTION Description
// None.

#ifndef __vtkFlowVRRenderWindowInteractor_h
#define __vtkFlowVRRenderWindowInteractor_h

#include "vtkFlowVRRenderer.h"
#include "vtkRenderWindowInteractor.h"

class VTK_RENDERING_EXPORT vtkFlowVRRenderWindowInteractor
  : public vtkRenderWindowInteractor
{
public:
  static vtkFlowVRRenderWindowInteractor *New();
  vtkTypeRevisionMacro(vtkFlowVRRenderWindowInteractor,vtkRenderWindowInteractor);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Prepare for handling events. This must be called before the
  // interactor will work.
  virtual void Initialize();
protected:
  vtkFlowVRRenderWindowInteractor();
  ~vtkFlowVRRenderWindowInteractor();
private:
  vtkFlowVRRenderWindowInteractor(const vtkFlowVRRenderWindowInteractor&);  // Not implemented.
  void operator=(const vtkFlowVRRenderWindowInteractor&);  // Not implemented.
};

#endif
