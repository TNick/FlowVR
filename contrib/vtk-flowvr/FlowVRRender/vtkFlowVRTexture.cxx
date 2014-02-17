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
* File: .//FlowVRRender/vtkFlowVRTexture.cxx                                         *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
#include "vtkFlowVRTexture.h"
#include "vtkObjectFactory.h"

#include "vtkMapper.h"
#include "vtkMatrix4x4.h"
#include "vtkProperty.h"

#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkRenderWindow.h"

#include "vtkFlowVRRenderer.h"
#include "vtkFlowVRRenderWindow.h"

#include <flowvr/module.h>
#include <flowvr/render/chunkrenderwriter.h>

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFlowVRTexture, "$Revision: 5485 $");
vtkStandardNewMacro(vtkFlowVRTexture);

vtkFlowVRTexture::vtkFlowVRTexture()
: Index(0), RenderWindow(NULL), Gen(0)
{
}

vtkFlowVRTexture::~vtkFlowVRTexture()
{
  RenderWindow = NULL;
}

void vtkFlowVRTexture::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

// Description:
// Implement base class method.
void vtkFlowVRTexture::Load(vtkRenderer *ren)
{
  vtkImageData *input = this->GetInput();

  if (!ren->IsA("vtkFlowVRRenderer"))
  {
    vtkErrorMacro(<< "Renderer must be a vtkFlowVRRenderer");
    return;
  }

  vtkFlowVRRenderer* fren = static_cast<vtkFlowVRRenderer*>(ren);
  flowvr::render::ChunkWriter* scene = fren->GetSceneWriter();
  
  
  // need to reload the texture
  if (this->GetMTime() > this->LoadTime.GetMTime() ||
      input->GetMTime() > this->LoadTime.GetMTime() ||
      (this->GetLookupTable() && this->GetLookupTable()->GetMTime () >  
       this->LoadTime.GetMTime()) || 
       ren->GetRenderWindow() != this->RenderWindow)
  {
    int pixelType = 0;
    int imageType = 0;
    //int bytesPerPixel;
    int *size;
    vtkDataArray *scalars;
    unsigned char *dataPtr;
    int rowLength;

    // get some info
    size = input->GetDimensions();
    scalars = input->GetPointData()->GetScalars();

    // make sure scalars are non null
    if (!scalars) 
    {
      vtkErrorMacro(<< "No scalar values found for texture input!");
      return;
    }

    // make sure using unsigned char data of color scalars type
    if (this->MapColorScalarsThroughLookupTable ||
	scalars->GetDataType() != VTK_UNSIGNED_CHAR )
    {
      dataPtr = this->MapScalarsToColors (scalars);
      pixelType = flowvr::render::Type::Vec4b;
    }
    else
    {
      dataPtr = static_cast<vtkUnsignedCharArray *>(scalars)->GetPointer(0);
      pixelType = flowvr::render::Type::vector(flowvr::render::Type::Byte,scalars->GetNumberOfComponents());
    }

    // Find the first dimension not 1
    if (size[0]==1)
    {
      if (size[1]==1)
      {
	size[0] = size[2];
	size[2] = 1;
      }
      else
      {
	size[0] = size[1];
	size[1] = size[2];
	size[2] = 1;
      }
    }

    if (!Index)
    {
      Index = static_cast<vtkFlowVRRenderer*>(ren)->GenerateID();
    }
    if (!Index)
    {
      vtkErrorMacro(<< "Failed to generate an ID!");
      return;
    }

    flowvr::render::ChunkTexture* texture = scene->addTexture(Index, imageType, pixelType, size[0], size[1], ((size[2]==1)?0:size[2]));
    texture->gen = Gen;
    memcpy(texture->data(), dataPtr, texture->dataSize());

    this->RenderWindow = ren->GetRenderWindow();
    // modify the load time to the current time
    this->LoadTime.Modified();
    ++this->Gen;
  }
}
  
// Description:
// Release any graphics resources that are being consumed by this texture.
// The parameter window could be used to determine which graphic
// resources to release. Using the same texture object in multiple
// render windows is NOT currently supported. 
void vtkFlowVRTexture::ReleaseGraphicsResources(vtkWindow *win)
{
  if (Index!=0 && win!=NULL && win->IsA("vtkFlowVRRenderWindow"))
  {
    flowvr::render::ChunkWriter* scene = static_cast<vtkFlowVRRenderWindow*>(win)->GetSceneWriter();
    scene->delTexture(Index);
    Index = 0;
    RenderWindow = NULL;
    Modified();
  }
}

