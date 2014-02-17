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
* File: include/Objects.h                                         *
*                                                                 *
* Contacts:                                                       *
*  12/10/2003 Sophie Robert <Sophie.Robert@lifo.univ-orleans.fr>  *
*                                                                 *
******************************************************************/
/*! \file Objects.h
 *
 * \brief the classes to manipulate the FlowVR objects
 *
 * These classes allow to register and manipulate the modules, the
 * filters and the synchronizers.
 *
 */

#ifndef OBJECTS_H
#define OBJECTS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>


namespace flowvr { namespace parse {


class Port
{
 protected:
  std::string id;
  std::string datatype;
  bool full;
 public:
  Port();
  ~Port();
  void setIdentifier(const std::string& PortId);
  void setDataType(bool existence, const std::string& DataType);
  std::string getIdentifier();
  std::string getDataType();
  bool getFull();
};

typedef std::map<std::string, Port*> ListPort;

  
/*! The generic class of FlowVR objects.
 */
class Object
{
 protected:
  std::string id;          
  std::string host;
  ListPort Inputs;
  ListPort Outputs;
 public:
  Object();
  virtual ~Object();
  void setIdentifier(const std::string& ModId);
  void setHost(const std::string& ModHost);
  void setInput(const std::string& PortId, Port* Port);
  void setOutput(const std::string& PortId, Port* Port);  
  std::string getIdentifier();
  std::string getHost();
  bool InputExistence(const std::string& PortId);
  bool OutputExistence(const std::string& PortId);
  virtual bool InputDataType(const std::string& PortId);
  virtual bool OutputDataType(const std::string& PortId);
  void PrintId();
  void PrintHost();
  void PrintPort();
};

/*! The module class
 */
class Module: public Object
{
 public:
  Module();
  virtual ~Module();
  void Print();
};
/*! The filter class
 */
class Filter: public Object
{
 protected:
  std::string FClass;
 public:
  Filter();
  virtual ~Filter();
  void setClass(const std::string& Class);
  void Print();
};


/*! The synchronizer class
 */
class Synchronizer: public Object
{
 protected:
  std::string SClass;
 public:
  Synchronizer();
  virtual ~Synchronizer();
  void setClass(const std::string& Class);
  void Print();
};

/*! The routing node class
 */
class RoutingNode
{
 protected:
  std::string id;
  std::string host;
  std::string prevnode; // previous node in a path
  std::string truesource;
  std::string sourcetype;
  std::string portsource;
  bool type; // routingnode (1) ou routingnodestamps (0)
  bool full; // to indicate the addroute NET action attribute
 public:
  RoutingNode();
  ~RoutingNode();
  void setIdentifier(const std::string& ModId);
  void setHost(const std::string& ModHost);
  void setPrevNode(const std::string& previous);
  void setTrueSource(const std::string& source, const std::string& sport, const std::string& type);
  void setType(bool Type);
  void setFull(bool Full);
  std::string getIdentifier();
  std::string getHost();
  std::string getPrevNode();
  std::string getTrueSource();
  std::string getSourceType();
  std::string getPortSource();
  bool getType();
  bool getFull();
  void PrintId();
  void PrintHost();
  void PrintPrevious();
  void PrintSource();
  void PrintType();
  void PrintFull();
  void Print();
};
 
                 
};}; // end namespace

#endif

