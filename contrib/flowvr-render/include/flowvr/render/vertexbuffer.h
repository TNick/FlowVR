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
* File: ./include/flowvr/render/vertexbuffer.h                    *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_VERTEXBUFFER_H
#define FLOWVR_RENDER_VERTEXBUFFER_H

#include "resource.h"

namespace flowvr
{

namespace render
{

template<class UserData>
class PrimitiveTemplate;

template<class UserData, class User = PrimitiveTemplate<UserData> >
class VertexBufferTemplate : public Resource3D<RES_VERTEXBUFFER,typename UserData::VertexBuffer,ChunkVertexBuffer, User>
{
 public:
  int vertexSize;
  std::vector<int> dataType;

  VertexBufferTemplate() : vertexSize(0) {}

  bool add(const MsgChunk<ChunkVertexBuffer>& chunk)
  {
    if (!Resource3D<RES_VERTEXBUFFER,typename UserData::VertexBuffer,ChunkVertexBuffer,User>::add(chunk))
      return false;
    vertexSize = chunk->vertexSize;
    dataType.resize(chunk->nbData);
    for (unsigned int i=0;i<dataType.size();i++)
      dataType[i] = chunk->dataType[i];
    return true;
  }

  bool add(const MsgChunk<ChunkVertexBufferUpdate>& chunk)
  {
    vertexSize = chunk->vertexSize;
    dataType.resize(chunk->nbData);
    for (unsigned int i=0;i<dataType.size();i++)
      dataType[i] = chunk->dataType[i];
    return true;
  }

  int nbVertex() const
  {
    if (vertexSize==0 || this->empty()) return 0;
    return this->firstLevel()->dataSize()/vertexSize;
  }

};

} // namespace render

} // namespace flowvr

#endif
