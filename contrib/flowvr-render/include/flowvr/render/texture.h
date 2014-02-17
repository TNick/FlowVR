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
* File: ./include/flowvr/render/texture.h                         *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_TEXTURE_H
#define FLOWVR_RENDER_TEXTURE_H

#include "resource.h"

namespace flowvr
{

namespace render
{

template<class UserData>
class PrimitiveTemplate;

template<class UserData, class User = PrimitiveTemplate<UserData> >
class TextureTemplate : public Resource<RES_TEXTURE,typename UserData::Texture, ChunkTexture, User>
{
 public:
  int nbDim; // Number of dimensions
  int tp;

  short imageType; ///< See ImageType enum.
  short nx,ny,nz; ///< Size of this texture. ny==0 means a 1D texture and nz==0 means a 2D texture, else a 3D texture
  int pixelType; ///< Type of each pixel (see Type enum).

  TextureTemplate() : nbDim(0), imageType(0), nx(0), ny(0), nz(0), pixelType(0), tp(TP_TEXTURE) {}

  bool add(const MsgChunk<ChunkTexture>& chunk)
  {
    if (!Resource<RES_TEXTURE,typename UserData::Texture,ChunkTexture,User>::add(chunk))
      return false;
    imageType = chunk->imageType;
    nx = chunk->nx;
    ny = chunk->ny;
    nz = chunk->nz;
    pixelType = chunk->pixelType;
    tp = chunk->tp;
    return true;
  }

  bool add(const MsgChunk<ChunkTextureUpdate>& chunk)
  {
    imageType = chunk->imageType;
    nx = chunk->nx;
    ny = chunk->ny;
    nz = chunk->nz;
    pixelType = chunk->pixelType;
    return true;
  }

    short getType() const { return imageType & ChunkTexture::MASK_TYPE; }
    bool isCubemap() const { return (imageType & ChunkTexture::CUBEMAP)!=0; }
    bool isClamp() const { return (imageType & ChunkTexture::CLAMP)!=0; }
    bool isDepth() const { return getType()==ChunkTexture::Depth; }
    bool isColor() const { return !isDepth(); }

};

} // namespace render

} // namespace flowvr

#endif
