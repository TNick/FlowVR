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
* File: ./include/flowvr/render/resourcelist.h                    *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_RESOURCELIST_H
#define FLOWVR_RENDER_RESOURCELIST_H

#include <flowvr/id.h>

namespace flowvr
{

namespace render
{

template <class Resource>
class ResourceList
{
 public:

  Resource* add(ID id)
  {
    if (!id)
    	return NULL;

    if (get(id)!=NULL)
    	return NULL;
    Resource* r = &(Map[id]);
    r->id=id;
    return r;
  }

  Resource* get(ID id)
  {
    typename IDMap<Resource>::iterator it = Map.find(id);
    if (it == Map.end())
    	return NULL;
    else return &(it->second);
  }

  class _compareType : public std::unary_function<typename IDMap<Resource>::value_type, bool>
  {
  public:
	  _compareType(int tp) : m_tp(tp) {}

	  bool operator()( const typename IDMap<Resource>::value_type &v ) const
	  {
		  if( v.second.tp == m_tp )
			  return true;
		  return false;
	  }

	  int m_tp;
  };


  Resource *getByType( int tp ) const
  {
	  typename IDMap<Resource>::const_iterator c = std::find_if( begin(), end(), _compareType(tp) );
	  if( c == end() )
		  return NULL;
	  return const_cast<Resource*>(&(*c).second);
  }

  Resource *getByType( int tp, typename IDMap<Resource>::const_iterator bg,
		                       typename IDMap<Resource>::const_iterator &c) const
  {
	  c = std::find_if( bg, end(), _compareType(tp) );
	  if( c == end() )
		  return NULL;
	  return const_cast<Resource*>(&(*c).second);
  }

  bool del(ID id)
  {
    return Map.erase(id) == 1;
  }

  Resource* operator[](ID id)
  {
    if (!id)
    	return NULL;

    Resource* r = get(id);
    if (r == NULL)
      r = add(id);
    return r;
  }

  typedef typename IDMap<Resource>::iterator iterator;
  typedef typename IDMap<Resource>::const_iterator const_iterator;

  iterator begin() { return Map.begin(); }
  iterator end() { return Map.end(); }
  const_iterator begin() const { return Map.begin(); }
  const_iterator end() const { return Map.end(); }

 protected:

  IDMap<Resource> Map;

};

} // namespace render

} // namespace flowvr

#endif
