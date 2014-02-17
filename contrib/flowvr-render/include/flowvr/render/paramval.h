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
* File: ./include/flowvr/render/paramval.h                        *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_PARAMVAL_H
#define FLOWVR_RENDER_PARAMVAL_H

#include <flowvr/id.h>
#include <ftl/type.h>

namespace flowvr{

namespace render{

template<class UserData>
class ParamVal : public ftl::TypedArray
{
public:
  UserData userData;
  void operator=(const ftl::TypedArray& from)
  {
    ftl::TypedArray::operator=(from);
  }
};

}//render

}//flowvr


#endif
