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
* File: .//FlowVRRender/vtkFlowVRTexture.h                                           *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
// .NAME vtkFlowVRTexture - FlowVRTexture class for vtk
// .SECTION Description
// None.

#ifndef __vtkFlowVRTexture_h
#define __vtkFlowVRTexture_h

#include "vtkTexture.h"
#include "vtkRenderWindow.h"
#include <flowvr/id.h>

class VTK_RENDERING_EXPORT vtkFlowVRTexture : public vtkTexture
{
public:
  static vtkFlowVRTexture *New();
  vtkTypeRevisionMacro(vtkFlowVRTexture,vtkTexture);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Implement base class method.
  void Load(vtkRenderer *ren);
  
  // Description:
  // Release any graphics resources that are being consumed by this texture.
  // The parameter window could be used to determine which graphic
  // resources to release. Using the same texture object in multiple
  // render windows is NOT currently supported. 
  void ReleaseGraphicsResources(vtkWindow *win);

  //BTX
  flowvr::ID GetID() { return Index; }
  //ETX

protected:
  vtkFlowVRTexture();
  ~vtkFlowVRTexture();

  vtkTimeStamp   LoadTime;
  //BTX
  flowvr::ID Index;
  //ETX
  vtkRenderWindow *RenderWindow;   // RenderWindow used for previous render
  int Gen;

private:
  vtkFlowVRTexture(const vtkFlowVRTexture&);  // Not implemented.
  void operator=(const vtkFlowVRTexture&);  // Not implemented.
};

#endif 
