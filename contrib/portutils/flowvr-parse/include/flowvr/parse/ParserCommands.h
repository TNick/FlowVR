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
 * File: include/ParserCommands                                    *
 *                                                                 *
 * Contacts:                                                       *
 *  12/10/2003 Sophie Robert <Sophie.Robert@lifo.univ-orleans.fr>  *
 *                                                                 *
 ******************************************************************/
/*! \file ParserCommands.h
 *
 * \brief The different parsers.
 *
 * This file regroups the functions to build the orders form the controller
 * to the deamons including the routing table. The parsers work on the instanciated graph.
 */

#ifndef PARSERCOMMANDS_H
#define PARSERCOMMANDS_H

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "flowvr/parse/Objects.h"
#include "flowvr/parse/Deamons.h"
#include "flowvr/xml.h"

namespace flowvr
{

namespace parse
{


void PassOptions(bool object, bool route, bool start);

typedef std::map<std::string, Module*> ModuleArray;
typedef std::map<std::string, Filter*> FilterArray;
typedef std::map<std::string, Synchronizer*> SyncArray;
typedef std::map<std::string, RoutingNode*> RoutingArray;
typedef std::map<std::string, Deamon*> ArrayDeamon;


using namespace flowvr::xml;


// Utility class used to build a full connection (from a source to a destination object, routing node by routing node)
//
class ConnectionStep {

	private:
		ConnectionStep *prev;

	public:
		std::string sourceHost;
		std::string destHost;
		bool isSourceTypeStamps;
		bool isActionTypeStamps;
		bool isActive;

		// Destination information (defined only for the terminal connection) :
		std::string destType; // "moduleid", "filterid", "synchronizerid", "routingnodeid" or "routingnodestampsid"
		std::string destId;
		std::string destPort;


		ConnectionStep(std::string sourceHost, std::string destHost, bool isSourceTypeStamps, bool isActionTypeStamps) {
			this->sourceHost = sourceHost;
			this->destHost = destHost;
			this->isSourceTypeStamps = isSourceTypeStamps;
			this->isActionTypeStamps = isActionTypeStamps;
			this->prev = 0;
			this->isActive = true;

			this->destType = "";
			this->destId = "";
			this->destPort = "";
		}


		~ConnectionStep() {
			// Delete all preceding connection :
			if (prev) delete prev;
		}

		// Precede the whole chain of connection by a new one :
		void SetNewFirstConnection(std::string sourceHost, bool isSourceTypeStamps) {
			ConnectionStep *curConnection = 0;

			curConnection = this;
			while (curConnection->prev)
				curConnection = curConnection->prev;

			curConnection->prev = new ConnectionStep(sourceHost, curConnection->sourceHost, isSourceTypeStamps, curConnection->isSourceTypeStamps);
		}


		// Return final active connection :
		ConnectionStep *GetActiveTerminalConnectionStep() {
			if (this->isActive)
				return this;
			else
				return this->GetPreviousConnectionStep();
		}


		// Return previous active connection :
		ConnectionStep *GetPreviousConnectionStep() {
			ConnectionStep *prevConnection = this->prev;
			bool isPrevConnectionActive;

			if (prevConnection)
				isPrevConnectionActive = prevConnection->isActive;
			else
				return 0;

			while (!isPrevConnectionActive) {
				prevConnection = prevConnection->prev;
				if (prevConnection)
					isPrevConnectionActive = prevConnection->isActive;
				else
					isPrevConnectionActive = true;
			}

			return prevConnection;
		}


		void SetDestinationInformation(std::string destType, std::string destId, std::string destPort) {
			this->destType = destType;
			this->destId = destId;
			this->destPort = destPort;
		}


		void Simplify() {
			ConnectionStep *curConnection = 0;
			ConnectionStep *prevConnection = 0;
			ConnectionStep *nextActiveConnection = 0;

			std::string currentHost = this->sourceHost;

			curConnection = this->prev;
			nextActiveConnection = this;

			while (curConnection) {
				prevConnection = curConnection->prev;

				if (curConnection->sourceHost == currentHost) {
					curConnection->isActive = false;
					nextActiveConnection->isSourceTypeStamps = curConnection->isSourceTypeStamps;
				}

				currentHost = curConnection->sourceHost;

				if (curConnection->isActive)
					nextActiveConnection = curConnection;
				curConnection = prevConnection;
			}
		}

};




// ParserComands functions :

void AddObjectModule(ModuleArray& result, DOMNodeList* SetEltModule, FILE *f);
void AddObjectFilter(FilterArray& result, DOMNodeList* SetEltFilter, FILE *f);
void AddObjectSynchronizer(SyncArray& result, DOMNodeList* SetEltSynchronizer, FILE *f);

DOMNode* DefSource(DOMElement* CurrentId, bool isSourceTypeStamps,
		bool isActionTypeStamps, std::string* Id,
		RoutingArray& LocateRouting);
DOMNode* DefActionBuffer(std::string destId, std::string destPort,
		bool isTypeStamps);
DOMNode* DefActionNet(std::string HostName, bool isTypeStamps);

void AddRoute(DOMNodeList* SetEltModule, DOMNodeList* SetEltFilter, DOMNodeList* SetEltRoutingNode,
		DOMNodeList* SetEltConnection, ArrayDeamon *ListDeamon, int step, FILE *f);
void AddRouteStamps(DOMNodeList* SetEltModule, DOMNodeList* SetEltFilter, DOMNodeList* SetEltSynchronizer,
		DOMNodeList* SetEltRoutingNode, DOMNodeList* SetEltConnection,
		ArrayDeamon *ListDeamon, int step, FILE *f);
void AddStart(DOMNodeList* SetElt, FILE *f);

int ElementarySourceCheck(std::string CId, DOMElement* ObjectId, ModuleArray& LocateModule,
		FilterArray& LocateFilter, SyncArray& LocateSync);

int ElementaryDestCheck(std::string CId, DOMElement* ObjectId, ModuleArray& LocateModule,
		FilterArray& LocateFilter, SyncArray& LocateSync);



void  ConnectionCheck(ModuleArray& LocateModule, FilterArray& LocateFilter, SyncArray& LocateSync,
		RoutingArray& LocateRouting, DOMNodeList* SetEltConnection, char* source,
		char* dest);

void RoutingUpdate(RoutingArray& LocateRouting);

void ParserCommands(char * InstGraphFile, char * OutputFile);
void ParserCommands(DOMDocument* InstGraphDoc, DOMDocument* OutputDoc);

};};

#endif

/*! \mainpage
 *
 * \section intro Introduction
 *
 * This is the implementation of the FlowVR controller. This particular module is dedicated
 * to the launching of the instanciated modules, the routing table definition and the
 * application reconfiguration on the fly. It uses 3 XML input files which describe respectively
 * the meta modules, the application graph and the instanciation
 * graph.
 *
 * \section parser1 ParserScript
 *
 * From the meta modules description and the application graph, this
 * parser builds the launching scripts to execute the application.
 *
 * \section parser2 ParserCommands
 *
 * The Instanciation graph is supposed to be valid and conform with
 * the imposed DTD. Then the ParserCommands generates the orders sent
 * to deamons in order to initialize the application. These orders are
 * of three types : AddObject to declare a module, a filter or a
 * synchronizer, AddRoute to build the routing table or Start to
 * initialize an object.
 *
 */
