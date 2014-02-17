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
* File: src/Objects.cpp                                           *
*                                                                 *
* Contacts:                                                       *
*  12/10/2003 Sophie Robert <Sophie.Robert@lifo.univ-orleans.fr>  *
*                                                                 *
******************************************************************/
/*! \file Objects.cpp
 *
 * \brief The functions associated to the different FlowVR objects 
 *
 */

#include "flowvr/parse/Objects.h"


namespace flowvr { namespace parse {

Port::Port()
{}
Port::~Port()
{}

void Port::setIdentifier(const std::string& PortId)
{
  id = PortId;
}

void Port::setDataType(bool existence, const std::string& DataType)
{
  if (existence) {
    full = 1;
    datatype = DataType;
  }
  else
    full = 0;
}

std::string Port::getIdentifier()
{
  return id;
}

std::string Port::getDataType()
{
  return datatype;
}

bool Port::getFull()
{
  return full;
}

/*! \fn Object::Object()
 * \brief constructor
 */
Object::Object() 
{}

/*! \fn Object::~Object()
 * \brief destructor
 */
Object::~Object()
{}


/*! \fn void Object::setIdentifier(const std::string& ModId)
 * \brief to initialize the object identifier
 * \param ModId : the identifier
 */
void Object::setIdentifier(const std::string& ModId)
{
  id = ModId;
}

/*! \fn void Object::setHost(const std::string& ModHost)
 * \brief to initialize the object host
 * \param ModHost : the host
 */
void Object::setHost(const std::string& ModHost)
{
  host = ModHost;
}

/*! \fn void Object::setInput(const std::string& PortId, bool test)
 * \brief to add a port to the object input ports
 * \param PortId : The port identifier
 * \param test : not use but for stamps test 
 */
void Object::setInput(const std::string& PortId, Port* Port)
{
  Inputs[PortId] = Port;
}

/*! \fn void Object::setOutput(const std::string& PortId, bool test)
 * \brief to add a port to the object output ports
 * \param PortId : The port identifier
 * \param test : not use but for stamps test 
 */
void Object::setOutput(const std::string& PortId, Port* Port)
{
  Outputs[PortId] = Port;
}

/*! \fn std::string Object::getIdentifier()
 * \brief to get the object identifier
 */
std::string Object::getIdentifier()
{
  return id;
}

/*! \fn std::string Object::getHost()
 * \brief to get the object host
 */
std::string Object::getHost()
{
  return host;
}

/*! \fn bool Object::InputExistence(const std::string& PortId)
 * \brief to test an input port existence
 * \param PortId : an identifier 
 * \return true in case of the existence of port named PortId
 */
bool Object::InputExistence(const std::string& PortId)
{
  ListPort::iterator index = Inputs.find(PortId);
  return (index != Inputs.end());
}

bool Object::InputDataType(const std::string& PortId)
{
  ListPort::iterator index = Inputs.find(PortId);
  if (index != Inputs.end())
    return (index->second)->getFull();
  else
  {
    std::cerr<<"Warning : undeclared input port "<<PortId<<std::endl;
    return false;
  }
}



/*! \fn bool Object::OutputExistence(const std::string& PortId)
 * \brief to test an output port existence
 * \param PortId : an identifier 
 * \return true in case of the existence of port named PortId
 */
bool Object::OutputExistence(const std::string& PortId)
{
  ListPort::iterator index = Outputs.find(PortId);
  return (index != Outputs.end());
}

bool Object::OutputDataType(const std::string& PortId)
{
  ListPort::iterator index = Outputs.find(PortId);
  if (index != Outputs.end())
    return (index->second)->getFull();
  else
  {
    std::cerr<<"Warning : undeclared output port "<<PortId<<std::endl;
    return false;
  }
}



/*! \fn void Object::PrintId()
 * \brief to print the identifier 
 */
void Object::PrintId()
{
  std::cerr << "Identifier : " << id << "\n";
}

/*! \fn void Object::PrintHost()
 * \brief to print the host 
 */
void Object::PrintHost()
{
  std::cerr << "Host : " << host << "\n";
}

/*! \fn void Object::PrintPort()
 * \brief to print the port set (input and output) 
 */
void Object::PrintPort()
{

  ListPort::iterator index;
  if (Inputs.begin() != Inputs.end()) {
    std::cerr << "Input ports \n";
    for (index=Inputs.begin(); index != Inputs.end(); index++) {
      std::cerr << index->first << " : " ;
    Port* CurrentPort = index->second;
    if (CurrentPort->getFull()) {
      std::string DataType = CurrentPort->getDataType();
      std::cerr << "Port of " << DataType << " type \n";
    }
    else std::cerr << "Stamps port \n";      
    }
    std::cerr << "\n";
  }
  if (Outputs.begin() != Outputs.end()) {
    std::cerr << "Output ports \n";
    for (index=Outputs.begin(); index != Outputs.end(); index++) {
      std::cerr << index->first << " " ;
      Port* CurrentPort = index->second;
      if (CurrentPort->getFull()) {
	std::string DataType = CurrentPort->getDataType();
      std::cerr << "Port of " << DataType << " type \n";
      }
    else std::cerr << "Stamps port \n";      
    }
    std::cerr << "\n";
  }
}


/*! \fn Module::Module()
 * \brief constructor
 */
Module::Module() 
{}

/*! \fn Module::~Module()
 * \brief destructor
 */
Module::~Module()
{}

/*! \fn void Module::Print()
 * \brief to write the module features
 */
void Module::Print()
{
  std::cerr << "The module has the following features \n";
  PrintId();
  PrintHost();
  PrintPort();
}
/*! \fn Filter::Filter()
 * \brief constructor
 */
Filter::Filter() 
{}
/*! \fn Filter::~Filter()
 * \brief destructor
 */
Filter::~Filter()
{}

/*! \fn void Filter::setClass(const std::string& Class)
 * \brief to initialize the filter class
 * \param Class : the class name
 */
void Filter::setClass(const std::string& Class)
{
  FClass = Class;
}
/*! \fn void Filter::Print()
 * \brief to write the filter features
 */
void Filter::Print()
{
  std::cerr << "The filter has the following features \n";
  PrintId();
  PrintHost();
  PrintPort();
}

/*! \fn Synchronizer::Synchronizer()
 * \brief constructor
 */
Synchronizer::Synchronizer() 
{}
/*! \fn Synchronizer::~Synchronizer()
 * \brief destructor
 */
Synchronizer::~Synchronizer()
{}

/*! \fn void Synchronizer::setClass(const std::string& Class)
 * \brief to initialize the synchronizer class
 * \param Class : the class name
 */
void Synchronizer::setClass(const std::string& Class)
{
  SClass = Class;
}

/*! \fn void Synchronizer::Print()
 * \brief to write the synchronizer features
 */
void Synchronizer::Print()
{
  std::cerr << "The synchronizer has the following features \n";
  PrintId();
  PrintHost();
  PrintPort();
}
/*! \fn RoutingNode::RoutingNode()
 * \brief constructor
 */
RoutingNode::RoutingNode() 
{
  prevnode = "null";
  type = 1;
  full = 1;
}
/*! \fn RoutingNode::~RoutingNode()
 * \brief destructor
 */
RoutingNode::~RoutingNode()
{}

/*! \fn void RoutingNode::setIdentifier(const std::string& Id)
 * \brief to initialize theRoutingNode identifier
 * \param Id : the identifier
 */
void RoutingNode::setIdentifier(const std::string& Id)
{
  id = Id;
}

/*! \fn void RoutingNode::setHost(const std::string& Host)
 * \brief to initialize the routingNode host
 * \param Host : the host
 */
void RoutingNode::setHost(const std::string& Host)
{
  host = Host;
}
/*! \fn void RoutingNode::setPrevNode(const std::string& previous)
 * \brief to initialize the previous routing node identifier
 * \param Host : the previous node identifier 
 */
void RoutingNode::setPrevNode(const std::string& previous)
{
  prevnode = previous;
}

void RoutingNode::setTrueSource(const std::string& source,const std::string& sport,const std::string& type)
{
  truesource = source;
  portsource = sport;
  sourcetype = type;
}

void RoutingNode::setFull(bool Full)
{
  full = Full;
}

void RoutingNode::setType(bool Type)
{
  type = Type;
}
/*! \fn std::string RoutingNode::getIdentifier()
 * \brief to get the RoutingNode identifier
 */
std::string RoutingNode::getIdentifier()
{
  return id;
}

/*! \fn std::string RoutingNode::getHost()
 * \brief to get the RoutingNode host
 */
std::string RoutingNode::getHost()
{
  return host;
}
/*! std::string RoutingNode::getPrevNode()
 * \brief to get the previous routing node identifier
 */
std::string RoutingNode::getPrevNode()
{
  return prevnode;
}

std::string RoutingNode::getTrueSource()
{
  return truesource;
}

std::string RoutingNode::getPortSource()
{
  return portsource;
}


std::string RoutingNode::getSourceType()
{
  return sourcetype;
}


bool RoutingNode::getType()
{
  return type;
}

bool RoutingNode::getFull()
{
  return full;
}

/*! \fn void RoutingNode::PrintId()
 * \brief to print the identifier 
 */
void RoutingNode::PrintId()
{
  std::cerr << "Identifier : " << id << "\n";
}

/*! \fn void RoutingNode::PrintHost()
 * \brief to print the host 
 */
void RoutingNode::PrintHost()
{
  std::cerr << "Host : " << host << "\n";
}

void RoutingNode::PrintPrevious()
{
  std::cerr << "Previous node : " << prevnode << "\n";
}

void RoutingNode::PrintSource()
{ 
  std::cerr << "The real source is the port " << portsource << " of the " 
	    << sourcetype << " " << truesource << "\n";
}

void RoutingNode::PrintType()
{
  std::cerr << "Routing node (1)  Routing node stamps (0) : " << type << "\n";
}

void RoutingNode::PrintFull()
{
  std::cerr << "Routing node NET attribute : " << full << "\n";
}


/*! \fn void RoutingNode::Print()
 * \brief to write the RoutingNode features
 */
void RoutingNode::Print()
{
  std::cerr << "The routing node has the following features \n";
  PrintId();
  PrintHost();
  PrintSource();
  PrintPrevious();
  PrintType();
  PrintFull();
}

};}; // end namespace
