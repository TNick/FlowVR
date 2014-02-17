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
* File: ./src/librender/chunkreader.cpp                           *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include "flowvr/render/chunkrenderreader.h"
#include "flowvr/render/chunkrenderreader.inl"
#include "flowvr/render/primitivelist.inl"

namespace flowvr
{

namespace render
{

template class PrimitiveListTemplate<ChunkRenderReaderDefaultUserData>;
template class ChunkRenderReader<>;

} // namespace render

} // namespace flowvr
