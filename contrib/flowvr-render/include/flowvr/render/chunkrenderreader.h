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
* File: ./include/flowvr/render/chunkrenderreader.h               *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_CHUNKREADER_H
#define FLOWVR_RENDER_CHUNKREADER_H

#include "primitivelist.h"
#include "paramval.h"

#include "resourcelist.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "vertexshader.h"
#include "pixelshader.h"
#include "texture.h"

#include <flowvr/message.h>

#include <ftl/chunk.h>
#include <ftl/chunkreader.h>

namespace flowvr
{

namespace render
{

//======================================================================

/// Note: A class deriving from ChunkReader must use a UserData class
/// with the following subclasses/typedef :
/// * VertexBuffer,PixelBuffer,VertexShader,PixelShader,Texture : data associated with each resource
/// * Primitive : data associated with each primitive
/// * PrimitiveVertexBuffer : data associated with each primitive referenced vertex buffer
/// * PrimitiveIndexBuffer : data associated with each primitive referenced vertex buffer
/// * PrimitiveTexture : data associated with each primitive referenced texture
/// * ParamVShader : data associated with each primitive vertex shader parameter
/// * ParamPShader : data associated with each primitive pixel shader parameter
/// * ParamOpenGL : data associated with each primitive opengl parameter

class ChunkRenderReaderDefaultUserData
{
public:
  class VertexBuffer           { };
  class IndexBuffer            { };
  class VertexShader           { };
  class PixelShader            { };
  class Texture                { };
  class Primitive                { };
  class   PrimitiveVertexBuffer  { };
  class   PrimitiveIndexBuffer   { };
  class   PrimitiveTexture       { };
  class   ParamVShader           { };
  class   ParamPShader           { };
  class   ParamOpenGL            { };
  class   ParamDisplay           { };
};



template <class UserData = ChunkRenderReaderDefaultUserData >
class ChunkRenderReader
{
public:

  typedef PrimitiveTemplate<UserData> Primitive;
  typedef VertexShaderTemplate<UserData> VertexShader;
  typedef PixelShaderTemplate<UserData> PixelShader;
  typedef TextureTemplate<UserData> Texture;
  typedef VertexBufferTemplate<UserData> VertexBuffer;
  typedef IndexBufferTemplate<UserData> IndexBuffer;

  typedef PrimitiveListTemplate<UserData> PrimitiveList;
  typedef ResourceList<VertexShader> VertexShaderList;
  typedef ResourceList<PixelShader> PixelShaderList;
  typedef ResourceList<Texture> TextureList;
  typedef ResourceList<VertexBuffer> VertexBufferList;
  typedef ResourceList<IndexBuffer> IndexBufferList;

  PrimitiveList primitives;
  VertexShaderList vShaders;
  PixelShaderList pShaders;
  TextureList textures;
  VertexBufferList vBuffers;
  IndexBufferList iBuffers;
  BBox sceneBBox; ///< Global scene bounding-box

  int iteration;

  ChunkRenderReader()
  : iteration(-1)
  {
  }

  // Main info is primitives
  typedef typename PrimitiveList::iterator iterator;
  typedef typename PrimitiveList::const_iterator const_iterator;

  iterator begin() { return primitives.begin(); }
  iterator end() { return primitives.end(); }
  const_iterator begin() const { return primitives.begin(); }
  const_iterator end() const { return primitives.end(); }

  virtual ~ChunkRenderReader() {}

  /// global init
  /// @return false if error
  virtual bool init();

  virtual bool process(flowvr::Message msg, int it=-1);

  /// update scene bounding-box (clear it first if necessary)
  virtual void updateBBox();

protected:

  virtual bool processChunk(const MsgChunk<Chunk>& data);
  virtual bool processChunk(const MsgChunk<ChunkPrimAdd>& data);
  virtual bool processChunk(const MsgChunk<ChunkPrimDel>& data);
  virtual bool processChunk(const MsgChunk<ChunkPrimParam>& data);
  virtual bool processChunk(const MsgChunk<ChunkResource>& data);
  virtual bool processChunk(const MsgChunk<ChunkTexture>& data);
  virtual bool processChunk(const MsgChunk<ChunkVertexBuffer>& data);
  virtual bool processChunk(const MsgChunk<ChunkIndexBuffer>& data);
  virtual bool processChunk(const MsgChunk<ChunkVertexShader>& data);
  virtual bool processChunk(const MsgChunk<ChunkPixelShader>& data);
  virtual bool processChunk(const MsgChunk<ChunkResUpdate>& data);
  virtual bool processChunk(const MsgChunk<ChunkTextureUpdate>& data);
  virtual bool processChunk(const MsgChunk<ChunkVertexBufferUpdate>& data);
  virtual bool processChunk(const MsgChunk<ChunkIndexBufferUpdate>& data);
  virtual bool processChunk(const MsgChunk<ChunkResDel>& data);
  virtual bool setParam(Primitive* prim, int param, const char* name, const TypedArray& val);
  virtual bool resetParam(Primitive* prim, int param, const char* name);

  /// @name User-defined callbacks
  /// @{
/*
  virtual void cameraMoved()
  {
    invcamera.invert(camera->xform.mat);
  }
*/
  virtual bool storeResource(const ChunkResource* data)
  { return true; }

  virtual bool storeTexture(Texture* texture, const ChunkTexture* data)
  { return storeResource(data); }

  virtual bool storeVertexBuffer(VertexBuffer* vbuffer, const ChunkVertexBuffer* data)
  { return storeResource(data); }

  virtual bool storeIndexBuffer(IndexBuffer* ibuffer, const ChunkIndexBuffer* data)
  { return storeResource(data); }

  virtual bool storeVertexShader(VertexShader* vshader, const ChunkVertexShader* data)
  { return storeResource(data); }

  virtual bool storePixelShader(PixelShader* pshader, const ChunkPixelShader* data)
  { return storeResource(data); }

  virtual bool supportPixelShader(ShaderLanguage language)
  { return true; }

  virtual bool supportVertexShader(ShaderLanguage language)
  { return true; }

  virtual bool addPrimitive(Primitive* prim, Primitive* model)
  { return true; }

  virtual bool delPrimitive(Primitive* prim)
  { return true; }

  virtual bool setVShader(Primitive* prim, VertexShader* shader)
  { return true; }

  virtual bool setPShader(Primitive* prim, PixelShader* shader)
  { return true; }

  virtual bool setVBuffer(Primitive* prim, const char* name, typename Primitive::VBData* dest, VertexBuffer*buffer, bool create)
  { return true; }

  virtual bool setTexture(Primitive* prim, const char* name, typename Primitive::TData* dest, Texture* texture, bool create)
  { return true; }

  virtual bool setParamVShader(Primitive* prim, const char* name, typename Primitive::ParamVShader* dest, const TypedArray& val, bool create)
  { return true; }

  virtual bool setParamPShader(Primitive* prim, const char* name, typename Primitive::ParamPShader* dest, const TypedArray& val, bool create)
  { return true; }

  virtual bool setParamOpenGL(Primitive* prim, const char* name, typename Primitive::ParamOpenGL* dest, const TypedArray& val, bool create)
  { return true; }

  virtual bool setParamDisplay(Primitive* prim, const char* name, typename Primitive::ParamDisplay* dest, const TypedArray& val, bool create)
  { return true; }

  virtual bool resetVBuffer(Primitive* prim, const char* name, typename Primitive::VBData* dest)
  { return true; }

  virtual bool resetTexture(Primitive* prim, const char* name, typename Primitive::TData* dest)
  { return true; }

  virtual bool resetParamVShader(Primitive* prim, const char* name, typename Primitive::ParamVShader* dest)
  { return true; }

  virtual bool resetParamPShader(Primitive* prim, const char* name, typename Primitive::ParamPShader* dest)
  { return true; }

  virtual bool resetParamOpenGL(Primitive* prim, const char* name, typename Primitive::ParamOpenGL* dest)
  { return true; }

  virtual bool resetParamDisplay(Primitive* prim, const char* name, typename Primitive::ParamDisplay* dest)
  { return true; }

  virtual bool addTexture(Texture* texture, const ChunkTexture* data, bool create)
  { return true; }

  virtual bool addVertexBuffer(VertexBuffer* vbuffer, const ChunkVertexBuffer* data, bool create)
  { return true; }

  virtual bool addIndexBuffer(IndexBuffer* ibuffer, const ChunkIndexBuffer* data, bool create)
  { return true; }

  virtual bool addVertexShader(VertexShader* vshader, const ChunkVertexShader* data, bool create)
  { return true; }

  virtual bool addPixelShader(PixelShader* pshader, const ChunkPixelShader* data, bool create)
  { return true; }

  virtual bool updateTexture(Texture* texture, const ChunkTextureUpdate* data)
  { return true; }

  virtual bool updateVertexBuffer(VertexBuffer* vbuffer, const ChunkVertexBufferUpdate* data)
  { return true; }

  virtual bool updateIndexBuffer(IndexBuffer* ibuffer, const ChunkIndexBufferUpdate* data)
  { return true; }

  virtual bool delTexture(Texture* texture)
  { return true; }

  virtual bool delVertexBuffer(VertexBuffer* vbuffer)
  { return true; }

  virtual bool delIndexBuffer(IndexBuffer* ibuffer)
  { return true; }

  virtual bool delVertexShader(VertexShader* vshader)
  { return true; }

  virtual bool delPixelShader(PixelShader* pshader)
  { return true; }

  virtual bool updateTargetGroups( Primitive *p )
  {
	  return true;
  }

  /// @}

};

} // namespace render

} // namespace flowvr

#endif
