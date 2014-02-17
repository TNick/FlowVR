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
 * File: src/ParserCommands.cpp                                    *
 *                                                                 *
 * Contacts:                                                       *
 *  12/10/2003 Sophie Robert <Sophie.Robert@lifo.univ-orleans.fr>  *
 *                                                                 *
 ******************************************************************/
/*! \file ParserCommands.cpp
 *
 * \brief The parser functions called by the controller to generate the XML commands
 *
 */

#include "flowvr/parse/ParserCommands.h"
#include <list>

namespace flowvr { namespace parse {

static bool printObject, printRoute, printStart;
void PassOptions(bool object, bool route, bool start)
{
	printObject = object;
	printRoute = route;
	printStart = start;
}

/*! \fn void AddObjectModule(ModuleArray& result, DOMNodeList* SetEltModule, FILE *f)
 *
 * \brief to construct the xml node AddObject in case of module object
 * \param SetEltModule : the list of all module objects
 * \param f : the open file to write corresponding orders
 * \return the order with flowvr.plugins.Regulator as default regulator
 * \return the module set as a map of module class element
 */
void AddObjectModule(ModuleArray& result, DOMNodeList* SetEltModule, DOMNode *root)
{

	ModuleArray& LocateModule = result;

	int NbModule = SetEltModule->getLength();

	for (int i=0; i<NbModule; i++) {

		Module* Register = new Module();

		DOMElement* CurrentMod = (DOMElement*)SetEltModule->item(i);

		std::string OTmp = CurrentMod->getAttributeNode("host")->getValue();
		Register->setHost(OTmp);
		if( printObject ) {
			DOMElement* dest = new DOMElement("dest");
			DOMText* text = new DOMText(OTmp);
			dest->LinkEndChild(text);
			root->LinkEndChild(dest);
			//fprintf(f,"<dest>%s</dest>\n",OTmp.c_str());
		}
		
		
		OTmp = CurrentMod->getAttributeNode("id")->getValue();
		Register->setIdentifier(OTmp);
		if( printObject ) {
			DOMElement* addObject = new DOMElement("addobject");
			addObject->SetAttribute("id", OTmp);
			addObject->SetAttribute("class", "flowvr.plugins.Regulator");
			root->LinkEndChild(addObject);
			//      fprintf(f, "<addobject id=\"%s\" class=\"flowvr.plugins.Regulator\" />\n",OTmp.c_str());

			std::string OParameters;
			DOMNode* parameters_ = NULL;
			DOMNodeList *FilterParameters = CurrentMod->getElementsByTagName("parameters");
			if (FilterParameters->getLength() != 0) {
				const DOMNode* ParametersNode = FilterParameters->item(0);
				parameters_ = ParametersNode->Clone();
				OParameters = DOMWriter::toString(ParametersNode);
			}
			delete FilterParameters;
			//addObject->LinkEndChild(parameters_);
		}

		

		DOMNodeList* Inputs = CurrentMod->getElementsByTagName("input");
		if (Inputs->getLength() != 0)
		{
			DOMNodeList* Ports = ((DOMElement*)Inputs->item(0))->getElementsByTagName("port");
			int NbPort = Ports->getLength();
			for (int k=0; k<NbPort; k++)
			{
				Port* CurrentPort = new Port();
				DOMElement* PortEl = (DOMElement*)Ports->item(k);
				std::string PId = PortEl->getAttributeNode("id")->getValue();
				CurrentPort->setIdentifier(PId);

				DOMNodeList* Type = PortEl->getElementsByTagName("datatype");
				int size = Type->getLength();
				std::string TextContent;
				bool full = false;
				if (size!=0)
				{
                                    full = true;
					DOMElement* DataType = (DOMElement*) Type->item(0);
					TextContent = DataType->getTextContent();
				}
				delete Type;
				CurrentPort->setDataType(full,TextContent);

				Register->setInput(PId,CurrentPort);
			}
		}
		delete Inputs;

		DOMNodeList* Outputs = CurrentMod->getElementsByTagName("output");
		if (Outputs->getLength() != 0)
		{
			DOMNodeList* Ports = ((DOMElement*)Outputs->item(0))->getElementsByTagName("port");
			int NbPort = Ports->getLength();
			for (int k=0; k<NbPort; k++)
			{
				Port* CurrentPort = new Port;
				DOMElement* PortEl = (DOMElement*)Ports->item(k);
				std::string PId = PortEl->getAttributeNode("id")->getValue();
				CurrentPort->setIdentifier(PId);

				DOMNodeList* Type = PortEl->getElementsByTagName("datatype");
				std::string TextContent;
				int size = Type->getLength();
				bool full = false;
				if (size!=0)
				{
                                    full = true;
                                    DOMElement* DataType = (DOMElement*) Type->item(0);
                                    TextContent = DataType->getTextContent();
				}
				delete Type;
				CurrentPort->setDataType(full,TextContent);

				Register->setOutput(PId,CurrentPort);
			}
		}
		delete Outputs;


		LocateModule[OTmp] = Register;
	}
}


/*! \fn void AddObjectFilter(FilterArray& result, DOMNodeList* SetEltFilter, FILE *f)
 *
 * \brief to construct the xml node AddObject in case of filter object
 * \param SetEltFilter : the list of all filter objects
 * \param f : the open file to write corresponding orders
 * \return the order with the class and the parameters of this filter
 * \return the filter set as a map of filter class elements
 *
 */
void AddObjectFilter(FilterArray& result, DOMNodeList* SetEltFilter, DOMNode *root)
{
	FilterArray& LocateFilter = result;
	int NbFilter = SetEltFilter->getLength();

	for (int i=0; i<NbFilter; i++) {
		Filter* Register = new Filter();
		DOMElement* CurrentFilter = (DOMElement*)SetEltFilter->item(i);

		std::string OTmp = CurrentFilter->getAttributeNode("host")->getValue();
		Register->setHost(OTmp);

		if( printObject ) {
			DOMElement* dest = new DOMElement("dest");
			DOMText* text = new DOMText(OTmp);
			dest->LinkEndChild(text);
			root->LinkEndChild(dest);
			//fprintf(f,"<dest>%s</dest>\n",OTmp.c_str());
		}

		OTmp = CurrentFilter->getAttributeNode("id")->getValue();
		Register->setIdentifier(OTmp);

		if( printObject ) {
			DOMNodeList* FilterDescription = CurrentFilter->getElementsByTagName("filterclass");
			DOMElement* FDesc = (DOMElement*)FilterDescription->item(0);
			std::string OClass = FDesc->getTextContent();
			delete FilterDescription;

			std::string OParameters;
			DOMNode* parameters = NULL;
			DOMNodeList *FilterParameters = CurrentFilter->getElementsByTagName("parameters");
			if (FilterParameters->getLength() != 0) {
				const DOMNode* ParametersNode = FilterParameters->item(0);
				parameters = ParametersNode->Clone();
				OParameters = DOMWriter::toString(ParametersNode);
			}
			delete FilterParameters;

			if (OParameters.empty())
			{
				DOMElement* addObject = new DOMElement("addobject");
				addObject->SetAttribute("id", OTmp);
				addObject->SetAttribute("class", OClass);
				root->LinkEndChild(addObject);
				delete parameters;

				//	fprintf(f, "<addobject id=\"%s\" class=\"%s\" />\n"
				//		,OTmp.c_str(),OClass.c_str());
			}
			else
			{
				DOMElement* addObject = new DOMElement("addobject");
				addObject->SetAttribute("id", OTmp);
				addObject->SetAttribute("class", OClass);
		//		DOMNode* parameters = FilterParameters->item(0);//new DOMText(OParameters);
				addObject->LinkEndChild(parameters);
				root->LinkEndChild(addObject);

				//	fprintf(f, "<addobject id=\"%s\" class=\"%s\">%s</addobject>\n"
				//		,OTmp.c_str(),OClass.c_str(),OParameters.c_str());
			}
		}

		DOMNodeList* Inputs = CurrentFilter->getElementsByTagName("input");
		if (Inputs->getLength() != 0) {
			DOMNodeList* Ports = ((DOMElement*)Inputs->item(0))->getElementsByTagName("port");
			int NbPort = Ports->getLength();
			for (int k=0; k<NbPort; k++) {
				Port* CurrentPort = new Port();
				DOMElement* PortEl = (DOMElement*)Ports->item(k);
				std::string PId = PortEl->getAttributeNode("id")->getValue();
				CurrentPort->setIdentifier(PId);

				DOMNodeList* Type = PortEl->getElementsByTagName("datatype");
				int size = Type->getLength();
				std::string TextContent;
				bool full = false;
				if (size!=0)
				{
					full = true;
					DOMElement* DataType = (DOMElement*) Type->item(0);
					TextContent = DataType->getTextContent();
				}
				delete Type;
				CurrentPort->setDataType(full,TextContent);

				Register->setInput(PId,CurrentPort);
			}
			delete Ports;
		}
		delete Inputs;

		DOMNodeList* Outputs = CurrentFilter->getElementsByTagName("output");
		if (Outputs->getLength() != 0) {
			DOMNodeList* Ports = ((DOMElement*)Outputs->item(0))->getElementsByTagName("port");
			int NbPort = Ports->getLength();
			for (int k=0; k<NbPort; k++) {
				Port* CurrentPort = new Port();
				DOMElement* PortEl = (DOMElement*)Ports->item(k);
				std::string PId = PortEl->getAttributeNode("id")->getValue();
				CurrentPort->setIdentifier(PId);

				DOMNodeList* Type = PortEl->getElementsByTagName("datatype");
				int size = Type->getLength();
				std::string TextContent;
				bool full = false;
				if (size!=0)
				{
					full = true;
					DOMElement* DataType = (DOMElement*) Type->item(0);
					TextContent = DataType->getTextContent();
				}
				delete Type;
				CurrentPort->setDataType(full,TextContent);

				Register->setOutput(PId,CurrentPort);
			}
			delete Ports;
		}
		delete Outputs;

		LocateFilter[OTmp] = Register;
	}
}

/*! \fn void AddObjectSynchronizer(SyncArray& result, DOMNodeList* SetEltSynchronizer, FILE *f)
 *
 * \brief to construct the xml node AddObject in case of synchronizer object
 * \param SetEltSynchronizer : the list of all synchronizer objects
 * \param f : the open file to write corresponding orders
 * \return the order with the class and the parameters of this synchronizer
 * \return the synchronizer set as a map of synchronizer class elements
 *
 */
void AddObjectSynchronizer(SyncArray& result, DOMNodeList* SetEltSynchronizer, DOMNode *root)
{
	SyncArray& LocateSync = result;
	int NbSynchronizer = SetEltSynchronizer->getLength();

	for (int i=0; i<NbSynchronizer; i++) {
		Synchronizer* Register = new Synchronizer();
		DOMElement* CurrentSync = (DOMElement*)SetEltSynchronizer->item(i);

		std::string OTmp = CurrentSync->getAttributeNode("host")->getValue();
		Register->setHost(OTmp);
		if( printObject ) {
			DOMElement* dest = new DOMElement("dest");
			DOMText* text = new DOMText(OTmp);
			dest->LinkEndChild(text);
			root->LinkEndChild(dest);

			//fprintf(f,"<dest>%s</dest>\n",OTmp.c_str());
		}

		OTmp = CurrentSync->getAttributeNode("id")->getValue();
		Register->setIdentifier(OTmp);

		if( printObject ) {
			DOMNodeList* SyncDescription = CurrentSync->getElementsByTagName("synchronizerclass");
			DOMElement* FDesc = (DOMElement*)SyncDescription->item(0);
			std::string OClass = FDesc->getTextContent();
			delete SyncDescription;

			std::string OParameters;
			DOMNode* parameters = NULL;
			DOMNodeList *SyncParameters = CurrentSync->getElementsByTagName("parameters");
			if (SyncParameters->getLength() != 0) {
				const DOMNode* ParametersNode = SyncParameters->item(0);
				parameters = ParametersNode->Clone();
				OParameters = DOMWriter::toString(ParametersNode);
			}
			delete SyncParameters;
			/*
				if (OParameters.empty())
				fprintf(f, "<addobject id=\"%s\" class=\"%s\" />\n"
				,OTmp.c_str(),OClass.c_str());
				else
				fprintf(f, "<addobject id=\"%s\" class=\"%s\">%s</addobject>\n"
				,OTmp.c_str(),OClass.c_str(),OParameters.c_str());
				*/

			if (OParameters.empty())
			{
				DOMElement* addObject = new DOMElement("addobject");
				addObject->SetAttribute("id", OTmp);
				addObject->SetAttribute("class", OClass);
				root->LinkEndChild(addObject);
				delete parameters;
			}
			else
			{
				DOMElement* addObject = new DOMElement("addobject");
				addObject->SetAttribute("id", OTmp);
				addObject->SetAttribute("class", OClass);
	//			DOMNode* parameters = SyncParameters->item(0);
				addObject->LinkEndChild(parameters);
				root->LinkEndChild(addObject);
			}

		}
		DOMNodeList* Inputs = CurrentSync->getElementsByTagName("input");
		if (Inputs->getLength() != 0) {
			DOMNodeList* Ports = ((DOMElement*)Inputs->item(0))->getElementsByTagName("port");
			int NbPort = Ports->getLength();
			for (int k=0; k<NbPort; k++) {
				Port* CurrentPort = new Port();
				DOMElement* PortEl = (DOMElement*)Ports->item(k);
				std::string PId = PortEl->getAttributeNode("id")->getValue();
				CurrentPort->setIdentifier(PId);

				DOMNodeList* Type = PortEl->getElementsByTagName("datatype");
				if (Type->getLength() != 0)
					std::cerr << "Warning : Synchronizer "<<OTmp<<" has input port "<<PId<<" with a datatype."<<std::endl;
				delete Type;
				CurrentPort->setDataType(false, "");

				Register->setInput(PId,CurrentPort);
			}
			delete Ports;
		}
		delete Inputs;

		DOMNodeList* Outputs = CurrentSync->getElementsByTagName("output");
		if (Outputs->getLength() != 0) {
			DOMNodeList* Ports = ((DOMElement*)Outputs->item(0))->getElementsByTagName("port");
			int NbPort = Ports->getLength();
			for (int k=0; k<NbPort; k++) {
				Port* CurrentPort = new Port();
				DOMElement* PortEl = (DOMElement*)Ports->item(k);
				std::string PId = PortEl->getAttributeNode("id")->getValue();
				CurrentPort->setIdentifier(PId);

				DOMNodeList* Type = PortEl->getElementsByTagName("datatype");
				if (Type->getLength()!=0)
					std::cerr << "Warning : Synchronizer "<<OTmp<<" has output port "<<PId<<" with a datatype."<<std::endl;
				delete Type;
				CurrentPort->setDataType(false, "");

				Register->setOutput(PId,CurrentPort);
			}
			delete Ports;
		}
		delete Outputs;

		LocateSync[OTmp] = Register;
	}
}

/*! \fn std::string DefSource(DOMElement* CurrentId, bool isSourceTypeStamps,
 *		      bool isActionTypeStamps, std::string* Result,
 *		      RoutingArray& LocateRouting)
 *
 * \brief The source DOMElement definition in a AddRoute order
 * \param CurrentId : The DOMElement of the FlowVR source object in a connection
 * \param isSourceTypeStamps to separate full and stamp messages from the source
 * \param isActionTypeStamps to know action message type
 * \param Identifier to clean the unuseful AddRoute order.
 * \return the XML description of a source in a AddRoute order.
 */
DOMNode* DefSource(DOMElement* CurrentId, bool isSourceTypeStamps,
		bool isActionTypeStamps, std::string* Id,
		RoutingArray& LocateRouting)
{
	std::string SourceNodeName = CurrentId->getNodeName();
	*Id = CurrentId->getAttributeNode("id")->getValue();

	DOMElement* Result = new DOMElement("source");
	Result->SetAttribute("id", *Id);
	//  *Result += "<source id=\""+Id+"\"";

	if ( (SourceNodeName=="moduleid") || (SourceNodeName=="filterid") || (SourceNodeName=="synchronizerid") ) {
		std::string PId = CurrentId->getAttributeNode("port")->getValue();
		//   *Result += " port=\""+PId+"\"";
		Result->SetAttribute("port", PId);
		*Id += "." + PId;
	}
	/*
		if (isSourceTypeStamps)
	 *Result += " messagetype=\"stamps\" />";
	 else
	 *Result += " messagetype=\"full\" />";
	 */
	Result->SetAttribute("messagetype", isSourceTypeStamps?"stamps":"full");
	return Result;
}

/*! \fn std::string DefActionBuffer(std::string destId, std::string destPort,
 *			    std::string* Result, bool isTypeStamps)
 *
 * \brief The construction of the action DOMElement in a AddRoute order
 * \param destId : the identifier of the destination object (should be a module, filter or synchronizer)
 * \param destPort : the destination input port
 * \param Identifier : routing node description
 * \param isTypeStamps : in order to manage the connectionstamps and the message type.
 * \return the XML description of the action with the buffer method in a AddRoute order.
 */
DOMNode* DefActionBuffer(std::string destId, std::string destPort,
		bool isTypeStamps)
{
	DOMElement* Result = new DOMElement("action");
	Result->SetAttribute("id", destId);
	//  *Result += "<action id=\""+destId+"\"";
	/*  if (isTypeStamps)
	 *Result += " messagetype=\"stamps\">";
	 else
	 *Result += " messagetype=\"full\">";
	 */
	Result->SetAttribute("messagetype", isTypeStamps?"stamps":"full");
	DOMElement* port = new DOMElement("port");
	DOMText* portDest = new DOMText(destPort);
	port->LinkEndChild(portDest);
	Result->LinkEndChild(port);
	//  *Result += "<port>"+destPort+"</port>";
	// *Result += "</action>";
	return Result;

	//  return  destId + "." + destPort;
}

/*! \fn void DefActionNet(std::string HostName, bool isTypeStamps, std::string* Result)
 *
 * \brief The construction of the action DOMElement in a AddRoute order
 * \param HostName the destination of the NET action
 * \param isTypeStamps the message type in an connectionstamps action.
 * \return the XML description of the action with the /NET method in a AddRoute order.
 */
DOMNode* DefActionNet(std::string HostName, bool isTypeStamps)
{
	DOMElement* Result = new DOMElement("action");
	Result->SetAttribute("id", "/NET");
	//  *Result += "<action id=\"/NET\"";
	/*  if (isTypeStamps)
	 *Result += " messagetype=\"stamps\">";
	 else
	 *Result += " messagetype=\"full\">";
	 */
	Result->SetAttribute("messagetype", (isTypeStamps ? "stamps" : "full"));
	DOMElement* dest = new DOMElement("dest");
	DOMText* host = new DOMText(HostName);
	dest->LinkEndChild(host);
	Result->LinkEndChild(dest);
	//  *Result += "<dest>"+HostName+"</dest>";
	//  *Result += "</action>";
	return Result;
}



/*! \fn AddRoute(ModuleArray& Module, FilterArray& Filter, SyncArray& Synchronizer, RoutingArray& Routing, DOMNodeList* SetEltConnection, DOMNodeList* SetEltConnectionStamps, ArrayDeamon* ListDeamon, int* iteration, FILE *f)
 *
 * \brief Build all the AddRoute components of the commands file
 * \param Module : set of all modules declared in the instantiation graph
 * \param Filter : set of all filters declared in the instantiation graph
 * \param Synchronizer : set of all Synchronizers declared in the instantiation graph
 * \param Routing : routing information at each routingnode(stamps)
 * \param SetEltConnection : set of connections declared in the instantiation graph
 * \param SetEltConnectionStamps : set of connectionstamps declared in the instantiation graph
 * \param ListDeamon : routing information for each deamon (on each host machine)
 * \param iteration : connection counter (used to label the with a unique name)
 * \param f : output commands file
 */
void AddRoute(ModuleArray& Module, FilterArray& Filter, SyncArray& Synchronizer,
		RoutingArray& Routing, DOMNodeList* SetEltConnection, DOMNodeList* SetEltConnectionStamps,
		ArrayDeamon* ListDeamon, int* iteration, DOMNode* root)
{
	if (!printRoute) return;
	bool isconnectionStamps;
	int NbConnection = SetEltConnection->getLength();
	int NbConnectionStamps = SetEltConnectionStamps->getLength();
	int NbConnectionTotal = NbConnection + NbConnectionStamps;
	bool isSourceTypeStamps;
	bool isActionTypeStamps;

	//std::vector<DOMElement*> terminalConnections; // connections whose destination is a module, a filter or a synchroniser
	//std::vector<DOMElement*> intermedConnections; // intermediary connections

	//int nbTerminalConnections;
	//int nbIntermedConnections;
	std::list< DOMElement* > TerminalConnections;
	std::map< std::string, DOMElement* > RoutingNodeConnections;
	std::map< std::string, DOMElement* > RoutingNodeStampsConnections;

	//std::cerr << "SortRoute"<<std::endl;
	// Separate terminal connections from intermediate ones :
	for (int i=0; i<NbConnectionTotal; i++) {
		DOMElement* CurrentConnection;

		isconnectionStamps = (i >= NbConnection);

		DOMNodeList* SetEltDestination;
		if (isconnectionStamps) {
			CurrentConnection = (DOMElement*)SetEltConnectionStamps->item(i-NbConnection);
			SetEltDestination = CurrentConnection->getElementsByTagName("destinationstamps");
		}
		else {
			CurrentConnection = (DOMElement*)SetEltConnection->item(i);
			SetEltDestination = CurrentConnection->getElementsByTagName("destination");
		}


		// Get destination object type (module, filter, synchroniser, routingnode or routingnodestamps) :
		DOMElement* CurrentDestination = (DOMElement*)SetEltDestination->item(0);
		DOMElement* CurrentDestinationId = (DOMElement*)CurrentDestination->getFirstChild();
		std::string DId = CurrentDestinationId->getAttributeNode("id")->getValue();
		std::string DestinationNodeName = CurrentDestinationId->getNodeName();
		delete SetEltDestination;

		// Connection is terminal only if the destination is a module, a filter or a synchroniser :
		if ( (DestinationNodeName=="moduleid") || (DestinationNodeName=="filterid") || (DestinationNodeName=="synchronizerid") )
		{
			TerminalConnections.push_back(CurrentConnection);
		}
		else if (DestinationNodeName=="routingnodeid")
			RoutingNodeConnections[DId] = CurrentConnection;
		else
			RoutingNodeStampsConnections[DId] = CurrentConnection;
	}

	//nbTerminalConnections = terminalConnections.size();
	//nbIntermedConnections = intermedConnections.size();

	//std::cerr << "CreateRoute"<<std::endl;
	for (std::list<DOMElement*>::iterator i=TerminalConnections.begin();
			i != TerminalConnections.end(); i++) {
		// Get a pointer to current connection :
		DOMElement* CurrentConnection = *i;

		// Determine if connection is of type "full" or "stamps" :
		std::string ConnectionType;
		ConnectionType = CurrentConnection->getNodeName();
		isconnectionStamps = (ConnectionType == "connectionstamps");

		DOMElement* CurrentSourceId; // Source description ; will correspond to the true source when reached the first connection of the path
		std::string SourceNodeName; // Source object type (module, filer,...) ; will correspond to the true source too
		std::string SId;
		ConnectionStep *entireConnection = 0; // whole path leading to current terminal connection

		bool foundPreviousConnection = true;
		while (foundPreviousConnection) {

			// Get destination :
			DOMNodeList* SetEltDestination;
			if (isconnectionStamps)
				SetEltDestination = CurrentConnection->getElementsByTagName("destinationstamps");
			else
				SetEltDestination = CurrentConnection->getElementsByTagName("destination");
			DOMElement* CurrentDestination = (DOMElement*)SetEltDestination->item(0);
			DOMElement* CurrentDestinationId = (DOMElement*)CurrentDestination->getFirstChild();
			std::string DId = CurrentDestinationId->getAttributeNode("id")->getValue();
			std::string DestinationNodeName = CurrentDestinationId->getNodeName();
			delete SetEltDestination;

			// Get destination host :
			std::string DestinationHostName;
			if (DestinationNodeName=="moduleid")
				DestinationHostName = Module[DId]->getHost();
			else if (DestinationNodeName=="filterid")
				DestinationHostName = Filter[DId]->getHost();
			else if (DestinationNodeName=="synchronizerid")
				DestinationHostName = Synchronizer[DId]->getHost();
			else
				DestinationHostName = Routing[DId]->getHost();

			// Get destination port :
			DOMAttr* PortDest;
			std::string PortDestId = "";
			if ((DestinationNodeName != "routingnodeid") && (DestinationNodeName != "routingnodestampsid")) {
				PortDest = CurrentDestinationId->getAttributeNode("port");
				if (PortDest)
					PortDestId = PortDest->getValue();
			}

			// Get the source for this connection :
			DOMNodeList *SetEltSource;
			if (isconnectionStamps)
				SetEltSource = CurrentConnection->getElementsByTagName("sourcestamps");
			else
				SetEltSource = CurrentConnection->getElementsByTagName("source");
			DOMElement* CurrentSource = (DOMElement*)SetEltSource->item(0);
			CurrentSourceId = (DOMElement*)CurrentSource->getFirstChild();
			DOMAttr* SourceId = CurrentSourceId->getAttributeNode("id");
			SId = SourceId->getValue();
			SourceNodeName = CurrentSourceId->getNodeName();
			delete SetEltSource;

			// Get source host :
			std::string SourceHostName;
			if (SourceNodeName=="moduleid")
				SourceHostName = Module[SId]->getHost();
			else if (SourceNodeName=="filterid")
				SourceHostName = Filter[SId]->getHost();
			else if (SourceNodeName=="synchronizerid")
				SourceHostName = Synchronizer[SId]->getHost();
			else
				SourceHostName = Routing[SId]->getHost();

			// Get source port :
			DOMAttr* PortSource;
			std::string PortSourceId = "";
			if ((SourceNodeName != "routingnodeid") && (SourceNodeName != "routingnodestampsid")) {
				PortSource = CurrentSourceId->getAttributeNode("port");
				if (PortSource)
					PortSourceId = PortSource->getValue();
			}

			// Determine source message type :
			if (isconnectionStamps)
			{
				if (SourceNodeName=="moduleid") {
					if (PortSourceId!="")
						isSourceTypeStamps = !Module[SId]->OutputDataType(PortSourceId);
 				}
				else if (SourceNodeName=="filterid") {
					if (PortSourceId!="")
						isSourceTypeStamps = !Filter[SId]->OutputDataType(PortSourceId);
				}
				else if (SourceNodeName=="synchronizerid") {
					if (PortSourceId!="")
						isSourceTypeStamps = !Synchronizer[SId]->OutputDataType(PortSourceId);
				}
				else if (SourceNodeName == "routingnodeid")
					isSourceTypeStamps = false;
				else // SourceNodeName != "routingnodestampsid"
					isSourceTypeStamps = true;
			}
			else
				isSourceTypeStamps = false; // A connection get necessary a full message

			// The destination is determined by the connection type :
			isActionTypeStamps = isconnectionStamps;

			// Initiate connection path by terminal connection :
			if ( (DestinationNodeName=="moduleid") || (DestinationNodeName=="filterid") || (DestinationNodeName=="synchronizerid") ) {

				// terminal connection is subdivided if source host is different from destination host :
				if (SourceHostName != DestinationHostName) {
					entireConnection = new ConnectionStep(DestinationHostName, DestinationHostName, isActionTypeStamps, isActionTypeStamps);
					entireConnection->SetNewFirstConnection(SourceHostName, isSourceTypeStamps);
				}
				else
					entireConnection = new ConnectionStep(SourceHostName, DestinationHostName, isSourceTypeStamps, isActionTypeStamps);

				entireConnection->SetDestinationInformation(DestinationNodeName, DId, PortDestId);
			}
			else if (entireConnection)
				entireConnection->SetNewFirstConnection(SourceHostName, isSourceTypeStamps);


			foundPreviousConnection = false;

			// Search for previous connection if source is a routingnode(stamps) :
			if (SourceNodeName == "routingnodeid") {
				//std::cerr << "Searching for connection to routing node "<<SId<<std::endl;
				std::map<std::string,DOMElement*>::iterator prev = RoutingNodeConnections.find(SId);
				if (prev != RoutingNodeConnections.end())
				{
					CurrentConnection = prev->second;
					ConnectionType = CurrentConnection->getNodeName();
					isconnectionStamps = (ConnectionType == "connectionstamps");
					DestinationNodeName = "routingnodeid";
					//std::cerr << "Found "<<CurrentConnection->Attribute("id")<<std::endl;
					foundPreviousConnection = true;
				}
				//else std::cerr << "NOT FOUND"<<std::endl;

			}
			else if (SourceNodeName == "routingnodestampsid") {
				//std::cerr << "Searching for connection to routing node stamps "<<SId<<std::endl;
				std::map<std::string,DOMElement*>::iterator prev = RoutingNodeStampsConnections.find(SId);
				if (prev != RoutingNodeStampsConnections.end())
				{
					CurrentConnection = prev->second;
					ConnectionType = CurrentConnection->getNodeName();
					isconnectionStamps = (ConnectionType == "connectionstamps");
					DestinationNodeName = "routingnodestampsid";
					//std::cerr << "Found "<<CurrentConnection->Attribute("id")<<std::endl;
					foundPreviousConnection = true;
				}
				//else std::cerr << "NOT FOUND"<<std::endl;
			}
		}

		// Delete non necessary connections :
		if (entireConnection)
			entireConnection->Simplify();

		// Initiate AddRoute commands construction :

		ConnectionStep *currentConnectionStep;
		if (entireConnection)
			currentConnectionStep = entireConnection->GetActiveTerminalConnectionStep();
		else
			currentConnectionStep = 0;

		bool isTerminalConnection = true;

		// The source must be a module, filter ou synchronizer (if not there is an error in the instantiation graph) :
		if ( (SourceNodeName=="moduleid") || (SourceNodeName=="filterid") || (SourceNodeName=="synchronizerid") ) {
			while (currentConnectionStep) {

				// Build connection identifier :
				std::string nom = "com";
				char increment[10];
				sprintf(increment, "%d", *iteration);
				(*iteration)++;
				nom += increment;

				// Select which routing table should be updated :
				bool isDeamonNew;
				Deamon* CurrentDeamon;
				std::map<std::string, Deamon*>::iterator index = (*ListDeamon).find(currentConnectionStep->sourceHost);
				isDeamonNew = (index == (*ListDeamon).end());
				if (isDeamonNew) {
					CurrentDeamon = new Deamon;
					CurrentDeamon->setHost(currentConnectionStep->sourceHost);
					(*ListDeamon)[currentConnectionStep->sourceHost] = CurrentDeamon;
				}
				else
					CurrentDeamon = (index->second);

				std::string BufId, NetId; // Daemon routing descripters
				//	std::string AddRoute = "<addroute id=\""+nom+"\">";

				DOMElement* AddRoute = new DOMElement("addroute");
				AddRoute->SetAttribute("id", nom);

				DOMNode* Source = DefSource(CurrentSourceId,
						currentConnectionStep->isSourceTypeStamps,
						currentConnectionStep->isActionTypeStamps,
						&NetId,
						Routing);
				AddRoute->LinkEndChild(Source);
				bool test;

				// Connection via a buffer for the terminal connection :
				if (isTerminalConnection) {
					BufId = entireConnection->destId + "." + entireConnection->destPort;
					DOMNode* route =  DefActionBuffer(entireConnection->destId,
							entireConnection->destPort,
							currentConnectionStep->isActionTypeStamps);
					//	  AddRoute += "</addroute>";
					AddRoute->LinkEndChild(route);
					test = CurrentDeamon->setBuffer(BufId, AddRoute);
					isTerminalConnection = false;
				}
				else {
					NetId += ".";
					NetId += currentConnectionStep->destHost;
					if (currentConnectionStep->isActionTypeStamps)
						NetId += ".stamps";
					else
						NetId += ".full";

					DOMNode* route = DefActionNet(currentConnectionStep->destHost,
							currentConnectionStep->isActionTypeStamps);
					//  AddRoute += "</addroute>";
					AddRoute->LinkEndChild(route);

					test = CurrentDeamon->setNet(NetId, AddRoute);
				}

				if (test) (*iteration)--;

				currentConnectionStep = currentConnectionStep->GetPreviousConnectionStep();
			}
		}
		else if (entireConnection)
		{
			std::cerr << "Error : Connection to "<<entireConnection->destId<<':'<<entireConnection->destPort<<" ends at routing node "<<SId<<std::endl;
		}

		// Free path :
		if (entireConnection)
			delete entireConnection;

		entireConnection = 0;
	}

	//std::cerr << "WriteRoute"<<std::endl;
	// Write all AddRoute to the file :
	if( printRoute ) {
		std::map<std::string, Deamon*>::iterator itDeamons;
		Deamon *curDeamon;
		for (itDeamons=(*ListDeamon).begin(); itDeamons!=(*ListDeamon).end(); itDeamons++) {
			curDeamon = (Deamon*)(itDeamons->second);

			DOMElement* dest = new DOMElement("dest");
			DOMText* text = new DOMText(curDeamon->getHost());
			dest->LinkEndChild(text);
			root->LinkEndChild(dest);

			//      fprintf(f,"<dest>%s</dest>\n",curDeamon->getHost().c_str());

			//std::string text;
			// Write /NET AddRoute :
			/*text = */curDeamon->getActionNets(root);
			//fprintf(f,"%s",text.c_str());
			// Write buffer AddRoute :
			/*text = */curDeamon->getActionBuffers(root);
			//fprintf(f,"%s",text.c_str());
		}
	}
}


/*! \fn void AddStart(DOMNodeList* SetElt, FILE *f)
 *
 * \brief to construct the xml node Action in case of module objects
 * \param SetElt : the list of objects (module or filter ...)
 * \param f : the open file to write corresponding orders
 * \return the start orders
 *
 */
void AddStart(DOMNodeList* SetElt, DOMNode *root)
{
	if( !printStart ) return;
	int NbElt = SetElt->getLength();
	for (int i=0; i<NbElt; i++) {
		DOMElement* CurrentElt = (DOMElement*)SetElt->item(i);

		DOMElement* dest = new DOMElement("dest");
		DOMText* text = new DOMText(CurrentElt->Attribute("host"));
		dest->LinkEndChild(text);
		root->LinkEndChild(dest);

		//    fprintf(f,"<dest>%s</dest>\n",CurrentElt->Attribute("host"));
		DOMElement* action = new DOMElement("action");
		action->SetAttribute("id", CurrentElt->Attribute("id"));
		DOMElement* start = new DOMElement("start");
		action->LinkEndChild(start);
		root->LinkEndChild(action);
		//    fprintf(f,"<action id=\"%s\"><start /></action>\n",CurrentElt->Attribute("id"));
	}
}

/*! \fn int ElementarySourceCheck(std::string CId, DOMElement* ObjectId, ModuleArray& LocateModule,
  FilterArray& LocateFilter, SyncArray& LocateSync)
 * \brief to control a source of an XML connection node
 *
 * \param CId : the connection identifier to locate the error
 * \param ObjectId : the source or sourcestamps node
 * \param LocateModule : the module map to check the existence of the source in case of module
 * \param LocateFilter : the filter map
 * \param LocateSync : the synchronizer map
 * \return NbError : the number of found errors
 */
int ElementarySourceCheck(std::string CId, DOMElement* ObjectId, ModuleArray& LocateModule,
		FilterArray& LocateFilter, SyncArray& LocateSync, DOMElement* context)
{
	DOMAttr* Tmp;
	std::string OTmp;
	int NbError = 0;

	std::string Id = ObjectId->getAttributeNode("id")->getValue();
	std::string Name = ObjectId->getNodeName();

	if (Name=="moduleid")
	{
		ModuleArray::iterator index = LocateModule.find(Id);
		if (index != LocateModule.end()) {
			Module* Current = index->second;

			Tmp = ObjectId->getAttributeNode("host");
			if (Tmp != NULL) {
				OTmp = Tmp->getValue();
				if (OTmp != Current->getHost()) {
					NbError++;
					std::cerr << "Connection " << CId << " ERROR : source host " << OTmp << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
				}
			}

			Tmp = ObjectId->getAttributeNode("port");
			OTmp = Tmp->getValue();
			if (!(Current->OutputExistence(OTmp))) {
				NbError++;
				std::cerr << "Connection " << CId << " ERROR : source port " << OTmp << " undeclared IN "<<DOMWriter::toString(context)<<std::endl;
			}
		}
		else {
			NbError++;
			std::cerr << "Connection " << CId << " ERROR : source identifier " << Id << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
		}
	}

	else if (Name=="filterid")
	{
		FilterArray::iterator index = LocateFilter.find(Id);
		if (index != LocateFilter.end()) {
			Filter* Current = index->second;

			Tmp = ObjectId->getAttributeNode("host");
			if (Tmp != NULL) {
				OTmp = Tmp->getValue();
				if (OTmp != Current->getHost()) {
					NbError++;
					std::cerr << "Connection " << CId << " ERROR : source host " << OTmp << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
				}
			}

			Tmp = ObjectId->getAttributeNode("port");
			OTmp = Tmp->getValue();
			if (!(Current->OutputExistence(OTmp))) {
				NbError++;
				std::cerr << "Connection " << CId << " ERROR : source port " << OTmp << " undeclared IN "<<DOMWriter::toString(context)<<std::endl;
			}
		}
		else {
			NbError++;
			std::cerr << "Connection " << CId << " ERROR : source identifier " << Id << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
		}
	}

	else if (Name=="synchronizerid")
	{
		SyncArray::iterator index = LocateSync.find(Id);
		if (index != LocateSync.end()) {
			Synchronizer* Current = index->second;

			Tmp = ObjectId->getAttributeNode("host");
			if (Tmp != NULL) {
				OTmp = Tmp->getValue();
				if (OTmp != Current->getHost()) {
					NbError++;
					std::cerr << "Connection " << CId << " ERROR : source host " << OTmp << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
				}
			}

			Tmp = ObjectId->getAttributeNode("port");
			OTmp = Tmp->getValue();
			if (!(Current->OutputExistence(OTmp))) {
				NbError++;
				std::cerr << "Connection " << CId << " ERROR : source port " << OTmp << " undeclared IN "<<DOMWriter::toString(context)<<std::endl;
			}
		}
		else {
			NbError++;
			std::cerr << "Connection " << CId << " ERROR : source identifier " << Id << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
		}
	}

	return NbError;
}

/*! \fn int ElementaryDestCheck(std::string CId, DOMElement* ObjectId, ModuleArray& LocateModule,
  FilterArray& LocateFilter, SyncArray& LocateSync)
 * \brief to control a destination of an XML connection node
 *
 * \param CId : the connection identifier to locate the error
 * \param ObjectId : the destination or destinationstamps node of the connection
 * \param LocateModule : the module map to check the existence of the destination in case of module
 * \param LocateFilter : the filter map
 * \param LocateSync : the synchronizer map
 * \return NbError : the number of found errors
 */
int ElementaryDestCheck(std::string CId, DOMElement* ObjectId, ModuleArray& LocateModule,
		FilterArray& LocateFilter, SyncArray& LocateSync, DOMElement* context)
{
	DOMAttr* Tmp;
	std::string OTmp;
	int NbError = 0;

	std::string Id = ObjectId->getAttributeNode("id")->getValue();
	std::string Name = ObjectId->getNodeName();

	if (Name=="moduleid")
	{
		ModuleArray::iterator index = LocateModule.find(Id);
		if (index != LocateModule.end()) {
			Module* Current = index->second;

			Tmp = ObjectId->getAttributeNode("host");
			if (Tmp != NULL) {
				OTmp = Tmp->getValue();
				if (OTmp != Current->getHost()) {
					NbError++;
					std::cerr << "Connection " << CId << " ERROR : destination host " << OTmp << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
				}
			}

			Tmp = ObjectId->getAttributeNode("port");
			if (Tmp == NULL)
			{
				NbError++;
				std::cerr << "Connection " << CId << " ERROR : no destination port IN "<<DOMWriter::toString(context)<<std::endl;
			}
			else
			{
				OTmp = Tmp->getValue();
				if (!(Current->InputExistence(OTmp))) {
					NbError++;
					std::cerr << "Connection " << CId << " ERROR : destination port " << OTmp << " undeclared IN "<<DOMWriter::toString(context)<<std::endl;
				}
			}
		}
		else {
			NbError++;
			std::cerr << "Connection " << CId << " ERROR : destination identifier " << Id << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
		}
	}

	else if (Name=="filterid")
	{
		FilterArray::iterator index = LocateFilter.find(Id);
		if (index != LocateFilter.end()) {
			Filter* Current = index->second;

			Tmp = ObjectId->getAttributeNode("host");
			if (Tmp != NULL) {
				OTmp = Tmp->getValue();
				if (OTmp != Current->getHost()) {
					NbError++;
					std::cerr << "Connection " << CId << " ERROR : destination host " << OTmp << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
				}
			}

			Tmp = ObjectId->getAttributeNode("port");

			Tmp = ObjectId->getAttributeNode("port");
			if (Tmp == NULL)
			{
				NbError++;
				std::cerr << "Connection " << CId << " ERROR : no destination port IN "<<DOMWriter::toString(context)<<std::endl;
			}
			else
			{
				OTmp = Tmp->getValue();
				if (!(Current->InputExistence(OTmp))) {
					NbError++;
					std::cerr << "Connection " << CId << " ERROR : destination port " << OTmp << " undeclared IN "<<DOMWriter::toString(context)<<std::endl;
				}
			}
		}
		else {
			NbError++;
			std::cerr << "Connection " << CId << " ERROR : destination identifier " << Id << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
		}
	}

	else if (Name=="synchronizerid")
	{
		SyncArray::iterator index = LocateSync.find(Id);
		if (index != LocateSync.end()) {
			Synchronizer* Current = index->second;

			Tmp = ObjectId->getAttributeNode("host");
			if (Tmp != NULL) {
				OTmp = Tmp->getValue();
				if (OTmp != Current->getHost()) {
					NbError++;
					std::cerr << "Connection " << CId << " ERROR : destination host " << OTmp << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
				}
			}

			Tmp = ObjectId->getAttributeNode("port");

			Tmp = ObjectId->getAttributeNode("port");
			if (Tmp == NULL)
			{
				NbError++;
				std::cerr << "Connection " << CId << " ERROR : no destination port IN "<<DOMWriter::toString(context)<<std::endl;
			}
			else
			{
				OTmp = Tmp->getValue();
				if (!(Current->InputExistence(OTmp))) {
					NbError++;
					std::cerr << "Connection " << CId << " ERROR : destination port " << OTmp << " undeclared IN "<<DOMWriter::toString(context)<<std::endl;
				}
			}
		}
		else {
			NbError++;
			std::cerr << "Connection " << CId << " ERROR : destination identifier " << Id << " invalid IN "<<DOMWriter::toString(context)<<std::endl;
		}
	}

	return NbError;
}


/*! \fn void ConnectionCheck(ModuleArray& LocateModule, FilterArray& LocateFilter, SyncArray& LocateSync,
  RoutingArray& LocateRouting, DOMNodeList* SetEltConnection, char* source, char* dest)
 * \brief A connection check on the existence of the source and destination including its features
 * (identifier, host, port).
 * \param LocateModule : the module map which correspond to the modules of the network
 * \param LocateFilter : the filter map
 * \param LocateSync : the synchronizer map
 * \param LocateRouting : the routingnode map
 * \param SetEltConnection : all the connection nodes of the network
 * \param source : to separate connection node and connectionstamps node
 * \param dest : to separate connection node and connectionstamps node
 */

void ConnectionCheck(ModuleArray& LocateModule, FilterArray& LocateFilter, SyncArray& LocateSync,
		RoutingArray& LocateRouting, DOMNodeList* SetEltConnection, char* source, char* dest)
{
	int NbConnection = SetEltConnection->getLength();

	int NbError = 0;

	for (int i=0; i<NbConnection; i++)
	{
		DOMElement* CurrentConnection = (DOMElement*)SetEltConnection->item(i);

		DOMAttr* ConId = CurrentConnection->getAttributeNode("id");
		std::string CId = ConId->getValue();
		DOMNodeList* Sources = CurrentConnection->getElementsByTagName(source);
		DOMElement* SourceNode = (DOMElement*)(Sources->item(0));
		delete Sources;
		DOMElement* ObjectId = (DOMElement*) SourceNode->getFirstChild();
		std::string SourceName = ObjectId->getNodeName();
		std::string SId;
		std::string DId;
		std::string PortId;

		if ((SourceName != "routingnodeid") && (SourceName != "routingnodestampsid"))
		{
			PortId = ObjectId->getAttributeNode("port")->getValue();
			SId = ObjectId->getAttributeNode("id")->getValue();
			int Error = ElementarySourceCheck(CId, ObjectId, LocateModule, LocateFilter, LocateSync, CurrentConnection);
			NbError += Error;
		}
		else
		{
			SId = ObjectId->getAttributeNode("id")->getValue();

			RoutingArray::const_iterator index = LocateRouting.find(SId);
			if (index != LocateRouting.end()) {
				RoutingNode* Current = index->second;
				DOMAttr* Tmp = ObjectId->getAttributeNode("host");
				if (Tmp != NULL) {
					std::string OTmp = Tmp->getValue();
					if (OTmp != Current->getHost()) {
						NbError++;
						std::cerr << "Connection " << CId << " ERROR : source host " << OTmp << " invalid \n";
					}
				}
				if (ObjectId->getFirstChild() != NULL) {
					DOMElement* TrueSource = (DOMElement*)ObjectId->getFirstChild();
					int Error = ElementarySourceCheck(CId, TrueSource, LocateModule, LocateFilter, LocateSync, CurrentConnection);
					NbError+=Error;
				}
			}
			else {
				NbError++;
				std::cerr << "Connection " << CId << " ERROR : source identifier " << SId << " invalid \n";
			}
		}

		DOMNodeList* Dests = CurrentConnection->getElementsByTagName(dest);
		DOMElement* DestNode=(DOMElement*)(Dests->item(0));
		delete Dests;
		ObjectId = (DOMElement*) DestNode->getFirstChild();
		std::string DestName = ObjectId->getNodeName();

		if ((DestName != "routingnodeid") && (DestName != "routingnodestampsid")) {
			int Error = ElementaryDestCheck(CId, ObjectId, LocateModule, LocateFilter, LocateSync, CurrentConnection);
			NbError += Error;
		}
		else {
			DId = ObjectId->getAttributeNode("id")->getValue();
			RoutingArray::const_iterator index = LocateRouting.find(DId);
			if (index != LocateRouting.end()) {
				RoutingNode* Current = index->second;
				DOMAttr* Tmp = ObjectId->getAttributeNode("host");
				if (Tmp != NULL) {
					std::string OTmp = Tmp->getValue();
					if (OTmp != Current->getHost()) {
						NbError++;
						std::cerr << "Connection " << CId << " ERROR : destination host " << OTmp << " invalid \n";
					}
				}

				if ((SourceName != "routingnodeid") && (SourceName != "routingnodestampsid"))
					LocateRouting[DId]->setTrueSource(SId,PortId,SourceName);
			}
			else {
				NbError++;
				std::cerr << "Connection " << CId << " ERROR : destination identifier " << DId << " invalid \n";
			}
		}

		if (NbError > 10) exit(1);

		if (DestName=="routingnodestampsid") LocateRouting[DId]->setType(0);
		if (SourceName=="routingnodestampsid") LocateRouting[SId]->setType(0);

		if ((DestName=="routingnodeid") || (DestName=="routingnodestampsid"))
			if ((SourceName=="routingnodeid") || (SourceName=="routingnodestampsid"))
				LocateRouting[DId]->setPrevNode(SId);
	}

	if (NbError != 0) exit(1);
}


/*! \fn void RoutingUpdate(RoutingArray& LocateRouting)
 *
 * \brief This function allows to define if a routing node generates a NET
 *        addroute with full or stamps message.
 * \param LocateRouting the routing node map.
 * \return the full attribute of the routing node class.
 */
void RoutingUpdate(RoutingArray& LocateRouting)
{
	RoutingArray::iterator index;
	/*  for (index = LocateRouting.begin(); index != LocateRouting.end(); index++) {
		 if (!((index->second)->getType())) {
		 std::string Host = (index->second)->getHost();
		 std::string PrevId = (index->second)->getPrevNode();
		 bool boucle = 1;
		 while (boucle) {
		 if (PrevId == "null") boucle = 0;
		 else if (LocateRouting[PrevId]->getHost()!=Host) {
		 boucle = 0;
		 }
		 else PrevId = LocateRouting[PrevId]->getPrevNode();
		 }
		 if (PrevId != "null")
		 (index->second)->setFull(0);
		 }
		 }*/
	for (index = LocateRouting.begin(); index != LocateRouting.end(); index++) {
		std::string Host = (index->second)->getHost();
		std::string PrevId = (index->second)->getPrevNode();
		std::string Prev;
		bool boucle = 1;
		bool stop = 1;
		bool exst = 0;
		while (stop) {
			if (PrevId == "null") stop = 0;
			else {
				exst = 1;
				Prev = PrevId;
				if (LocateRouting[PrevId]->getHost()!=Host) boucle = 0;
				PrevId = LocateRouting[PrevId]->getPrevNode();
			}
		}
		if (exst)
			(index->second)->setTrueSource(LocateRouting[Prev]->getTrueSource(),
					LocateRouting[Prev]->getPortSource(),
					LocateRouting[Prev]->getSourceType());

		if (!((index->second)->getType()))
			if (!boucle) (index->second)->setFull(0);
	}
}

/*! \fn void ParserCommands(char * InstGraphFile, char * OutputFile)
 *  \brief The XML orders from the controller to the deamons
 *  \param InstGraphFile : the XML file of the network
 *  \param OutputFile : in case of an file output instead of a stdout output.
 *  \return a file of these XML orders
 */
void ParserCommands(char * InstGraphFile, char * OutputFile)
{

	DOMDocument* OutputDoc = new DOMDocument(OutputFile);

	XercesDOMParser *InstGraphParser = new XercesDOMParser;
	InstGraphParser->parse(InstGraphFile);

	DOMDocument *InstGraphDoc = InstGraphParser->getDocument();
	ParserCommands(InstGraphDoc, OutputDoc);

	OutputDoc->SaveFile();
	delete OutputDoc;
}


void ParserCommands(DOMDocument* InstGraphDoc, DOMDocument* OutputDoc)
{
	// XML header
	DOMDeclaration* header = new DOMDeclaration("1.0", "ISO-8859-1", "yes");
	OutputDoc->LinkEndChild(header);
	DOMNode* root = new DOMElement("commands");
	OutputDoc->LinkEndChild(root);
	//fputs("<!DOCTYPE commands SYSTEM \"http://flowvr.sf.net/flowvr-parse/dtd/commands.dtd\" >\n",f);

	//std::cerr << "AddObjectModule"<<std::endl;

	// XML orders for regulator
	DOMNodeList *SetEltModule = InstGraphDoc->getElementsByTagName("module");
	ModuleArray LocateModule;
	AddObjectModule(LocateModule, SetEltModule,root);

	//std::cerr << "AddObjectFilter"<<std::endl;

	// XML orders for filter
	DOMNodeList *SetEltFilter = InstGraphDoc->getElementsByTagName("filter");
	FilterArray LocateFilter;
	AddObjectFilter(LocateFilter, SetEltFilter,root);

	//std::cerr << "AddObjectSynchronizer"<<std::endl;

	// XML orders for synchronizer
	DOMNodeList *SetEltSynchronizer = InstGraphDoc->getElementsByTagName("synchronizer");
	SyncArray LocateSync;
	AddObjectSynchronizer(LocateSync, SetEltSynchronizer,root);

	if( printObject ) {
		DOMElement* flush = new DOMElement("flush");
		root->LinkEndChild(flush);
	}


	int iteration = 0;

	//std::cerr << "AddRoutingNode"<<std::endl;

	DOMNodeList *SetEltRoutingNode = InstGraphDoc->getElementsByTagName("routingnode");

	RoutingArray LocateRouting;
	int NbRoutingNode = SetEltRoutingNode->getLength();
	for (int k=0; k<NbRoutingNode; k++) {
		RoutingNode* CurrentRouting = new RoutingNode();
		DOMElement* Current = (DOMElement*) SetEltRoutingNode->item(k);
		DOMAttr* Tmp = Current->getAttributeNode("id");
		std::string OTmp = Tmp->getValue();
		CurrentRouting->setIdentifier(OTmp);
		Tmp = Current->getAttributeNode("host");
		CurrentRouting->setHost(Tmp->getValue());
		//CurrentRouting->setType(1);
		LocateRouting[OTmp] = CurrentRouting;
	}

	//std::cerr << "AddRoutingNodeStamps"<<std::endl;

	SetEltRoutingNode = InstGraphDoc->getElementsByTagName("routingnodestamps");

	NbRoutingNode = SetEltRoutingNode->getLength();
	for (int k=0; k<NbRoutingNode; k++) {
		RoutingNode* CurrentRouting = new RoutingNode();
		DOMElement* Current = (DOMElement*) SetEltRoutingNode->item(k);
		DOMAttr* Tmp = Current->getAttributeNode("id");
		std::string OTmp = Tmp->getValue();
		CurrentRouting->setIdentifier(OTmp);
		Tmp = Current->getAttributeNode("host");
		CurrentRouting->setHost(Tmp->getValue());
		//CurrentRouting->setType(0);
		LocateRouting[OTmp] = CurrentRouting;
	}


	DOMNodeList *SetEltConnection = InstGraphDoc->getElementsByTagName("connection");
	DOMNodeList *SetEltConnectionStamps = InstGraphDoc->getElementsByTagName("connectionstamps");

	//std::cerr << "CheckConnection"<<std::endl;
	ConnectionCheck(LocateModule, LocateFilter, LocateSync, LocateRouting, SetEltConnection, (char*) "source", (char*) "destination");
	//std::cerr << "CheckConnectionStamps"<<std::endl;
	ConnectionCheck(LocateModule, LocateFilter, LocateSync, LocateRouting, SetEltConnectionStamps, (char*) "sourcestamps", (char*) "destinationstamps");


	//std::cerr << "UpdateRouting"<<std::endl;
	RoutingUpdate(LocateRouting);

	ArrayDeamon ListDeamon;

	//std::cerr << "AddRoute"<<std::endl;
	AddRoute(LocateModule, LocateFilter, LocateSync, LocateRouting, SetEltConnection, SetEltConnectionStamps, &ListDeamon, &iteration, root);


	/*
		ArrayDeamon::iterator index;
		for (index=ListDeamon.begin(); index!=ListDeamon.end(); index++)
		(index->second)->Print();*/

	if( printRoute ) {
		DOMElement* flush = new DOMElement("flush");
		root->LinkEndChild(flush);
	}

	// Start orders
	//std::cerr << "AddStart"<<std::endl;
	AddStart(SetEltModule,root);
	AddStart(SetEltFilter,root);
	AddStart(SetEltSynchronizer,root);

	if( printStart ) {
		DOMElement* flush = new DOMElement("flush");
		root->LinkEndChild(flush);
	}

	//std::cerr << "END"<<std::endl;
	delete SetEltModule;
	delete SetEltFilter;
	delete SetEltSynchronizer;
	delete SetEltConnection;
	delete SetEltConnectionStamps;
	delete SetEltRoutingNode;

}

};}; // end namespace 
