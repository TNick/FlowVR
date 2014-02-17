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
* File: src/Deamons.cpp                                           *
*                                                                 *
* Contacts:                                                       *
*  12/10/2003 Sophie Robert <Sophie.Robert@lifo.univ-orleans.fr>  *
*                                                                 *
******************************************************************/
/*! \file Deamons.cpp
 *
 * \brief The functions associated to the controller deamon class
 *
 */


#include "flowvr/parse/Deamons.h"


namespace flowvr { namespace parse {


/*! \fn Deamon::Deamon()
 * \brief constructor
 */
Deamon::Deamon() 
{}

/*! \fn Deamon::~Deamon()
 * \brief destructor
 */
Deamon::~Deamon()
{}


/*! \fn void Deamon::setHost(const std::string& Host)
 * \brief to initialize the deamon host and its identifier
 * \param Host : the host
 */
void Deamon::setHost(const std::string& Host)
{
  host = Host;
}

/*! \fn bool Deamon::setBuffer(const std::string& Id, DOMElement* Order)
 * \brief The Buffer action list
 * \param Id : the order identifier
 * \param Order : The DOMElement AddRoute
 * \return test : true if the order already exists
 */
bool Deamon::setBuffer(const std::string& Id, DOMNode* Order)
{
  bool insert = ActionBuffer.insert(Id).second;
  if (insert) {
	  ActionBuffers.push_back(Order);
    //ActionBuffers << Order << '\n';
  }
  return !insert;
}

/*! \fn bool Deamon::setNet(const std::string& Id, DOMElement* Order)
 * \brief The Net action list
 * \param Id : the order identifier
 * \param Order : The DOMElement AddRoute
 * \return test : true if the order already exists
 */
bool Deamon::setNet(const std::string& Id, DOMNode* Order)
{
  bool insert = ActionNet.insert(Id).second;
  if (insert) {
	  ActionNets.push_back(Order);
    //ActionNets << Order << '\n';
  }
  return !insert;
}

void Deamon::getActionBuffers(DOMNode* root) {
//  ActionBuffers.flush();
	for (std::list<DOMNode*>::iterator it = ActionBuffers.begin(); it != ActionBuffers.end(); ++it)
	{
		root->LinkEndChild(*it);
	}
}

void Deamon::getActionNets(DOMNode* root) {
//  ActionNets.flush();
  for (std::list<DOMNode*>::iterator it = ActionNets.begin(); it != ActionNets.end(); ++it)
	{
		root->LinkEndChild(*it);
	}

//  return ActionNets.str();
}

/*! \fn void Deamon::PrintHost()
 * \brief to print the Host deamon name
 */
void Deamon::PrintHost()
{
  std::cerr << "Daemon of " << host << "\n";
}

/*! \fn void Deamon::PrintAction()
 * \brief to print the list of Net and Buffer orders
 */
void Deamon::PrintAction()
{
  std::cerr << "Buffer Action : " << "\n";
  for(std::list<DOMNode*>::iterator it = ActionBuffers.begin(); it != ActionBuffers.end(); ++it)
  {
	std::cerr << **it << std::endl;
  }

  std::cerr << "Net Action : " << "\n";
  for(std::list<DOMNode*>::iterator it = ActionNets.begin(); it != ActionNets.end(); ++it)
  {
	std::cerr << **it << std::endl;
  }

}


/*! \fn void Deamon::Print()
 * \brief to print all the deamon informations
 */
void Deamon::Print()
{
  PrintHost();
  PrintAction();
}

};}; // end namespace
