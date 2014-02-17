/******* COPYRIGHT ************************************************
*                                                                 *
*                         FlowVR Render                           *
*                   Parallel Rendering Library                    *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
*  ALL RIGHTS RESERVED.                                           *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING-LIB file for further information.                       *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Clement Menier.                                              *
*                                                                 *
*******************************************************************
*                                                                 *
* File: ./src/librender/chunk.cpp                                 *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include <flowvr/render/chunk.h>
#include <flowvr/render/bbox.h>

#include <ftl/type.h>
#include <ftl/chunk.h>

#include <string>
#include <iostream>

namespace flowvr
{

namespace render
{

void ChunkResource3D::getBBox(BBox& bb) const
{
  bb.a[0] = bbox[0][0];
  bb.a[1] = bbox[0][1];
  bb.a[2] = bbox[0][2];
  bb.b[0] = bbox[1][0];
  bb.b[1] = bbox[1][1];
  bb.b[2] = bbox[1][2];
}

void ChunkResource3D::setBBox(const BBox& bb)
{
  bbox[0][0] = bb.a[0];
  bbox[0][1] = bb.a[1];
  bbox[0][2] = bb.a[2];
  bbox[1][0] = bb.b[0];
  bbox[1][1] = bb.b[1];
  bbox[1][2] = bb.b[2];
}

void ChunkRes3DUpdate::getBBox(BBox& bb) const
{
  bb.a[0] = bbox[0][0];
  bb.a[1] = bbox[0][1];
  bb.a[2] = bbox[0][2];
  bb.b[0] = bbox[1][0];
  bb.b[1] = bbox[1][1];
  bb.b[2] = bbox[1][2];
}

void ChunkRes3DUpdate::setBBox(const BBox& bb)
{
  bbox[0][0] = bb.a[0];
  bbox[0][1] = bb.a[1];
  bbox[0][2] = bb.a[2];
  bbox[1][0] = bb.b[0];
  bbox[1][1] = bb.b[1];
  bbox[1][2] = bb.b[2];
}

#define PENUM(val) case val: std::cout << #val; break

#define PENUM2(t,val) case t::val: std::cout << #val; break

/// Print chunk info to stdout
void ChunkRender::print() const
{
  std::cout << "Chunk @ "  << this << ":\t"
	    << "  size = " << size << "\t"
	    << "  type = ";
  switch (type)
  {
  case PRIM_ADD: std::cout << "PRIM_ADD\n"; ((const ChunkPrimAdd*)this)->print(); break;
  case PRIM_DEL: std::cout << "PRIM_DEL\n"; ((const ChunkPrimDel*)this)->print(); break;
  case PRIM_PARAM: std::cout << "PRIM_PARAM\n"; ((const ChunkPrimParam*)this)->print(); break;
  case RES_ADD: std::cout << "RES_ADD\n"; ((const ChunkResource*)this)->print(); break;
  case RES_DEL: std::cout << "RES_DEL\n"; ((const ChunkResDel*)this)->print(); break;
  case RES_UPDATE: std::cout << "RES_UPDATE\n"; ((const ChunkResUpdate*)this)->print(); break;
  default: std::cout << type << ": Not a Render Chunk!\n";
  }
}

void ChunkPrimAdd::print() const
{
  std::cout << "  id = 0x"<<std::hex<<id<<std::dec<<"\n"
	    << "  from = 0x"<<std::hex<<from<<std::dec<<"\n";
}

void ChunkPrimDel::print() const
{
  std::cout << "  id = 0x"<<std::hex<<id<<std::dec<<"\n";
}

void ChunkPrimParam::print() const
{
  std::cout << "  id = 0x"<<std::hex<<id<<std::dec<<"\n"
	    << "  param = ";
  switch (param)
  {
    PENUM(VISIBLE);
    PENUM(ORDER);
    PENUM(VSHADER);
    PENUM(PSHADER);
    PENUM(VBUFFER_ID);
    PENUM(VBUFFER_NUMDATA);
    PENUM(VBUFFER_V0);
    PENUM(NBV);
    PENUM(IBUFFER_ID);
    PENUM(IBUFFER_I0);
    PENUM(IBUFFER_NBI);
    PENUM(TEXTURE);
    PENUM(PARAMVSHADER);
    PENUM(PARAMPSHADER);
    PENUM(PARAMOPENGL);
    PENUM(TRANSFORM);
    PENUM(TRANSFORM_POSITION);
    PENUM(TRANSFORM_ROTATION);
    PENUM(TRANSFORM_SCALE);
    PENUM(PROJECTION);
    PENUM(NAME);
    PENUM(PARAMDISPLAY);
    PENUM(TRANSFORM_OVERRIDE);
  default: std::cout << param;
  }
  std::cout << "\n"
	    << "  dataType = "<<ftl::Type::name(dataType)<<"\n"
	    << "  name = "<<name<<"\n";
  std::string val;
  ftl::Type::assign(val, dataType, data());
  std::cout << "  value = "<<val<<"\n";
}

void ChunkResource::print() const
{
  std::cout << "  id = 0x"<<std::hex<<id<<std::dec<<"\n"
	    << "  resType = ";
  switch (resType)
  {
    PENUM(RES_TEXTURE);
    PENUM(RES_VERTEXBUFFER);
    PENUM(RES_INDEXBUFFER);
    PENUM(RES_VERTEXSHADER);
    PENUM(RES_PIXELSHADER);
  default: std::cout << resType;
  }
  std::cout << "\n"
	    << "  level = "<<level<<"\n"
	    << "  gen = "<<gen<<"\n";
  switch (resType)
  {
  case RES_TEXTURE: ((const ChunkTexture*)this)->print(); break;
  case RES_VERTEXBUFFER: ((const ChunkVertexBuffer*)this)->print(); break;
  case RES_INDEXBUFFER: ((const ChunkIndexBuffer*)this)->print(); break;
  case RES_VERTEXSHADER: ((const ChunkVertexShader*)this)->print(); break;
  case RES_PIXELSHADER: ((const ChunkPixelShader*)this)->print(); break;
  }
}

void ChunkResource3D::print() const
{
  std::cout << "  bbox = {{"<<bbox[0][0]<<", "<<bbox[0][1]<<", "<<bbox[0][2]<<"}, "
	    << "{"<<bbox[1][0]<<", "<<bbox[1][1]<<", "<<bbox[1][2]<<"}}\n";
}

void ChunkTexture::print() const
{
  std::cout << "  imageType = ";
  if (imageType&~MASK_TYPE)
  {
    switch(imageType&~MASK_TYPE)
    {
      PENUM(CUBEMAP);
    default: std::cout << "0x"<<std::hex<<(imageType&~MASK_TYPE);
    }
    std::cout << "|";
  }
  switch (imageType&MASK_TYPE)
  {
    PENUM(Unknown);
    PENUM(BW);
    PENUM(Grayscale);
    PENUM(RGB);
    PENUM(RGB32F_ARB);
    PENUM(BGR);
    PENUM(YUV);
    PENUM(YUYV);
    PENUM(Depth);
    PENUM(RGB_DXT1);
    PENUM(RGBA_DXT1);
    PENUM(RGBA_DXT3);
    PENUM(RGBA_DXT5);
    PENUM(YCOCG_DXT5);
  default: std::cout<<(imageType&MASK_TYPE);
  }
  int b = 1 << ((imageType&MASK_BLOC_LOG2) >> SHIFT_BLOC_LOG2);
  if (imageType&MASK_BLOC_LOG2) std::cout << " ("<<b<<"x"<<b<<" blocs)";
  std::cout << "\n"
	    << "  nx = "<<nx<<"\n"
	    << "  ny = "<<ny<<"\n"
	    << "  nz = "<<nz<<"\n"
	    << "  pixelType = "<<ftl::Type::name(pixelType)<<"\n"
	    << "  lineSize = "<<lineSize<<"\n";
  int textureSize = nx*lineSize*(ny==0?1:ny)*(nz==0?1:nz);
  std::cout << "  textureSize: "<<textureSize<<"\n"
	    << "  dataSize: "<<dataSize()<<"\n";
}

void ChunkVertexBuffer::print() const
{
  this->ChunkResource3D::print();
  std::cout << "  nbData = "<<nbData<<"\n"
	    << "  vertexSize = "<<vertexSize<<"\n"
	    << "  dataType = {";
  for (int i=0;i<nbData;i++)
  {
    if (i>0) std::cout << ", ";
    std::cout << ftl::Type::name(dataType[i])<<std::dec;
  }
  std::cout << "}\n"
	    << "  nbv: "<<dataSize()/vertexSize<<"\n"
	    << "  dataSize: "<<dataSize()<<"\n";
}

void ChunkIndexBuffer::print() const
{
  std::cout << "  dataType = "<<ftl::Type::name(dataType)<<"\n"
	    << "  primitive = ";
  switch (primitive)
  {
    PENUM(Point);
    PENUM(Line);
    PENUM(LineLoop);
    PENUM(LineStrip);
    PENUM(Triangle);
    PENUM(TriangleStrip);
    PENUM(TriangleFan);
    PENUM(Quad);
    PENUM(QuadStrip);
    PENUM(Polygon);
    PENUM(Tetrahedron);
  default: std::cout << primitive;
  }
  std::cout << "\n"
	    << "  restart = 0x"<<std::hex<<restart<<std::dec<<"\n"
	    << "  nbi: "<<(ftl::Type::size(dataType)==0?-1:dataSize()/ftl::Type::size(dataType))<<"\n"
	    << "  dataSize: "<<dataSize()<<"\n";
}

void ChunkShader::print() const
{
  std::cout << "  language = ";
  switch (language)
  {
    PENUM(SHADER_ALL);
    PENUM(SHADER_HLSL);
    PENUM(SHADER_CG);
    PENUM(SHADER_GLSL);
    PENUM(SHADER_ARB_PROGRAM);
  default: std::cout << language;
  }
  std::cout << "\n";
  std::string source(data(), data()+dataSize());
  std::cout << "  data=\"\n"
	    << source << "\n\"\n";
}

void ChunkResUpdate::print() const
{
  std::cout << "  id = 0x"<<std::hex<<id<<std::dec<<"\n"
	    << "  resType = ";
  switch (resType)
  {
    PENUM(RES_TEXTURE);
    PENUM(RES_VERTEXBUFFER);
    PENUM(RES_INDEXBUFFER);
    PENUM(RES_VERTEXSHADER);
    PENUM(RES_PIXELSHADER);
  default: std::cout << resType;
  }
  std::cout << "\n"
	    << "  level = "<<level<<"\n"
	    << "  gen = "<<gen<<"\n";
  switch (resType)
  {
  case RES_TEXTURE: ((const ChunkTextureUpdate*)this)->print(); break;
  case RES_VERTEXBUFFER: ((const ChunkVertexBufferUpdate*)this)->print(); break;
  case RES_INDEXBUFFER: ((const ChunkIndexBufferUpdate*)this)->print(); break;
  }
}

void ChunkRes3DUpdate::print() const
{
  std::cout << "  bbox = {{"<<bbox[0][0]<<", "<<bbox[0][1]<<", "<<bbox[0][2]<<"}, "
	    << "{"<<bbox[1][0]<<", "<<bbox[1][1]<<", "<<bbox[1][2]<<"}}\n";
}

void ChunkTextureUpdate::print() const
{
  std::cout << "  imageType = ";
  if (imageType&~MASK_TYPE)
  {
    int m = (imageType&~MASK_TYPE);
#define EMASK(M) if (m&M) { std::cout << #M << "|"; m &= ~M; } else
    EMASK(CUBEMAP);
    EMASK(CLAMP);
    EMASK(NEAREST);
    EMASK(RECT);
    EMASK(ARRAY);
#undef EMASK
    if (m) std::cout << "0x"<<std::hex<<m<<std::dec<<"|";
  }
  switch (imageType&MASK_TYPE)
  {
    PENUM(Unknown);
    PENUM(BW);
    PENUM(Grayscale);
    PENUM(RGB);
    PENUM(RGB32F_ARB);
    PENUM(BGR);
    PENUM(YUV);
    PENUM(YUYV);
    PENUM(Depth);
    PENUM(RGB_DXT1);
    PENUM(RGBA_DXT1);
    PENUM(RGBA_DXT3);
    PENUM(RGBA_DXT5);
    PENUM(YCOCG_DXT5);
  default: std::cout<<(imageType&MASK_TYPE);
  }
  int b = 1 << ((imageType&MASK_BLOC_LOG2) >> SHIFT_BLOC_LOG2);
  if (b) std::cout << " ("<<b<<"x"<<b<<" blocs)";
  std::cout << "\n"
	    << "  nx = "<<nx<<"\n"
	    << "  ny = "<<ny<<"\n"
	    << "  nz = "<<nz<<"\n"
	    << "  pixelType = "<<ftl::Type::name(pixelType)<<"\n"
	    << "  lineSize = "<<lineSize<<"\n"
	    << "  ux0 uy0 uz0 = "<<ux0<<" "<<uy0<<" "<<uz0<<"\n"
	    << "  unx uny unz = "<<unx<<" "<<uny<<" "<<unz<<"\n";
  int textureSize = unx*lineSize*(uny==0?1:uny)*(unz==0?1:unz);
  std::cout << "  textureSize: "<<textureSize<<"\n"
	    << "  dataSize: "<<dataSize()<<"\n";
}

void ChunkVertexBufferUpdate::print() const
{
  this->ChunkRes3DUpdate::print();
  std::cout << "  uoffset = "<<uoffset<<"\n"
	    << "  nbData = "<<nbData<<"\n"
	    << "  vertexSize = "<<vertexSize<<"\n"
	    << "  dataType = {";
  for (int i=0;i<nbData;i++)
  {
    if (i>0) std::cout << ", ";
    std::cout << ftl::Type::name(dataType[i])<<std::dec;
  }
  std::cout << "}\n"
	    << "  nbv: "<<dataSize()/vertexSize<<"\n"
	    << "  dataSize: "<<dataSize()<<"\n";
}

void ChunkIndexBufferUpdate::print() const
{
  std::cout << "  uoffset = "<<uoffset<<"\n"
	    << "  dataType = "<<ftl::Type::name(dataType)<<"\n"
	    << "  primitive = ";
  switch (primitive)
  {
    PENUM2(ChunkIndexBuffer,Point);
    PENUM2(ChunkIndexBuffer,Line);
    PENUM2(ChunkIndexBuffer,LineLoop);
    PENUM2(ChunkIndexBuffer,LineStrip);
    PENUM2(ChunkIndexBuffer,Triangle);
    PENUM2(ChunkIndexBuffer,TriangleStrip);
    PENUM2(ChunkIndexBuffer,TriangleFan);
    PENUM2(ChunkIndexBuffer,Quad);
    PENUM2(ChunkIndexBuffer,QuadStrip);
    PENUM2(ChunkIndexBuffer,Polygon);
    PENUM2(ChunkIndexBuffer,Tetrahedron);
  default: std::cout << primitive;
  }
  std::cout << "\n"
	    << "  restart = 0x"<<std::hex<<restart<<std::dec<<"\n"
	    << "  nbi: "<<(ftl::Type::size(dataType)==0?-1:dataSize()/ftl::Type::size(dataType))<<"\n"
	    << "  dataSize: "<<dataSize()<<"\n";
}

void ChunkResDel::print() const
{
  std::cout << "  id = 0x"<<std::hex<<id<<std::dec<<"\n"
	    << "  resType = ";
  switch (resType)
  {
    PENUM(RES_TEXTURE);
    PENUM(RES_VERTEXBUFFER);
    PENUM(RES_INDEXBUFFER);
    PENUM(RES_VERTEXSHADER);
    PENUM(RES_PIXELSHADER);
  default: std::cout << resType;
  }
  std::cout << "\n"
	    << "  level = "<<level<<"\n"
	    << "  gen = "<<gen<<"\n";
}

} // namespace render

} // namespace flowvr
