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
*---------------------------------------------------------       *
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Thomas Arcila.                                               *
*                                                                 *
*******************************************************************
*                                                                                                          *
* File: .//FlowVRRender/vtkFlowVRVolumeMapper.cxx                                    *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
#include "vtkFlowVRVolumeMapper.h"

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPlaneCollection.h"
#include "vtkTimerLog.h"
#include "vtkVolume.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

#include "vtkFlowVRRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkFlowVRRenderWindow.h"

#include <flowvr/render/primitive.h>
using namespace flowvr::render;
using flowvr::ID;

vtkCxxRevisionMacro(vtkFlowVRVolumeMapper, "$Revision: 5485 $");
vtkStandardNewMacro(vtkFlowVRVolumeMapper);

vtkFlowVRVolumeMapper::vtkFlowVRVolumeMapper()
{
  this->Quality = 0;
  this->Shader = NULL;
  this->Steps = 32;
  this->IdTexture = 0;
  this->IdTTransfer = 0;
  this->IdPrim = 0;
  this->Gen = 0;
}

vtkFlowVRVolumeMapper::~vtkFlowVRVolumeMapper()
{
}

void vtkFlowVRVolumeMapper::Render(vtkRenderer *ren, vtkVolume *vol)
{
//  std::cout << "VolumeRender"<<std::endl;

  if (!ren->IsA("vtkFlowVRRenderer"))
  {
    vtkErrorMacro(<< "Renderer must be a vtkFlowVRRenderer");
    return;
  }

  vtkFlowVRRenderer* fren = static_cast<vtkFlowVRRenderer*>(ren);
  flowvr::render::ChunkWriter* scene = fren->GetSceneWriter();

  // Let the superclass take care of some initialization
  this->vtkVolumeTextureMapper::InitializeRender( ren, vol );

  vtkImageData *input = this->GetInput();

  if (!IdTexture || TimeTexture < this->GetMTime() || TimeTexture < input->GetMTime())
  {
    int                    size[3];
    void                   *inputPointer;
    int                    inputType;
  
    inputPointer = 
      input->GetPointData()->GetScalars()->GetVoidPointer(0);
    inputType = 
      input->GetPointData()->GetScalars()->GetDataType();
    input->GetDimensions( size );

    if (inputType != VTK_UNSIGNED_CHAR)
    {
      vtkErrorMacro(<< "Input data type must be unsigned char");
      return;
    }

    if (!IdTexture)
      IdTexture = fren->GenerateID();

    ChunkTexture* texture = scene->addTexture(IdTexture, ChunkTexture::Grayscale|ChunkTexture::CLAMP, Type::Byte, size[0], size[1], size[2]);
    texture->gen = Gen;
    texture->level = 0;
    memcpy(texture->data(), inputPointer, texture->dataSize());

    // clear borders
    int x,y,z;
    unsigned char* dest = (unsigned char*)texture->data();
    z = 0;
    for (int y=0;y<size[1];y++)
      for (int x=0;x<size[0];x++)
      {
	*dest = 0;
	++dest;
      }
    dest = (unsigned char*)texture->data();
    z = size[2]-1; dest+=(size[2]-1)*size[1]*size[0];
    for (int y=0;y<size[1];y++)
      for (int x=0;x<size[0];x++)
      {
	*dest = 0;
	++dest;
      }
    dest = (unsigned char*)texture->data();
    y = 0;
    for (int z=0;z<size[2];z++,dest+=(size[1]-1)*size[0])
      for (int x=0;x<size[0];x++)
      {
	*dest = 0;
	++dest;
      }
    dest = (unsigned char*)texture->data();
    y = size[1]-1; dest+=(size[1]-1)*size[0];
    for (int z=0;z<size[2];z++,dest+=(size[1]-1)*size[0])
      for (int x=0;x<size[0];x++)
      {
	*dest = 0;
	++dest;
      }
    dest = (unsigned char*)texture->data();
    x = 0;
    for (int z=0;z<size[2];z++)
      for (int y=0;y<size[1];y++)
      {
	*dest = 0;
	dest+=size[0];
      }
    dest = (unsigned char*)texture->data();
    x = size[0]-1; dest+=size[0]-1;
    for (int z=0;z<size[2];z++)
      for (int y=0;y<size[1];y++)
      {
	*dest = 0;
	dest+=size[0];
      }
    TimeTexture.Modified();
  }

  if (!IdTTransfer || TimeTTransfer < this->GetMTime() || TimeTTransfer < vol->GetMTime())
  {
    int                    inputType;
    inputType = 
      input->GetPointData()->GetScalars()->GetDataType();

    if (inputType != VTK_UNSIGNED_CHAR)
    {
      vtkErrorMacro(<< "Input data type must be unsigned char");
      return;
    }

    if (!IdTTransfer)
      IdTTransfer = fren->GenerateID();

    if (Quality==1)
    { // Preintegrated transfer texture
//      std::cout << "PreIntegrate(Transfer)"<<std::endl;
      ChunkTexture* texture = scene->addTexture(IdTTransfer, ChunkTexture::RGBA|ChunkTexture::CLAMP, Type::Vec4b, 256, 256);
      texture->gen = Gen;
      texture->level = 0;
      const Vec4b* src = (const Vec4b*)GetRGBAArray();
      Vec4b* dest = (Vec4b*) texture->data();
      float f,r,g,b,a;
      for (int i = 0; i < 256; i++)
      {
	a = src[i][3];
	r = src[i][0]*a;
	g = src[i][1]*a;
	b = src[i][2]*a;
	for (int j = 0; j < 256; j++)
	{
	  dest[j][0] = (unsigned char)(r / 255);
	  dest[j][1] = (unsigned char)(g / 255);
	  dest[j][2] = (unsigned char)(b / 255);
	  dest[j][3] = (unsigned char)(255-a);
	}

//#define PREINT_MEAN
#define PREINT_BLEND
//#define PREINT_MAX

#ifdef PREINT_MEAN
	a = src[i][3];
	r = src[i][0]*a;
	g = src[i][1]*a;
	b = src[i][2]*a;
	for (int j = i-1; j >= 0 ; j--)
	{
	  f = src[j][3];
	  r += src[j][0]*f;
	  g += src[j][1]*f;
	  b += src[j][2]*f;
	  a += f;
	  if (a<=0.0f)
	  {
	    dest[j]  = Vec4b(0,0,0,255);
	  }
	  else
	  {
	    dest[j][0] = (unsigned char)( r / (255*(i-j+1)));
	    dest[j][1] = (unsigned char)( g / (255*(i-j+1)));
	    dest[j][2] = (unsigned char)( b / (255*(i-j+1)));
	    dest[j][3] = (unsigned char)( 255 - a / (i-j+1));
	  }
	}
	a = src[i][3];
	r = src[i][0]*a;
	g = src[i][1]*a;
	b = src[i][2]*a;
	for (int j = i+1; j < 256 ; j++)
	{
	  f = src[j][3];
	  r += src[j][0]*f;
	  g += src[j][1]*f;
	  b += src[j][2]*f;
	  a += f;
	  if (a<=0.0f)
	  {
	    dest[j]  = Vec4b(0,0,0,255);
	  }
	  else
	  {
	    dest[j][0] = (unsigned char)( r / (255*(j-i+1)));
	    dest[j][1] = (unsigned char)( g / (255*(j-i+1)));
	    dest[j][2] = (unsigned char)( b / (255*(j-i+1)));
	    dest[j][3] = (unsigned char)( 255 - a / (j-i+1));
	  }
	}
#elif defined(PREINT_BLEND)
	for (int j = 0; j < 256 ; j++)
	{
	  if (i==j) continue;
	  float p = 1.0f/(abs(i-j));
	  int add = i<j ? 1 : -1;
	  r = 0;
	  g = 0;
	  b = 0;
	  a = 0;
	  for (int k = i; k != j ; k+=add)
	  {
	    f = src[k][3] / 255.0f;
	    f = 1-powf(1-f,p); // correct for smaller size step
	    r = r*(1-f) + src[k][0]*f;
	    g = g*(1-f) + src[k][1]*f;
	    b = b*(1-f) + src[k][2]*f;
	    a = a*(1-f) + f;
	  }
	  if (a<=0.0f)
	  {
	    dest[j]  = Vec4b(0,0,0,255);
//	    dest[j]  = Vec4b(0,0,0,0);
	  }
	  else
	  {
//	    f = powf(a,1.0f/(i-j+1));
	    dest[j][0] = (unsigned char)(r);
	    dest[j][1] = (unsigned char)(g);
	    dest[j][2] = (unsigned char)(b);
	    dest[j][3] = (unsigned char)(255.0f*(1-a));
//	    dest[j][0] = (unsigned char)(r/a);
//	    dest[j][1] = (unsigned char)(g/a);
//	    dest[j][2] = (unsigned char)(b/a);
//	    dest[j][3] = (unsigned char)(255.0f*a);
	  }
	}
#elif defined(PREINT_MAX)
#endif
	dest += 256;
      }
//    std::cout << "Saving PreIntegrate(Transfer)"<<std::endl;
//    if (!scene->saveTexture(texture, "log-TTransfer.png"))
//	std::cout << "FAILED :("<<std::endl;
    }
    else
    {
      ChunkTexture* texture = scene->addTexture(IdTTransfer, ChunkTexture::RGBA|ChunkTexture::CLAMP, Type::Vec4b, 256);
      texture->gen = Gen;
      texture->level = 0;
      //memcpy(texture->data(), GetRGBAArray(), texture->dataSize());
      const Vec4b* src = (const Vec4b*)GetRGBAArray();
      Vec4b* dest = (Vec4b*) texture->data();
      for (int i = 0; i < 256; i++)
      {
	dest[i][0] = (unsigned char)(src[i][0]*src[i][3]/255);
	dest[i][1] = (unsigned char)(src[i][1]*src[i][3]/255);
	dest[i][2] = (unsigned char)(src[i][2]*src[i][3]/255);
	dest[i][3] = (unsigned char)(255-src[i][3]);
      }
    }
    TimeTTransfer.Modified();
  }

  if (!IdPrim)
  {
    IdPrim = fren->GenerateID();
    flowvr::ID IdVB = fren->GenerateID();
    flowvr::ID IdIB = fren->GenerateID();
    flowvr::ID IdVS = fren->GenerateID();
    flowvr::ID IdPS = fren->GenerateID();

    int                    size[3];
    input->GetDimensions( size );

    // Create vertex+index buffers
    {
      int dataType[1] = { Type::Vec3f };
      ChunkVertexBuffer* vb = scene->addVertexBuffer(IdVB, 4, 1, dataType, BBox(Vec3f(0,0,0),Vec3f(1,1,1)));
      Vec3f* data = (Vec3f*) vb->data();
      float z = -1;
      data[0] = Vec3f(-2,-2,z);
      data[1] = Vec3f( 2,-2,z);
      data[2] = Vec3f( 2, 2,z);
      data[3] = Vec3f(-2, 2,z);
      scene->addIndexBuffer(IdIB, -1, Type::Null, ChunkIndexBuffer::Quad);
    }

    // Load custom shaders
    scene->loadVertexShader(IdVS, "shaders/volume_v.cg");
    std::string shader((this->Shader ? this->Shader : ""));
    if (shader.empty())
    {
      switch (Quality)
      {
      case 1:
    	  shader = "shaders/volume_vtk_preint_p.cg";
    	  break;
      default:
    	  shader = "shaders/volume_vtk_p.cg";
    	  break;
      }
    }
    scene->loadPixelShader(IdPS, shader);
    
    // Create new primitive
    scene->addPrimitive(IdPrim,scene->generateName("VTKVolume"));

    // Set shaders
    scene->addParamID(IdPrim, ChunkPrimParam::VSHADER,"",IdVS);
    scene->addParamID(IdPrim, ChunkPrimParam::PSHADER,"",IdPS);

    // Set vertex buffers
    scene->addParamID(IdPrim, ChunkPrimParam::VBUFFER_ID, "position", IdVB);

    // Set index buffer
    scene->addParamID(IdPrim, ChunkPrimParam::IBUFFER_ID, "", IdIB);
    scene->addParam(IdPrim, ChunkPrimParam::IBUFFER_NBI, "", 4);

    float fsize = float(size[0]+size[1]+size[2])/3;

    // Set parameters
    scene->addParamEnum(IdPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewProjI", ChunkPrimParam::ModelViewProjection_Inv);
    scene->addParamEnum(IdPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewI", ChunkPrimParam::ModelView_Inv);
    scene->addParamEnum(IdPrim, ChunkPrimParam::PARAMVSHADER, "Proj", ChunkPrimParam::Projection);
    scene->addParam(IdPrim, ChunkPrimParam::PARAMPSHADER, "steps", this->Steps/2);
    scene->addParam(IdPrim, ChunkPrimParam::PARAMPSHADER, "inv_steps", sqrtf(3.0)/this->Steps);
//  scene->addParam(IdPrim, ChunkPrimParam::PARAMPSHADER, "opacity", fsize/this->Steps);
    scene->addParamID(IdPrim, ChunkPrimParam::TEXTURE, "texture", IdTexture);
    scene->addParamID(IdPrim, ChunkPrimParam::TEXTURE, "ttransfer", IdTTransfer);

    scene->addParam(IdPrim, ChunkPrimParam::PARAMOPENGL, "DepthWrite", false);
    scene->addParam(IdPrim, ChunkPrimParam::PARAMOPENGL, "DepthTest", false);
    scene->addParam(IdPrim, ChunkPrimParam::PARAMOPENGL, "Blend", true);
    scene->addParamEnum(IdPrim, ChunkPrimParam::PARAMOPENGL, "BlendMode", BLEND_PREMULT);
    scene->addParam(IdPrim, ChunkPrimParam::ORDER, "", 10);

  }

  ++this->Gen;

}

// Print the vtkFlowVRVolumeMapper
void vtkFlowVRVolumeMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

