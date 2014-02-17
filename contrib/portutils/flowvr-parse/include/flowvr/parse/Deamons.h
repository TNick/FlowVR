/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                       Configuration Parser                      *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA and                                                       *
* Laboratoire d'Informatique Fondamentale d'Orleans               *
* (FRE 2490). ALL RIGHTS RESERVED.                                *
*                                                                 *
* This source is covered by the GNU GPL, please refer to the      *
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
*    Bruno Raffin,                                                *
*    Sophie Robert,                                               *
*    Emmanuel Melin.                                              *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: include/Deamons.h                                         *
*                                                                 *
* Contacts:                                                       *
*  12/10/2003 Sophie Robert <Sophie.Robert@lifo.univ-orleans.fr>  *
*                                                                 *
******************************************************************/
/*! \file Deamons.h
 *
 * \brief the deamon class to store and control the routing table 
 *
 * This class allows the controller to store and reconfigure the
 * orders sent to deamons.
 *
 */

#ifndef DEAMONS_H
#define DEAMONS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
//#include <sstream>
#include <list>
#include <vector>
#include <set>
#include "flowvr/xml.h"

using namespace flowvr::xml;

namespace flowvr
{

namespace parse
{

//typedef std::map<std::string, bool> ListPort;
typedef std::set<std::string> ListOrder;

/*! The generic class of a deamon in the controller domain
 */
class Deamon
{
 protected:
  std::string host;
  ListOrder ActionBuffer;
  ListOrder ActionNet;
  std::list<DOMNode*> ActionBuffers;
  std::list<DOMNode*> ActionNets;
 public:
  Deamon();
  ~Deamon();
  void setHost(const std::string& ModHost);
  bool setBuffer(const std::string& Id, DOMNode* Order);
  bool setNet(const std::string& Id, DOMNode* Order);  
  void getActionBuffers(DOMNode* root);
  void getActionNets(DOMNode* root);
  const std::string& getHost() { return host; }
  void PrintHost();
  void PrintAction();
  void Print();
};

};};//end namespace
#endif

