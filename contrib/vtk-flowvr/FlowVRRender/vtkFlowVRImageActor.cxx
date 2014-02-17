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
* File: .//FlowVRRender/vtkFlowVRImageActor.cxx                                      *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
#include "vtkFlowVRImageActor.h"

#include "vtkMapper.h"
#include "vtkObjectFactory.h"
#include "vtkFlowVRRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"
#include "vtkFlowVRRenderWindow.h"

#include <math.h>

vtkCxxRevisionMacro(vtkFlowVRImageActor, "$Revision: 5939 $");
vtkStandardNewMacro(vtkFlowVRImageActor);

// Initializes an instance, generates a unique index.
vtkFlowVRImageActor::vtkFlowVRImageActor()
{
  this->IDPrim = 0;
  this->IDVB = 0;
  this->IDIB = 0;
  this->Gen = 0;
  this->RenderWindow = 0;
  this->TextureSize[0] = 0;
  this->TextureSize[1] = 0;
}

vtkFlowVRImageActor::~vtkFlowVRImageActor()
{
  this->RenderWindow = NULL;
}

// Release the graphics resources used by this texture.  
void vtkFlowVRImageActor::ReleaseGraphicsResources(vtkWindow *renWin)
{
  if (this->Index && renWin)
  {
    vtkFlowVRRenderWindow* fren = (vtkFlowVRRenderWindow *) renWin;
    flowvr::render::ChunkWriter* scene = fren->GetSceneWriter();
    if (scene != NULL)
    {
      scene->delPrimitive(IDPrim);
      scene->delVertexBuffer(IDVB);
      scene->delIndexBuffer(IDIB);
      scene->delTexture(Index);
    }
    this->TextureSize[0] = 0;
    this->TextureSize[1] = 0;
  }
  this->Index = 0;
  this->RenderWindow = NULL;
  this->Modified();
}

// Implement base class method.
void vtkFlowVRImageActor::Load(vtkRenderer *ren)
{
  if (!ren->IsA("vtkFlowVRRenderer"))
  {
    vtkErrorMacro(<< "Renderer must be a vtkFlowVRRenderer");
    return;
  }

  vtkFlowVRRenderer* fren = static_cast<vtkFlowVRRenderer*>(ren);
  flowvr::render::ChunkWriter* scene = fren->GetSceneWriter();

  // need to reload the texture
  if (this->GetMTime() > this->LoadTime.GetMTime() ||
      this->GetInput()->GetMTime() > this->LoadTime.GetMTime() ||
      ren->GetRenderWindow() != this->RenderWindow)
  {
    double *spacing = this->GetInput()->GetSpacing();
    double *origin = this->GetInput()->GetOrigin();
    int *inpext = this->GetInput()->GetExtent();
    int *dispext = this->DisplayExtent;

    // find the two used dimensions
    int xdim, ydim;
    // this assumes a 2D image, no lines here folk
    if (dispext[0] != dispext[1])
    {
      xdim = 0;
      if (dispext[2] != dispext[3])
      {
	ydim = 1;
      }
      else
      {
	ydim = 2;
      }
    }
    else
    {
      xdim = 1;
      ydim = 2;
    }

    int xsize, ysize;
    xsize = dispext[xdim*2+1] - dispext[xdim*2] + 1;
    ysize = dispext[ydim*2+1] - dispext[ydim*2] + 1;
  
    // compute the world coordinates
    {
      if (!IDVB) IDVB = fren->GenerateID();
      int dataType[2] = { flowvr::render::Type::Vec3f, flowvr::render::Type::Vec2f };
      flowvr::render::ChunkVertexBuffer* vb = scene->addVertexBuffer(IDVB, 4, 2, dataType);
      vb->gen = Gen;
      struct Vertex { flowvr::render::Vec3f pos; flowvr::render::Vec2f tex; };
      Vertex* data = (Vertex*)vb->data();
      data[0].pos[0] = dispext[0]*spacing[0] + origin[0];
      data[0].pos[1] = dispext[2]*spacing[1] + origin[1];
      data[0].pos[2] = dispext[4]*spacing[2] + origin[2];

      data[1].pos[0] = dispext[1]*spacing[0] + origin[0];
      data[1].pos[1] = dispext[2 + (xdim == 1)]*spacing[1] + origin[1];
      data[1].pos[2] = dispext[4]*spacing[2] + origin[2];

      data[2].pos[0] = dispext[1]*spacing[0] + origin[0];
      data[2].pos[1] = dispext[3]*spacing[1] + origin[1];
      data[2].pos[2] = dispext[5]*spacing[2] + origin[2];

      data[3].pos[0] = dispext[0]*spacing[0] + origin[0];
      data[3].pos[1] = dispext[2 + (ydim == 1)]*spacing[1] + origin[1];
      data[3].pos[2] = dispext[5]*spacing[2] + origin[2];

      data[0].tex[0] = 0.5/xsize;        data[0].tex[1] = 0.5/ysize;
      data[1].tex[0] = 1.0 - 0.5/xsize;  data[1].tex[1] = 0.5/ysize;
      data[2].tex[0] = 1.0 - 0.5/xsize;  data[2].tex[1] = 1.0 - 0.5/ysize;  
      data[3].tex[0] = 0.5/xsize;        data[3].tex[1] = 1.0 - 0.5/ysize;

      flowvr::render::BBox bb;
      for (int i=0;i<4;i++) bb+=data[i].pos;
      vb->setBBox(bb);

      if (!IDIB)
      {
	IDIB = fren->GenerateID();
        scene->addIndexBuffer(IDIB,-1,flowvr::render::Type::Null,flowvr::render::ChunkIndexBuffer::Quad);
      }
    }

    // Compute the texture
    {
      if (!Index) Index = fren->GenerateID();
      int nbc = this->GetInput()->GetNumberOfScalarComponents();
      int imageType = flowvr::render::ChunkTexture::Unknown;
      if (nbc==1) imageType = flowvr::render::ChunkTexture::Grayscale;
      else if (nbc==3) imageType = flowvr::render::ChunkTexture::RGB;
      else if (nbc==4) imageType = flowvr::render::ChunkTexture::RGBA;
      flowvr::render::ChunkTexture* texture = scene->addTexture(Index, imageType, flowvr::render::Type::vector(flowvr::render::Type::Byte, nbc), xsize, ysize);
      texture->level = 0;
      texture->gen = Gen;

      // contiguous would require that xdim = 0 and ydim = 1
      // OR xextent = 1 pixel and xdim = 1 and ydim = 2 
      // OR xdim = 0 and ydim = 2 and yextent = i pixel
      if ((xdim ==0 && ydim == 1)||
	  (inpext[0] == inpext[1] && xdim == 1) ||
	  (inpext[2] == inpext[3] && xdim == 0 && ydim == 2))
      {
	// if contiguous
	memcpy(texture->data(), this->GetInput()->GetScalarPointerForExtent(dispext), texture->dataSize());
      }
      else
      {
	
	// copy the input data to the memory
	vtkIdType inIncX, inIncY, inIncZ;
	int idxZ, idxY, idxR;
	unsigned char *inPtr = (unsigned char *)
	  this->GetInput()->GetScalarPointerForExtent(dispext);
	this->GetInput()->GetContinuousIncrements(dispext, 
					     inIncX, inIncY, inIncZ);
	int rowLength = nbc*xsize;
	unsigned char *outPtr = (unsigned char*)texture->data();
	int outIncY, outIncZ;
	if (ydim == 2)
	{
	  if (xdim == 0)
	  {
	    outIncZ = nbc * 
	      (xsize - (dispext[1] - dispext[0] + 1));
	  }
	  else
	  {
	    outIncZ = nbc * 
	      (xsize - (dispext[3] - dispext[2] + 1));
	  }
	  outIncY = 0;
	}
	else
	{
	  outIncY = nbc * 
	    (xsize - (dispext[1] - dispext[0] + 1));
	  outIncZ = 0;    
	}
      
      
	for (idxZ = dispext[4]; idxZ <= dispext[5]; idxZ++)
	{
	  for (idxY = dispext[2]; idxY <= dispext[3]; idxY++)
	  {
	    for (idxR = 0; idxR < rowLength; idxR++)
	    {
	      // Pixel operation
	      *outPtr = *inPtr;
	      outPtr++;
	      inPtr++;
	    }
	    outPtr += outIncY;
	    inPtr += inIncY;
	  }
	  outPtr += outIncZ;
	  inPtr += inIncZ;
	}
      }
      this->RenderWindow = ren->GetRenderWindow();

      // modify the load time to the current time
      this->LoadTime.Modified();
    }

    if (!IDPrim)
    {
      IDPrim = fren->GenerateID();
      scene->addPrimitive(IDPrim, scene->generateName("VTKImage"));
      flowvr::ID IDVS, IDPS;
      IDVS = fren->GenerateID();
      IDPS = fren->GenerateID();
      scene->loadVertexShader(IDVS, "shaders/video_v.cg");
      scene->loadPixelShader(IDPS, "shaders/video_p.cg");
      scene->addParamID(IDPrim, flowvr::render::ChunkPrimParam::VSHADER,"",IDVS);
      scene->addParamID(IDPrim, flowvr::render::ChunkPrimParam::PSHADER,"",IDPS);
      scene->addParamID(IDPrim, flowvr::render::ChunkPrimParam::VBUFFER_ID,"position",IDVB);
      scene->addParamID(IDPrim, flowvr::render::ChunkPrimParam::VBUFFER_ID,"texcoord0",IDVB);
      scene->addParam(IDPrim, flowvr::render::ChunkPrimParam::VBUFFER_NUMDATA,"texcoord0",1);
      scene->addParamID(IDPrim, flowvr::render::ChunkPrimParam::IBUFFER_ID,"",IDIB);
      scene->addParam(IDPrim, flowvr::render::ChunkPrimParam::IBUFFER_NBI,"",4);
      scene->addParamEnum(IDPrim, flowvr::render::ChunkPrimParam::PARAMVSHADER, "Proj", flowvr::render::ChunkPrimParam::Projection);
      scene->addParamEnum(IDPrim, flowvr::render::ChunkPrimParam::PARAMVSHADER, "ModelViewProj", flowvr::render::ChunkPrimParam::ModelViewProjection);
      scene->addParamID(IDPrim, flowvr::render::ChunkPrimParam::TEXTURE, "texture", Index);
    }

  }
}

void vtkFlowVRImageActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
