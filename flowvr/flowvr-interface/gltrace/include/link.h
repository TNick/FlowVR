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
* File: include/flowvr/gltrace/link.h                             *
*                                                                 *
* Contacts:                                                       *
*  08/2004 Julien Garand                                          *
*                                                                 *
******************************************************************/

class Link
{
public:
  std::string name;
  bool display;

  Event* source;
  Event* destination;
  Match* matchfunc;
  std::vector<Display*> drawfunc;

  void draw()
  {
    if( !display
        or !source->display
        or !destination->display
        or !source->trace->object->display
        or !destination->trace->object->display)
      return;

    int a=0, b=0;

    if( !matchfunc->init(source,destination) )
      return;

    while( !matchfunc->end() )
    {
      a++;
      if( matchfunc->match() )
      {
        b++;
        for(unsigned int i=0;i<drawfunc.size();i++)
          drawfunc[i]->draw( matchfunc->getSrc(), matchfunc->getDest());
      }
    }
  }

  xml::DOMElement xmlDesc()
  {
    xml::DOMElement res=xml::DOMElement("link");
    res.SetAttribute("name",name);

    xml::DOMElement src=xml::DOMElement("source");
    src.SetAttribute("name",source->name);
    res.InsertEndChild(src);

    xml::DOMElement dest=xml::DOMElement("destination");
    dest.SetAttribute("name",destination->name);
    res.InsertEndChild(dest);

    xml::DOMElement match = xml::DOMElement("match");
    match.InsertEndChild(matchfunc->xmlDesc());

    xml::DOMElement disp = xml::DOMElement("lnkdisplay");
    if( display )
    {
      disp.SetAttribute("active","YES");

      for(unsigned int i=0; i<drawfunc.size(); i++)
        disp.InsertEndChild(drawfunc[i]->xmlDesc());
    }
    else
    {
      disp.SetAttribute("active","NO");
    }

    res.InsertEndChild(match);
    res.InsertEndChild(disp);

    return res;
  }

  Link()
  {
  }

  ~Link()
  {
    for(unsigned int i=0; i<drawfunc.size(); i++)
      delete drawfunc[i];

    drawfunc.clear();

    delete matchfunc;
  }

};
