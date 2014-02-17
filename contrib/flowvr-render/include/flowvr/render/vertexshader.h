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
* File: ./include/flowvr/render/vertexshader.h                    *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_VERTEXSHADER_H
#define FLOWVR_RENDER_VERTEXSHADER_H

#include "resource.h"

namespace flowvr
{

namespace render
{

template<class UserData>
class PrimitiveTemplate;

template<class UserData, class User = PrimitiveTemplate<UserData> >
class VertexShaderTemplate : public Resource<RES_VERTEXSHADER,typename UserData::VertexShader,ChunkVertexShader,User>
{
public:

};

} // namespace render

} // namespace flowvr

#endif
