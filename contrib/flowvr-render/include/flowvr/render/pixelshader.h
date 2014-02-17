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
* File: ./include/flowvr/render/pixelshader.h                     *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_PIXELSHADER_H
#define FLOWVR_RENDER_PIXELSHADER_H

#include "resource.h"

namespace flowvr
{

namespace render
{

template<class UserData>
class PrimitiveTemplate;

template <class UserData, class User = PrimitiveTemplate<UserData> >
class PixelShaderTemplate : public Resource<RES_PIXELSHADER,typename UserData::PixelShader,ChunkPixelShader,User>
{
public:

};

} // namespace render

} // namespace flowvr

#endif
