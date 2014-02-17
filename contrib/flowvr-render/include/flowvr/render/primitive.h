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
* File: ./include/flowvr/render/primitive.h                       *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_PRIMITIVE_H
#define FLOWVR_RENDER_PRIMITIVE_H

#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <ftl/vec.h>
#include "bbox.h"
#include "paramval.h"
#include <ftl/quat.h>
#include <flowvr/id.h>
#include "pixelshader.h"
#include "vertexshader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "texture.h"
#include "chunk.h"

namespace flowvr
{

namespace render
{


template<class UserData>
class PrimitiveTemplate
{

public:

  typedef ChunkPrimParam::Param Param;
  typedef ParamVal<typename UserData::ParamVShader> ParamVShader;
  typedef ParamVal<typename UserData::ParamPShader> ParamPShader;
  typedef ParamVal<typename UserData::ParamOpenGL>  ParamOpenGL;
  typedef ParamVal<typename UserData::ParamDisplay> ParamDisplay;

  struct VBData
  {
    VertexBufferTemplate<UserData> * buffer; ///< Buffer to use. Default = NULL.
    int numData;                     ///< Field number. Default = 0.
    int v0;                          ///< First index. Default = 0.
    typename UserData::PrimitiveVertexBuffer userData;

    VBData()
    : buffer(NULL)
    , numData(0)
    , v0(0)
    {}

    bool modified() const
    {
      return buffer!=NULL or numData!=0 or v0!=0;
    }

    bool valid() const
    {
    	 return buffer and !buffer->empty() and !buffer->levels[0].empty();
    }
  };

  struct IBData
  {
	IBData()
	: buffer(NULL)
	, i0(0)
	, nbi(-1)
	{}

    IndexBufferTemplate<UserData>* buffer;  ///< Buffer to use. Default = NULL.
    int i0;                         ///< First index. Default = 0.
    int nbi;                        ///< Number of indices to use (-1 for all). Default = -1.
    typename UserData::PrimitiveIndexBuffer userData;

    bool modified() const
    {
      return buffer!=NULL or i0!=0 or nbi!=-1;
    }

    bool valid() const
    {
    	 return buffer and !buffer->empty() and !buffer->levels[0].empty();
    }
  };

  struct TData
  {
	TData()
	: texture(NULL)
	{}

	TextureTemplate<UserData>          *texture;
    typename UserData::PrimitiveTexture userData;

    bool modified() const
    {
      return texture != NULL;
    }
  };

  
  struct Transform
  {
    Vec3f position; // Default = (0,0,0)
    Quat rotation;
    Vec3f scale; // Default = (1,1,1)
    Mat4x4f mat;
    int iteration; ///< Iteration of last change
    bool override; ///< override / replace current transfor (when true), mult with current (else)

    Transform()
    : scale(1,1,1)
    , iteration(-1)
    , override(false)
    {
    	mat.identity();
    }

    void update()
    {
      Mat3x3f r;
      rotation.toMatrix(&r);
      mat[0][0] = r[0][0]*scale[0];  mat[0][1] = r[0][1]*scale[1];  mat[0][2] = r[0][2]*scale[2];
      mat[1][0] = r[1][0]*scale[0];  mat[1][1] = r[1][1]*scale[1];  mat[1][2] = r[1][2]*scale[2];
      mat[2][0] = r[2][0]*scale[0];  mat[2][1] = r[2][1]*scale[1];  mat[2][2] = r[2][2]*scale[2];

      mat[0][3] = position[0];
      mat[1][3] = position[1];
      mat[2][3] = position[2];
    }
  };

  // --------------------------------------------------------------------------

  ID          id;
  std::string name;

  int order;
  int tp; ///< type of this primitive
  int iteration; ///< Iteration of last change (except transform)
  int nbv;                            ///< Number of used vertices (-1 for all). Default = -1.
  unsigned int targetGroups, lastTargetGroups;

  VertexShaderTemplate<UserData>* vShader;
  PixelShaderTemplate<UserData> * pShader;

  IBData iBuffer;

  std::map<std::string, TData>        textures;
  std::map<std::string, ParamVShader> paramVShader;
  std::map<std::string, ParamPShader> paramPShader;
  std::map<std::string, ParamOpenGL>  paramOpenGL;
  std::map<std::string, ParamDisplay> paramDisplay;
  std::map<std::string, VBData>       vBuffer;

  Transform xform, proj;

  BBox bbox; ///< Local coordinate bounding box
  BBox aabb; ///< Global axis-aligned bounding box

  typename UserData::Primitive userData;
  bool        visible; ///< Is this object visible. Default = true
  
  //methods

  PrimitiveTemplate()
  : id(0)
  , visible(true)
  , order(0)
  , vShader(NULL)
  , pShader(NULL)
  , nbv(-1)
  , iteration(-1)
  , targetGroups(1) // automatically add to default scene (0) upon add
  , lastTargetGroups(0)
  , tp( TP_OBJECT )
  {
  }

  void updateAABB()
  {
	  aabb.apply(bbox, xform.mat);
  }

  void updateBBox()
  {
	  bbox.clear();
	  for (typename std::map<std::string, VBData>::iterator it = vBuffer.begin(); it != vBuffer.end(); ++it)
	  {
		  if (it->second.buffer != NULL)
			bbox += it->second.buffer->bb;
	  }
	  updateAABB();
  }

  int lastIt() const
  {
    int it = iteration;

    if (xform.iteration > it)
    	it = xform.iteration;

    if (vShader!=NULL && vShader->lastIt() > it)
    	it = vShader->lastIt();

    if (pShader!=NULL && pShader->lastIt() > it)
    	it = pShader->lastIt();

    if (iBuffer.buffer!=NULL && iBuffer.buffer->lastIt() > it)
    	it = iBuffer.buffer->lastIt();

    for (typename std::map<std::string, VBData>::const_iterator i = vBuffer.begin(); i != vBuffer.end(); ++i)
    {
      if (i->second.buffer!=NULL && i->second.buffer->lastIt() > it)
    	  it = i->second.buffer->lastIt();
    }
    for (typename std::map<std::string, TData>::const_iterator i = textures.begin(); i != textures.end(); ++i)
    {
      if (i->second.texture!=NULL && i->second.texture->lastIt() > it)
    	  it = i->second.texture->lastIt();
    }
    return it;
  }

};

} // namespace render

} // namespace flowvr


#endif
