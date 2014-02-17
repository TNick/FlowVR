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
* File: ./include/flowvr/render/primitivelist.inl                 *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

namespace flowvr
{

namespace render
{

enum { ORDER_INF = 0x7fffffff };

template<class UserData>
PrimitiveListTemplate<UserData>::PrimitiveListTemplate()
: primList( new std::list< PrimitiveTemplate<UserData> >)
, orderMap( new std::map< int, iterator >)
, idMap(new IDMap< iterator >)
, iteration(-1)
{  
  (*orderMap)[ORDER_INF] = primList->end();
}

template<class UserData>
PrimitiveListTemplate<UserData>::~PrimitiveListTemplate()
{
  delete primList;
  delete orderMap;
  delete idMap;
}

template<class UserData>
PrimitiveTemplate<UserData> *PrimitiveListTemplate<UserData>::add(ID id, PrimitiveTemplate<UserData> *model, int order, int eTp, int tg)
{
  //first check if id is already used
  if (get(id)!=NULL)
    return NULL;

  // sorted insert (by order)
  iterator ins = orderMap->lower_bound(order+1)->second;
  // Insert the primitive in the list
  if (model!=NULL)
    ins = primList->insert(ins, *model);
  else
    ins = primList->insert(ins, PrimitiveTemplate<UserData>());

  ins->id    = id;
  ins->order = order;
  ins->tp    = eTp;
  ins->targetGroups = tg;

  orderMapAdd(order, ins);

  // Update the ID Map database
  idMap->insert(std::pair<ID,iterator>(id, ins));
  
  return &(*ins);
}

template<class UserData>
void PrimitiveListTemplate<UserData>::orderMapAdd(int order, iterator prim)
{
  // Update the Order database
  if (prim == primList->begin())
    (*orderMap)[order] = prim; // first primitive of this order
  else
  {
    iterator prev = prim; --prev;
    if (prev->order != order)
      (*orderMap)[order] = prim; // first primitive of this order
  }
}

template<class UserData>
void PrimitiveListTemplate<UserData>::orderMapDel(int order, iterator prim)
{
  iterator next = prim; ++next;
  if (prim == primList->begin())
  { // first primitive
    if (next == primList->end() || next->order != order)
    { // no more primitive with this order
      orderMap->erase(order);
    }
    else
      (*orderMap)[order] = next;
  }
  else
  {
    iterator prev = prim; --prev;
    if (prev->order != order)
    { // first primitive of this order
      if (next == primList->end() || next->order != order)
      { // no more primitive with this order
    	  orderMap->erase(order);
      }
      else
    	  (*orderMap)[order] = next;
    }
  }
}

//delete entry in Order when list becomes empty ?
template<class UserData>
bool PrimitiveListTemplate<UserData>::del(ID id)
{
  typename IDMap<iterator>::iterator mapit = idMap->find(id);
  if (mapit==idMap->end())
    return false;
  iterator it = mapit->second;
  idMap->erase(mapit);

  int order = it->order;

  orderMapDel(order, it);

  primList->erase(it);

  return true;
}
  
template<class UserData>
bool PrimitiveListTemplate<UserData>::move(ID id, int order)
{
  typename IDMap<iterator>::iterator mapit = idMap->find(id);
  if (mapit==idMap->end())
    return false;
  iterator it = mapit->second;

  int old_order = it->order;
  if (old_order == order)
	  return true; // Nothing to do

  orderMapDel(old_order, it);

  // swap the primitive
  primList->splice(orderMap->lower_bound(order+1)->second,*primList,it);
  it->order = order;

  orderMapAdd(order, it);

  return true;
}

template<class UserData>
BBox PrimitiveListTemplate<UserData>::calcBBox()
{
  BBox bb;
  for (iterator it=begin(); it!=end(); ++it)
    if (it->visible)
      bb+=it->aabb;
  return bb;
}

/// Swap the contents of two lists
template<class UserData>
void PrimitiveListTemplate<UserData>::swap(PrimitiveListTemplate<UserData>& l)
{
  { std::list< PrimitiveTemplate<UserData> >* t = primList; primList = l.primList; l.primList = t; }
  { std::map< int, iterator >* t = orderMap; orderMap = l.orderMap; l.orderMap = t; }
  { IDMap< iterator >* t = idMap; idMap = l.idMap; l.idMap = t; }

  { int t = iteration; iteration = l.iteration; l.iteration = t; }
}

} // namespace render

} // namespace flowvr
