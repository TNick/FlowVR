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
* File: .//FlowVRRender/vtkFlowVRImageActor.h                                        *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
// .NAME vtkFlowVRImageActor - FlowVRImageActor class for vtk
// .SECTION Description
// None.

#ifndef __vtkFlowVRImageActor_h
#define __vtkFlowVRImageActor_h

#include "vtkImageActor.h"

#include <flowvr/id.h>

class vtkWindow;
class vtkFlowVRRenderer;
class vtkRenderWindow;

class VTK_RENDERING_EXPORT vtkFlowVRImageActor : public vtkImageActor
{
public:
  static vtkFlowVRImageActor *New();
  vtkTypeRevisionMacro(vtkFlowVRImageActor,vtkImageActor);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Implement base class method.
  void Load(vtkRenderer *ren);
  
  // Description:
  // Release any graphics resources that are being consumed by this texture.
  // The parameter window could be used to determine which graphic
  // resources to release. Using the same texture object in multiple
  // render windows is NOT currently supported. 
  void ReleaseGraphicsResources(vtkWindow *);

protected:
  vtkFlowVRImageActor();
  ~vtkFlowVRImageActor();

  vtkTimeStamp   LoadTime;

  //BTX
  flowvr::ID     Index;
  flowvr::ID     IDPrim;
  flowvr::ID     IDVB;
  flowvr::ID     IDIB;
  //ETX

  int            Gen;

  vtkRenderWindow *RenderWindow;   // RenderWindow used for previous render
  int TextureSize[2];
private:
  vtkFlowVRImageActor(const vtkFlowVRImageActor&);  // Not implemented.
  void operator=(const vtkFlowVRImageActor&);  // Not implemented.
};

#endif
