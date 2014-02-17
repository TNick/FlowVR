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
* File: ./include/flowvr/render/engine.h                          *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_ENGINE_H
#define FLOWVR_RENDER_ENGINE_H

#include "chunkrenderreader.h"
#include "primitivelist.inl"
#include <map>
#include <iostream>
#include <deque>
#include <functional>
#include <algorithm>

namespace flowvr
{

namespace render
{


template <class UserData>
class BaseEngine : public ChunkRenderReader<UserData>
{
public:
  typedef ChunkRenderReader<UserData> Base;

  class Scene
  {
  public:
	    Scene()
	    : nx(0)
	    , ny(0)
	    , sceneBBox()
	    , userData()
	    , camera(NULL)
	    , enabled(true)
	    {}

	  typedef std::vector< PrimitiveTemplate<UserData>* > PRIMS;
	  PrimitiveTemplate<UserData>  *camera; // it's the reference frame, let's call it camera

	  BBox                          sceneBBox;
	  typename UserData::Target     userData;
	  int                           nx,ny;
	  bool                          enabled;


	  PrimitiveTemplate<UserData>* operator[]( int idx ) { return primVec[idx]; }
	  const PrimitiveTemplate<UserData>* operator[]( int idx ) const { return primVec[idx]; }

	  bool empty() const { return primVec.empty(); }


	  virtual bool preRender( float &aspectRatio, int vp[4], int lastIt, bool &setVp )
	  {
		  setVp = false;
		  return true;
	  }

	  virtual void postRender() {}

	  virtual void dump() {}

	  class _newIt
	  {
	  public:
		  _newIt( int it ) : m_it(it) {}

		  int m_it;
		  bool operator()( PrimitiveTemplate<UserData> *p ) const
		  {
			  return (p->iteration >= m_it);
		  }
	  };

	  virtual bool changedSince( int it ) const
	  {
		  return true; //(std::find_if( primVec.begin(), primVec.end(), _newIt(it) ) != primVec.end());
	  }

	  const PRIMS &getPrimitives() const { return primVec; }
	  PRIMS &getPrimitives() { return primVec; }
  private:
	  friend class BaseEngine;
	  PRIMS primVec;

  };

private:

public:
  float sceneVertFOV; ///< For free camera mode: global parameters
  Vec4f bboxColor;


  BaseEngine()
  : sceneVertFOV(60.0f)
  , bboxColor(0,0,0,1)
  {
  }

  virtual ~BaseEngine()
  {
	  for( typename std::map<int, Scene*>::const_iterator it = m_targetGroups.begin(); it != m_targetGroups.end(); ++ it)
		  delete (*it).second;
  }

protected:

  virtual Scene *createTargetScene( unsigned int nSceneTp ) = 0;

  virtual void   enforcePrimitiveAddConstraint( Scene *s, typename Base::Primitive *p, unsigned int mask )
  {
	  if( p->tp == TP_CAMERA )
	  {
		if( s->camera ) // already a 'camera' attached?
		{
			s->camera->lastTargetGroups = s->camera->targetGroups;
			s->camera->targetGroups = ( s->camera->targetGroups bitand ~mask); // yes, remove its tg bit for this group
		}

		// re-assign camera (we assume the bitmask is already set-up correctly for p)
		s->camera = p;
	  }
	  else
		  s->primVec.push_back(p); // ordinary primitive, add to vector (sort later)
  }

  typedef std::map<int, Scene*, std::greater<int> > TGTGRP;
  TGTGRP m_targetGroups;

public:
  void printTargetGroups() const
  {
	  for( typename TGTGRP::const_iterator it = m_targetGroups.begin(); it != m_targetGroups.end(); ++it )
	  {
		  std::cout << "[" << (*it).first << "] has (" << (*it).second->getPrimitives().size() << ") elements.";
		  std::cout << "\t" << ((*it).second->camera ? "CAM ATTACHED" : "NO CAM ATTACHED") << std::endl;

		  (*it).second->dump();
	  }
  }

  void printTextures() const
  {
	  for( typename ChunkRenderReader<UserData>::TextureList::const_iterator cit
			     = ChunkRenderReader<UserData>::textures.begin();
			  	  	  cit != ChunkRenderReader<UserData>::textures.end(); ++cit )
	  {
		  std::cout << "tx @ " << &(*cit).second << " is of type [" << (*cit).second.tp << "]" << std::endl;
	  }
  }

protected:
  class _primOrder
  {
  public:
	  bool operator()( const typename Base::Primitive *p1, const typename Base::Primitive *p2 ) const
	  {
//		  if( p1->tp < p2->tp )
//			  return true;
		  return p1->order < p2->order;
	  }
  };

  bool addToTargetGroups( typename Base::Primitive *p, unsigned int mask, bool writeNewMask = true, typename Base::Primitive *model = NULL )
  {
	  for( size_t n=0; n < sizeof(int)*8; ++n)
	  {
		  if( mask bitand (1 << n) )
		  {
			  Scene *s = getOrCreateScene(n);
			  enforcePrimitiveAddConstraint( s, p, (1<<n) );
//			  std::cout << "adding p@" << p << " [" << p->name << "] to targetGroup [" << n << "] @s=" << s << std::endl;
			  std::stable_sort( s->primVec.begin(), s->primVec.end(), _primOrder() );

		  }
	  }

	  if( writeNewMask )
		  p->targetGroups = mask bitor p->targetGroups;


	  return true;
  }

  bool remFromTargetGroups( typename Base::Primitive *p, unsigned int mask, bool bWriteNewMask = true )
  {
	  for( size_t n=0; n < sizeof(int)*8; ++n)
	  {
		  if( mask bitand ( 1 << n ) )
		  {
			  Scene *s = getScene(n);
			  if(s)
			  {
				  if( p->tp == TP_CAMERA and p == s->camera )
				  {
					  s->camera = NULL; // erase old cam
					  std::cout << "removing current cam from scene @ " << s << std::endl;
				  }
				  else
				  {
					  s->primVec.erase(std::remove( s->primVec.begin(), s->primVec.end(), p ), s->primVec.end() );
					  std::cout << "removing p@" << p << " [" << p->name << "] from targetGroup [" << n << "] @s=" << s << std::endl;
				  }
			  }
		  }
	  }
	  if(bWriteNewMask)
		  p->targetGroups = ( p->targetGroups bitand ~mask);
	  return true;
  }

  virtual bool updateTargetGroups( typename Base::Primitive *p )
  {
  	if( p->lastTargetGroups )
  	{
  		// only remove from unused groups
  		// (mask the 'difference' to the last state)
  		unsigned int mask = p->lastTargetGroups bitand ~p->targetGroups;
  		remFromTargetGroups(p, mask, false); // remove from difference

  		// only add to 'new' groups
  		// new - ( old cut )
  		mask = p->targetGroups bitand ~(p->lastTargetGroups bitand p->targetGroups);

  		addToTargetGroups(p, mask);
  	}
  	else
  		addToTargetGroups(p, p->targetGroups, false); // add to all groups marked in the mask

  	return true;
  }

  Scene *getScene( unsigned int id ) const
  {
	  typename TGTGRP::const_iterator it = m_targetGroups.find( id );
	  if( it == m_targetGroups.end() )
		  return NULL;
	  return (*it).second;
  }

  Scene *getOrCreateScene( unsigned int id )
  {
	  typename TGTGRP::const_iterator it = m_targetGroups.find( id );

	  if( it == m_targetGroups.end() )
	  {
		  Scene *s = createTargetScene(id);
		  m_targetGroups[id] = s;
		  return s;
	  }
	  return (*it).second;
  }

};

} // namespace render

} // namespace flowvr

#endif
