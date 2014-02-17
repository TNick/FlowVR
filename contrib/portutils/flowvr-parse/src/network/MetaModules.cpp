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
* File: src/MetaModules.cpp                                       *
*                                                                 *
* Contacts:                                                       *
*  12/10/2003 Sophie Robert <Sophie.Robert@lifo.univ-orleans.fr>  *
*                                                                 *
******************************************************************/
/*! \file MetaModules.cpp
 *
 * \brief The functions associated to the MetaModule class 
 *
 */

#include "flowvr/parse/MetaModules.h"


namespace flowvr { namespace parse {


/*! \fn MetaModule::MetaModule()
 * \brief The module constructor
 */
MetaModule::MetaModule() 
{}

MetaModule::~MetaModule()
{}

/*! \fn char* MetaModule::getId()
 * \return the module identifier
 */
std::string MetaModule::getId()
{
  return id;
}

/*! \fn char* MetaModule::getScriptName()
 * \return the module launching script
 */
std::string MetaModule::getScriptName()
{
  return ScriptName;
}

/*! \fn void MetaModule::setIdentifier(const std::string& ModId)
 * \brief to initialize the module identifier
 * \param ModId : the identifier
 */
void MetaModule::setIdentifier(const std::string& ModId)
{
  id = ModId;
}

/*! \fn void MetaModule::setScriptName(const std::string& FileName)
 * \brief to initialize the lauching script name
 * \param FileName : the script name
 */
void MetaModule::setScriptName(const std::string& FileName)
{
  ScriptName = FileName;
}

/*! \fn void MetaModule::setScriptCmd(const std::string& CmdName)
 * \brief to initialize the lauching command
 * \param CmdName 
 */
void MetaModule::setScriptCmd(const std::string& CmdName)
{
  ScriptCmd = CmdName;
}

/*! \fn char* MetaModule::getScriptCmd()
 * \return the script command
 */
std::string MetaModule::getScriptCmd()
{
  return ScriptCmd;
}

/*! \fn void MetaModule::setInput(const std::string& PortId, bool test)
 * \brief to add a port to the metamodule input ports
 * \param PortId : The port identifier
 * \param test : not use but for stamps test 
 */
void MetaModule::setInput(const std::string& PortId, Port* Port)
{
  Inputs[PortId] = Port;
}

/*! \fn void MetaModule::setOutput(const std::string& PortId, bool test)
 * \brief to add a port to the metamodule output ports
 * \param PortId : The port identifier
 * \param test : not use but for stamps test 
 */
void MetaModule::setOutput(const std::string& PortId, Port* Port)
{
  Outputs[PortId] = Port;
}


void MetaModule::setHost(const std::string& Host)
{
  Hosts.push_back(Host);
}

std::vector<std::string> MetaModule::getHosts()
{ 
  return Hosts;
}


/*! \fn ListPort MetaModule::getInput()
 * \brief return the input port set 
 */
ListPort MetaModule::getInput()
{
  return Inputs;
}

/*! \fn ListPort MetaModule::getOutput()
 * \brief return the output port set 
 */

ListPort MetaModule::getOutput()
{
  return Outputs;
}


/*! \fn void MetaModule::PrintPort()
 * \brief to print the port set (input and output) 
 */
void MetaModule::PrintPort()
{

  ListPort::iterator index;
  std::cerr << "Input ports \n";

  for (index=Inputs.begin(); index != Inputs.end(); index++) {
    std::cerr << index->first << " " ;
  }
  std::cerr << "\n";
  std::cerr << "Output ports \n";
  for (index=Outputs.begin(); index != Outputs.end(); index++) {
    std::cerr << index->first << " " ;
  }
  std::cerr << "\n";
}


void MetaModule::PrintHost()
{
  std::cerr << "la liste des hosts par instance \n";
  std::vector<std::string>::iterator index;
  for (index=Hosts.begin(); index<Hosts.end(); index++)
    std::cerr << *index << " " ;
  std::cerr << "\n";
}

};}; // end namespace
