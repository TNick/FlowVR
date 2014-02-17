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
* File: .//FlowVRRender/vtkFlowVRRenderFactory.h                                     *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
// .NAME vtkFlowVRRenderFactory - FlowVRRenderFactory class for vtk
// .SECTION Description
// None.

#ifndef __vtkFlowVRRenderFactory_h
#define __vtkFlowVRRenderFactory_h

#include "vtkObjectFactory.h"

#include <flowvr/render/chunkrenderwriter.h>

class VTK_RENDERING_EXPORT vtkFlowVRRenderFactory : public vtkObjectFactory
{
public:
  static vtkFlowVRRenderFactory *New();
  vtkTypeRevisionMacro(vtkFlowVRRenderFactory,vtkObjectFactory);

  // Description:
  // All sub-classes of vtkObjectFactory should must return the version of 
  // VTK they were built with.  This should be implemented with the macro
  // VTK_SOURCE_VERSION and NOT a call to vtkVersion::GetVTKSourceVersion.
  // As the version needs to be compiled into the file as a string constant.
  // This is critical to determine possible incompatible dynamic factory loads.
  virtual const char* GetVTKSourceVersion();

  // Description:
  // Return a descriptive string describing the factory.
  virtual const char* GetDescription();

  void Register();

protected:
  vtkFlowVRRenderFactory();
  ~vtkFlowVRRenderFactory();

private:
  vtkFlowVRRenderFactory(const vtkFlowVRRenderFactory&);  // Not implemented.
  void operator=(const vtkFlowVRRenderFactory&);  // Not implemented.
};

#endif 
