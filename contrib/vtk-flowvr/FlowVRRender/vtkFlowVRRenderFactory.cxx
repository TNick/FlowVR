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
* File: .//FlowVRRender/vtkFlowVRRenderFactory.cxx                                   *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
#include "vtkVersion.h"
#include "vtkFlowVRRenderFactory.h"
#include "vtkFlowVRRenderWindow.h"
#include "vtkFlowVRRenderer.h"
#include "vtkFlowVRActor.h"
#include "vtkFlowVRImageActor.h"
#include "vtkFlowVRTexture.h"
#include "vtkFlowVRPolyDataMapper.h"
#include "vtkFlowVRProperty.h"

#include <vtkProperty.h>

VTK_CREATE_CREATE_FUNCTION(vtkFlowVRRenderer);
VTK_CREATE_CREATE_FUNCTION(vtkFlowVRRenderWindow);
VTK_CREATE_CREATE_FUNCTION(vtkFlowVRActor);
VTK_CREATE_CREATE_FUNCTION(vtkFlowVRImageActor);
VTK_CREATE_CREATE_FUNCTION(vtkFlowVRTexture);
VTK_CREATE_CREATE_FUNCTION(vtkFlowVRPolyDataMapper);
VTK_CREATE_CREATE_FUNCTION(vtkFlowVRProperty);




vtkCxxRevisionMacro(vtkFlowVRRenderFactory, "$Revision: 5485 $");
vtkStandardNewMacro(vtkFlowVRRenderFactory);

vtkFlowVRRenderFactory::vtkFlowVRRenderFactory()
{
  cout << "FlowVRRender Factor init\n";
  RegisterOverride("vtkRenderWindow",
		   "vtkFlowVRRenderWindow",
		   "FlowVRRender implementation of a rendering window",
		   1, vtkObjectFactoryCreatevtkFlowVRRenderWindow);
  RegisterOverride("vtkRenderer",
		   "vtkFlowVRRenderer",
		   "FlowVRRender implementation of a renderer",
		   1, vtkObjectFactoryCreatevtkFlowVRRenderer);
  RegisterOverride("vtkActor",
		   "vtkFlowVRActor",
		   "FlowVRRender implementation of an actor",
		   1, vtkObjectFactoryCreatevtkFlowVRActor);
  RegisterOverride("vtkProperty",
		   "vtkFlowVRProperty",
		   "Default creator for vtkProperties (avoid openGL resource allocation)",
		   1, vtkObjectFactoryCreatevtkFlowVRProperty);
  RegisterOverride("vtkImageActor",
		   "vtkFlowVRImageActor",
		   "FlowVRRender implementation of an image actor",
		   1, vtkObjectFactoryCreatevtkFlowVRImageActor);
  RegisterOverride("vtkTexture",
		   "vtkFlowVRTexture",
		   "FlowVRRender implementation of a texture",
		   1, vtkObjectFactoryCreatevtkFlowVRTexture);
  RegisterOverride("vtkPolyDataMapper",
		   "vtkFlowVRPolyDataMapper",
		   "FlowVRRender implementation of a PolyDataMapper",
		   1, vtkObjectFactoryCreatevtkFlowVRPolyDataMapper);
}

vtkFlowVRRenderFactory::~vtkFlowVRRenderFactory()
{
}

// return the version of VTK that the factory was built with
const char* vtkFlowVRRenderFactory::GetVTKSourceVersion(void)
{
  return VTK_SOURCE_VERSION;
}

// return a text description of the factory
const char* vtkFlowVRRenderFactory::GetDescription(void)
{
  return "ObjectFactory for concrete Rendering classes implemented via FlowVRRender";
}

// Macro to create the interface "C" functions used in
// a dll or shared library that contains a VTK object factory.
VTK_FACTORY_INTERFACE_IMPLEMENT(vtkFlowVRRenderFactory);

void vtkFlowVRRenderFactory::Register()
{
  vtkObjectFactory::RegisterFactory(this);
}
