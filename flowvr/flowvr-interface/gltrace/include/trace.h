/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                             Utils                               *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA and                                                       *
* Laboratoire d'Informatique Fondamentale d'Orleans               *
* (FRE 2490) ALL RIGHTS RESERVED.                                 *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING file for further information.                           *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Ronan Gaugne,                                                *
*    Valerie Gouranton,                                           *
*    Loick Lecointre,                                             *
*    Sebastien Limet,                                             *
*    Clement Menier,                                              *
*    Bruno Raffin,                                                *
*    Sophie Robert,                                               *
*    Emmanuel Melin.                                              *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: include/flowvr/gltrace/trace.h                            *
*                                                                 *
* Contacts:                                                       *
*  08/2004 Julien Garand                                          *
*                                                                 *
******************************************************************/

class Trace
{
public:
  Host* host;
  Object* object;
  std::string name;
  std::string objectname;
  std::string hostname;

  OwnShotList shotlist;

  bool own(int id)
  {
    for(OwnShotList::iterator it=shotlist.begin();it!=shotlist.end();it++)
      if( *it == id)
        return true;

    return false;
  };

  xml::DOMElement xmlDesc()
  {
    xml::DOMElement desc = xml::DOMElement("trace");

    desc.SetAttribute("object",object->name);
    desc.SetAttribute("name",name);

    for(OwnShotList::iterator it=shotlist.begin();it!=shotlist.end();it++)
    {
      xml::DOMElement shot = xml::DOMElement("shot");
      shot.SetAttribute("id",*it);
      desc.InsertEndChild(shot);
    }

    return desc;
  };

  Trace()
  {
  };

  ~Trace()
  {
  };

};

