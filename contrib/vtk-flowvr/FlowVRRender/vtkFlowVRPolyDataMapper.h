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
* File: .//FlowVRRender/vtkFlowVRPolyDataMapper.h                                    *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
// .NAME vtkFlowVRPolyDataMapper - FlowVRPolyDataMapper class for vtk
// .SECTION Description
// None.

#ifndef __vtkFlowVRPolyDataMapper_h
#define __vtkFlowVRPolyDataMapper_h

#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include <flowvr/id.h>
#include <vector>

class vtkFlowVRRenderPrimitiveData;

class vtkFlowVRPolyDataMapper : public vtkPolyDataMapper
{
public:
  static vtkFlowVRPolyDataMapper *New();
  vtkTypeRevisionMacro(vtkFlowVRPolyDataMapper,vtkPolyDataMapper);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Implemented by sub classes. Actual rendering is done here.
  virtual void RenderPiece(vtkRenderer *ren, vtkActor *act);

  // Description:
  // This calls RenderPiece (in a for loop if streaming is necessary).
  virtual void Render(vtkRenderer *ren, vtkActor *act);

  // Description:
  // Release any graphics resources that are being consumed by this texture.
  // The parameter window could be used to determine which graphic
  // resources to release. Using the same texture object in multiple
  // render windows is NOT currently supported. 
  void ReleaseGraphicsResources(vtkWindow *win);

protected:
  vtkFlowVRPolyDataMapper();
  ~vtkFlowVRPolyDataMapper();

  int CurrentPiece;
  int Gen;

  //BTX
  std::vector<vtkFlowVRRenderPrimitiveData*> Primitives;
  flowvr::ID m_vShaderId,
      m_pShaderId;
  //ETX

private:
  vtkFlowVRPolyDataMapper(const vtkFlowVRPolyDataMapper&);  // Not implemented.
  void operator=(const vtkFlowVRPolyDataMapper&);  // Not implemented.
};

#endif 
