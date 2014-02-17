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
* File: .//FlowVRRender/vtkFlowVRRenderer.h                       *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
// .NAME vtkFlowVRRenderer - FlowVRRenderer class for vtk
// .SECTION Description
// None.

#ifndef __vtkFlowVRRenderer_h
#define __vtkFlowVRRenderer_h

#include "vtkRenderer.h"

#include <flowvr/render/chunkrenderwriter.h>

class VTK_RENDERING_EXPORT vtkFlowVRRenderer : public vtkRenderer
{
public:
  static vtkFlowVRRenderer *New();
  vtkTypeRevisionMacro(vtkFlowVRRenderer,vtkRenderer);

  // vtkViewport picking methods
  virtual double GetPickedZ() { return 0; }
  virtual void DevicePickRender() {}
  virtual void StartPick(unsigned int) {}
  virtual void UpdatePickId() {}
  virtual void DonePick() {}
  virtual unsigned int GetPickedId() { return 0; }
  virtual unsigned int GetNumPickedIds() { return 0; }
  virtual int GetPickedIds(unsigned int, unsigned int*){ return 0; }

  // vtkRenderer virtual methods
  virtual void DeviceRender();

  //BTX
  // Description:
  // Get flowvr-render scene writer
  flowvr::render::ChunkWriter* GetSceneWriter();

  // Description:
  // Generate a new ID
  flowvr::ID GenerateID();
  //ETX

protected:
  vtkFlowVRRenderer();
  ~vtkFlowVRRenderer();

private:
  vtkFlowVRRenderer(const vtkFlowVRRenderer&);  // Not implemented.
  void operator=(const vtkFlowVRRenderer&);  // Not implemented.
};

#endif 
