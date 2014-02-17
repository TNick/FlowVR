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
* File: ./include/flowvr/render/resource.h                        *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_RESOURCE_H
#define FLOWVR_RENDER_RESOURCE_H

#include <vector>
#include <flowvr/id.h>
#include <flowvr/message.h>
#include <ftl/vec.h>
#include "flowvr/render/chunk.h"
#include "flowvr/render/bbox.h"

namespace flowvr
{

namespace render
{

template <ResourceType Type, class UserData, class ChunkType=ChunkResource, class User = void >
class Resource
{
 public:
  typedef std::list< User* > UserList;
  UserList users;

  ID id;
  bool created;


  std::vector< MsgChunk<ChunkType> > levels;

  int iteration; ///< Iteration of last change

  UserData userData;

  static ResourceType type()
  {
    return Type;
  }

  Resource()
  : id(0), created(false), iteration(-1)
  {
  }

  bool add(const MsgChunk<ChunkType>& data)
  {
    int level = data->level;
    if (level<0) level = 0;
    if ((int)levels.size()<=level)
      levels.resize(level+1);
    levels[level] = data;
    return true;
  }

  void del(int level=-1, int gen=-1)
  {
    if (level==-1)
    {
      if (gen==-1)
      {
	levels.clear();
      }
      else
      {
	int last = -1;
	for (unsigned int i=0;i<levels.size();i++)
	{
	  if (!levels[i].empty())
	  {
	    if (levels[i]->gen <= gen)
	      levels[i].clear();
	    else
	      last = i;
	  }
	}
	if (last < (int)levels.size()-1)
	  levels.resize(last+1);
      }
    }
    else
    {
      if (level < (int)levels.size())
      {
	if (gen == -1 || levels[level]->gen <= gen)
	{
	  levels[level].clear();
	  if (level == (int)levels.size()-1)
	  {
	    int last = level-1;
	    while (last>=0 && levels[last].empty())
	      --last;
	    levels.resize(last);
	  }
	}
      }
    }
  }

  bool empty() const
  {
    return levels.empty();
  }

  MsgChunk<ChunkType> firstLevel() const
  {
    for (unsigned int i=0;i<levels.size();i++)
      if (!levels[i].empty())
	return levels[i];
    return MsgChunk<ChunkType>();
  }


  int lastIt() const
  {
    return iteration;
  }

};

template<ResourceType Type, class UserData, class ChunkType=ChunkResource3D, class User = void >
class Resource3D : public Resource<Type,UserData,ChunkType,User>
{
public:
  BBox bb;
  void updateBBox(const MsgChunk<ChunkType>& data)
  {
    bb = BBox(Vec3f(data->bbox[0]), Vec3f(data->bbox[1]));
    for (unsigned int i=0;i<this->levels.size();i++)
      if (!this->levels[i].empty())
      {
	bb += BBox(Vec3f(this->levels[i]->bbox[0]), Vec3f(this->levels[i]->bbox[1]));
      }
  }

};

} // namespace render

} // namespace flowvr

#endif
