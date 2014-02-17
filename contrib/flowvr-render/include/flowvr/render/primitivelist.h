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
* File: ./include/flowvr/render/primitivelist.h                   *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_PRIMITIVELIST_H
#define FLOWVR_RENDER_PRIMITIVELIST_H

#include <list>
#include <vector>
#include <map>
#include "primitive.h"

namespace flowvr
{

namespace render
{

/// List of primitives sorted by order.
template<class UserData>
class PrimitiveListTemplate
{

private:
    PrimitiveListTemplate(const PrimitiveListTemplate<UserData>&);
    void operator=(const PrimitiveListTemplate<UserData>&);

public :

  typedef typename std::list< PrimitiveTemplate<UserData> >::iterator iterator;
  typedef typename std::list< PrimitiveTemplate<UserData> >::const_iterator const_iterator;
  typedef typename std::map< int, iterator >::iterator baseitorder;
  class itorder : public baseitorder
  {
  public:
    itorder() {}
    itorder(const baseitorder& it) : baseitorder(it) {}
    int order() const { return (*this)->first; }
    iterator begin() { return (*this)->second; }
    iterator end() { baseitorder next = *this; ++next; return next->second; }
  };

protected :
    
  /// Main list of primitives, ordered.
  std::list< PrimitiveTemplate<UserData> >* primList;

  /// Pointer to first primitive of each order
  std::map< int, iterator >* orderMap;

  //itorder orderEndIt;

  /// Store a pointer to each primitive given its id.
  IDMap< iterator >* idMap;
  
public :

  /// Store the last modification's iteration
  int iteration;

  PrimitiveListTemplate();
  ~PrimitiveListTemplate();

  /// @name Primitive enumeration methods
  /// @{

  /// Returns the iterator on the first element of the list
  iterator begin()
  {
    return primList->begin();
  }

  /// Returns the iterator after the last element of the list
  iterator end()
  {
    return primList->end();
  }

  /// Returns the iterator on the first element of the list
  const_iterator begin() const
  {
    return primList->begin();
  }

  /// Returns the iterator after the last element of the list
  const_iterator end() const
  {
    return primList->end();
  }

  /// @}

  /// Compute the global scene bounding box
  BBox calcBBox();

  /// @name Single Primitive handling
  /// @{

  /// Add a primitive. Returns the pointer to the new primitive, or NULL if failed.
  PrimitiveTemplate<UserData> *add(ID id, PrimitiveTemplate<UserData> * model=NULL, int order=0, int eTp = TP_OBJECT, int tg = (1<<0));


  /// Access a primitive given its ID. Returns NULL if not found.
  PrimitiveTemplate<UserData> *get(ID id) const
  {
    typename IDMap<iterator>::iterator it = idMap->find(id);
    if (it == idMap->end())
      return NULL;
    else
      return &(*(it->second));
  }

  /// Access a primitive given its ID. Returns NULL if not found.
    PrimitiveTemplate<UserData> *getByType(int type, int id = 0, int pos = -1)
    {
      for( iterator it = begin(); it != end(); ++it )
      {
    	if( (*it).tp == type )
    		if( !id or id == (*it).id )
    			return &(*it);
      }
      return NULL;
    }



  /// Remove a primitive. Returns true if ok.
  bool del(ID id);

  /// Change the order of a primitive. Returns true if ok.
  bool move(ID id, int order);

  /// Swap the contents of two lists
  void swap(PrimitiveListTemplate<UserData>& l);

  /// Compute the last iteration when a primitive was updated
  int lastIt() const
  {
    int it = iteration;
    for (const_iterator i = begin(); i != end(); i++)
    {
      int it2 = i->lastIt();
      if (it2 > it)
    	  it = it2;
    }
    return it;
  }


  /// @}

protected:

  /// Update Order Map adding a new primitive
  void orderMapAdd(int order, iterator prim);

  /// Update Order Map deleting a primitive
  void orderMapDel(int order, iterator prim);

};
 
} // namespace render
 
} // namespace flowvr

#endif
