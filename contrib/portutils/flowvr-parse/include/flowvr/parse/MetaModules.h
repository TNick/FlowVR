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
* File: include/MetaModules.h                                         *
*                                                                 *
* Contacts:                                                       *
*  12/10/2003 Sophie Robert <Sophie.Robert@lifo.univ-orleans.fr>  *
*                                                                 *
******************************************************************/
/*! \file MetaModules.h
 *
 * \brief the module class for the FlowVR controller
 *
 * This class is reduced to the information for the launch script 
 * construction
 */

#ifndef METAMODULES_H
#define METAMODULES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>

#include "flowvr/parse/Objects.h"


namespace flowvr
{

namespace parse
{




/*! The meta module class
 */
class MetaModule
{
 protected:
  std::string id;          /*!<The module identifier */
  std::string ScriptName;  /*!<The file name of the launch script */
  std::string ScriptCmd;   /*!<The command name to execute the module from the controller */
  ListPort Inputs;
  ListPort Outputs;
  std::vector<std::string> Hosts;
 public:
  MetaModule();
  ~MetaModule();
  std::string getId        ();
  std::string getScriptName();
  void setIdentifier(const std::string& ModId);
  void setScriptName(const std::string& FileName);
  void setScriptCmd(const std::string& CmdName);
  void setInput(const std::string& PortId, Port* Port);
  void setOutput(const std::string& PortId, Port* Port);
  void setHost(const std::string& Host);
  std::string getScriptCmd();
  ListPort getInput();
  ListPort getOutput();
  std::vector<std::string> getHosts();
  void PrintPort();
  void PrintHost();
};


};}; // end namespace

#endif

