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
* File: ./include/flowvr/render/indexbuffer.h                     *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_INDEXBUFFER_H
#define FLOWVR_RENDER_INDEXBUFFER_H

#include "resource.h"

namespace flowvr
{

namespace render
{

template<class UserData>
class PrimitiveTemplate;

template<class UserData, class User = PrimitiveTemplate<UserData> >
class IndexBufferTemplate : public Resource<RES_INDEXBUFFER,typename UserData::IndexBuffer, ChunkIndexBuffer, User>
{
 public:
  int primitive;
  int dataType;

  IndexBufferTemplate() : primitive(0), dataType(0) {}

  bool add(const MsgChunk<ChunkIndexBuffer>& chunk)
  {
    if (!Resource<RES_INDEXBUFFER,typename UserData::IndexBuffer,ChunkIndexBuffer,User>::add(chunk))
      return false;
    primitive = chunk->primitive;
    dataType = chunk->dataType;
    return true;
  }

  bool add(const MsgChunk<ChunkIndexBufferUpdate>& chunk)
  {
    primitive = chunk->primitive;
    dataType = chunk->dataType;
    return true;
  }

  int nbIndex() const
  {
    if (this->empty() || dataType==0) return 0;
    return this->firstLevel()->dataSize()/Type::size(dataType);
  }

};

} // namespace render

} // namespace flowvr

#endif
