/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
 * INRIA                                                           *
 * ALL RIGHTS RESERVED.	                                           *
 *                                                                 *
 * This source is covered by the GNU LGPL, please refer to the     *
 * COPYING file for further information.                           *
 *                                                                 *
 *-----------------------------------------------------------------*
 *                                                                 *
 *  Original Contributors:                                         *
 *    Jeremie Allard,                                              *
 *    Thomas Arcila,                                               *
 *    Jean-Denis Lesage.                                           *
 *    Clement Menier,                                              *
 *    Bruno Raffin                                                 *
 *                                                                 *
 *******************************************************************
 *                                                                 *
 *       File : flowvr/app/core/applicationTracer.h                *
 *                                                                 *
 *                                                                 *
 *       Contact : Antoine Vanel                                   *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/applicationTracer.h"
#include "flowvr/app/components/connection.comp.h"
#include "flowvr/app/components/filterlogger.comp.h"

#include <flowvr/components/modulefwrite.comp.h>
#include <flowvr/components/metamodulefwrite.comp.h>

#include "flowvr/app/components/metamodule.comp.h"
#include "flowvr/app/core/run.h"

using namespace appTracer;

MyList<appTracer::Trace> mytracelist;
MyList<Object> myobjectlist;
MyList<appTracer::Connection> myconnectionlist;
MyList<HostToTrace> myhostlist;
MyList<Logger> myloggerlist;

bool applicationTracer::parse_network(const char* filename)
{

	flowvr::xml::DOMParser parser;
	parser.parse(filename);

	flowvr::xml::DOMDocument* doc = parser.getDocument();

	if (doc->Error())
	{
		std::cerr << "Non valid network file " << filename << ":" << doc->ErrorDesc() << std::endl;
		return false;
	}

	flowvr::xml::DOMElement* network = doc->FirstChildElement("network");
	if (network == NULL)
	{
		std::cerr << "root tag 'network' not present in " << filename << std::endl;
		return false;
	}

	flowvr::xml::DOMElement* child = network->FirstChildElement();
	if (child == NULL)
	{
		std::cerr << "network tag is empty in " << filename << std::endl;
		return false;
	}

	// First of all look for loggers
	flowvr::xml::DOMElement* logger = network->FirstChildElement("filter");
	while (logger)
	{
		std::cerr << "Check " << logger->Attribute("id") << "\n";
		if (strstr(logger->Attribute("id"), "logger") != NULL)
		{
			myloggerlist.add(Logger(logger->Attribute("id"), logger->Attribute("host"), appName.c_str()));
			//	  std::cerr << "Add logger "  << logger->Attribute("id")<< "\n";

		}
		logger = logger->NextSiblingElement("filter");
	}

	//Now look for traces
	int traceid = 0;
	while (child)
	{
		if (!strcmp(child->Value(), "module") or !strcmp(child->Value(), "filter") or !strcmp(child->Value(), "synchronizer"))
		{
			const char* objectname = child->Attribute("id");
			const char* hostname = child->Attribute("host");
			const char* loggername = NULL;

			if (!(objectname and hostname))
			{
				std::cerr << "error in module definition\n";
				return false;
			}
			// look for the logger  on that host
			for (unsigned int i = 0; i < myloggerlist.size(); i++)
			{
				if (myloggerlist[i].getHost() == hostname)
				{
					loggername = myloggerlist[i].getName().c_str();
					break;
				}
			}
			if (loggername == NULL)
			{
				std::cerr << "Error: No logger on host " << hostname << "\n";
				return false;
			}

			// now go through traces
			bool first = true;
			flowvr::xml::DOMElement* tracelist = child->FirstChildElement("tracelist");
			while (tracelist)
			{
				flowvr::xml::DOMElement* trace = tracelist->FirstChildElement("trace");
				while (trace)
				{
					const char* tracename = trace->Attribute("id");

					if (!(tracename))
					{
						std::cerr << "object '" << objectname << "': trace with no name find\n";
						return false;
					}

					if (first)
					{
						if (!myobjectlist.add(Object(objectname, hostname, child->Value())))
						{
							std::cerr << "object '" << myobjectlist.lastAdded()->getID() << "' defined twice\n";
							return false;
						}
						myhostlist.add(HostToTrace(hostname));

						if (!strcmp(child->Value(), "module"))
						{
							myconnectionlist.add(Connection(objectname + (string) ":wait", objectname + (string) ":waitBegin",	objectname + (string) ":waitEnd", "wait"));
						}
						if( !strcmp(child->Value(), "filter") )
						{
						  //myconnectionlist.add( Connection(objectname+(string)":wait", objectname+(string)":in", objectname+(string)":out",  "wait") );
						}
						first = false;
					}

					if (!mytracelist.add(Trace(objectname, tracename, ++traceid, loggername, hostname)))
					{
						std::cerr << "trace '" << mytracelist.lastAdded()->getID() << "' defined twice\n";
						return false;
					}
					myhostlist.lastAdded()->addLogger(loggername);

					trace = trace->NextSiblingElement();
				}
				tracelist = tracelist->NextSiblingElement("tracelist");
			}
		}
		child = child->NextSiblingElement();
	}
	child = network->FirstChildElement();
	while(child){
		if (!strcmp(child->Value(), "connection") or !strcmp(child->Value(), "connectionstamps"))
		{
			const char* name = child->Attribute("id");
			if (!name)
			{
				std::cerr << "error in " << child->Value() << " definition\n";
				return false;
			}

			flowvr::xml::DOMElement* source = child->FirstChildElement();
			if (!source)
			{
				std::cerr << "in connection '" << name << "': source tag not present\n";
				return false;
			}

			flowvr::xml::DOMElement* destination = source->NextSiblingElement();
			if (!destination)
			{
				std::cerr << "in connection '" << name << "': destination tag not present\n";
				return false;
			}

			source = source->FirstChildElement();
			destination = destination->FirstChildElement();
			if (!source or !destination)
			{
				std::cerr << "in connection '" << name << "': error in source or destination tag\n";
				return false;
			}

//			if (!strcmp(source->Value(), "routingnodeid"))
//			{
//				source = source->FirstChildElement();
//				if (!source)
//				{
//					std::cerr << "in connection '" << name << "': error in source->routingnodeid tag\n";
//					return false;
//				}
//			}

			if (strcmp(destination->Value(), "routingnodeid"))
			{
				const char* objsrc = source->Attribute("id");
				const char* portsrc = source->Attribute("port");
				const char* objdest = destination->Attribute("id");
				const char* portdest = destination->Attribute("port");

				if (!objsrc or !portsrc or !objdest or !portdest)
				{
					std::cerr << "in connection '" << name << "': error in source or destination tag's attributes\n";
					return false;
				}

				const char* style = child->Attribute("style");
				bool constraint = (style == NULL || !strstr(style, "constraint=false"));

				Trace* tr_src = mytracelist.find(objsrc + (string) ":" + portsrc);
				Trace* tr_dest = mytracelist.find(objdest + (string) ":" + portdest);

				if ((tr_src != NULL) and (tr_dest != NULL))
				{
					if (!myconnectionlist.add(Connection(name, tr_src->getID(), tr_dest->getID(), child->Value(), constraint)))
					{
						std::cerr << "connection '" << myconnectionlist.lastAdded()->getID() << "' define twice\n";
						return false;
					}
				}
			}
		}

		child = child->NextSiblingElement();
	}

return true;
}

/// Search for constrained connection linking src to dest and return true if found
bool applicationTracer::hasConstraint(Object* src, Object* dest)
{
	if (src == NULL || dest == NULL)
		return false;
	std::string ssrc = src->getID() + ':';
	std::string sdest = dest->getID() + ':';
	for (unsigned int i = 0; i < myconnectionlist.size(); i++)
	{
		Connection& c = myconnectionlist[i];
		if (c.getConstraint() && c.getSource().substr(0, ssrc.size()) == ssrc && c.getDestination().substr(0, sdest.size()) == sdest)
			return true;
	}
	return false;
}

bool applicationTracer::createGltrace(const char* filename, std::string traceResultPrefix)
{
	FILE* file;
	file = fopen(filename, "w");
	if (file == NULL)
	{
		std::cerr << "ERROR: Can't open or create gltrace-config file '" << filename << "'\n";
		return false;
	}

	// Compute an object order respecting connection constraints

	std::vector<Object*> objects;
	objects.resize(myobjectlist.size());
	for (unsigned int i = 0; i < myobjectlist.size(); i++)
		objects[i] = &(myobjectlist[i]);

	for (unsigned int id = 0; id < objects.size(); id++)
	{
		// find in id+ the object without any source

		for (unsigned int m = id; m < objects.size(); m++)
		{
			unsigned int s = id;
			for (; s < objects.size(); s++)
				if (s != m && hasConstraint(objects[s], objects[m]))
					break;
			if (s == objects.size())
			{ // NO SOURCE
				Object* winner = objects[m];
				while (m > id)
				{
					objects[m] = objects[m - 1];
					--m;
				}
				objects[id] = winner;
				break;
			}
		}

		//std::cout << "Object " << id << ": " << objects[id]->getID() << std::endl;

	}

	flowvr::xml::DOMElement filelist = flowvr::xml::DOMElement("filelist");

	//ping result to be removed !!
	flowvr::xml::DOMElement ping = flowvr::xml::DOMElement("pingresults");
	ping.SetAttribute("file", "log-results");
	filelist.InsertEndChild(ping);
	for (unsigned int i = 0; i < myloggerlist.size(); i++)
		filelist.InsertEndChild(myloggerlist[i].fileDesc(traceResultPrefix));

	flowvr::xml::DOMElement hostlist = flowvr::xml::DOMElement("hostlist");
	for (unsigned int i = 0; i < myhostlist.size(); i++)
		hostlist.InsertEndChild(myhostlist[i].hostDesc());

	flowvr::xml::DOMElement objectlist = flowvr::xml::DOMElement("objectlist");
	for (unsigned int i = 0; i < objects.size(); i++)
		objectlist.InsertEndChild(objects[i]->objectDesc());

	flowvr::xml::DOMElement tracelist = flowvr::xml::DOMElement("tracelist");
	flowvr::xml::DOMElement eventlist = flowvr::xml::DOMElement("eventlist");
	for (unsigned int i = 0; i < mytracelist.size(); i++)
	{
		tracelist.InsertEndChild(mytracelist[i].traceDesc());
		eventlist.InsertEndChild(mytracelist[i].eventDesc());
	}

	flowvr::xml::DOMElement linklist = flowvr::xml::DOMElement("linklist");
	for (unsigned int i = 0; i < myconnectionlist.size(); i++)
		linklist.InsertEndChild(myconnectionlist[i].linkDesc());

	flowvr::xml::DOMElement gltrace = flowvr::xml::DOMElement("gltrace");
	gltrace.InsertEndChild(filelist);
	gltrace.InsertEndChild(hostlist);
	gltrace.InsertEndChild(objectlist);
	gltrace.InsertEndChild(tracelist);
	gltrace.InsertEndChild(eventlist);
	gltrace.InsertEndChild(linklist);

	// flowvr::xml::DOMElement header
	fputs("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n", file);
	fputs("<!DOCTYPE commands SYSTEM \"http://flowvr.sf.net/flowvr-parse/dtd/gltrace-config.dtd\" >\n", file);
	//  fputs("<commands>\n",file);


	gltrace.Print(file, 0);

	fclose(file);
	return true;
}

bool applicationTracer::createProlog(const char* filename)
{
	FILE* file;
	file = fopen(filename, "w");
	if (file == NULL)
	{
		std::cerr << "ERROR: Can't open or create gltrace-config file '" << filename << "'\n";
		return false;
	}

	// flowvr::xml::DOMElement header
	fputs("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n", file);
	fputs("<!DOCTYPE commands SYSTEM \"http://flowvr.sf.net/flowvr-parse/dtd/commands.dtd\" >\n", file);
	fputs("<commands>\n", file);

	// Delay before starting to trace the application
	//fprintf(file, "<wait duration=\"%ss\"/>\n", "2");

	std::string currenthost = "";
	for (unsigned int i = 0; i < mytracelist.size(); i++)
	{
		std::string host = mytracelist[i].gethost();
		if (host != currenthost)
		{
			fprintf(file, "<dest>%s</dest>\n", host.c_str());
			currenthost = host;
		}
		fprintf(file, "%s\n", mytracelist[i].startCmd().c_str());
	}
	fprintf(file, "<flush />\n");

	fclose(file);
	return true;
}

bool applicationTracer::createEpilog(const char* filename)
{
	FILE* file;
	file = fopen(filename, "w");
	if (file == NULL)
	{
		std::cerr << "ERROR: Can't open or create gltrace-config file '" << filename << "'\n";
		return false;
	}

	// flowvr::xml::DOMElement header
	fputs("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n", file);
	fputs("<!DOCTYPE commands SYSTEM \"http://flowvr.sf.net/flowvr-parse/dtd/commands.dtd\" >\n", file);
	fputs("<commands>\n", file);

	//  // Delay before starting to trace the application
	//  fprintf(file,"<wait duration=\"%ss\"/>\n",OptVal[OptDelay].c_str());
	//
	//
	//  std::string currenthost = "";
	//  for(unsigned int i=0; i<mytracelist.size(); i++)
	//  {
	//    std::string host = mytracelist[i].gethost();
	//    if( host != currenthost )
	//    {
	//      fprintf(file,"<dest>%s</dest>\n",host.c_str());
	//      currenthost = host;
	//    }
	//    fprintf(file,"%s\n",mytracelist[i].startCmd().c_str());
	//  }
	//  fprintf(file,"<flush />\n");
	//
	//
	//  fprintf(file,"<wait duration=\"%ss\"/>\n",OptVal[OptDuration].c_str());

	std::string currenthost = "";
	for (unsigned int i = 0; i < mytracelist.size(); i++)
	{
		std::string host = mytracelist[i].gethost();
		if (host != currenthost)
		{
			fprintf(file, "<dest>%s</dest>\n", host.c_str());
			currenthost = host;
		}
		fprintf(file, "%s\n", mytracelist[i].stopCmd().c_str());
	}
	fprintf(file, "<flush/>\n");

	for (unsigned int i = 0; i < myhostlist.size(); i++)
	{
		fprintf(file, "<dest>%s</dest>\n", myhostlist[i].getName().c_str());
		fprintf(file, "<killall/>\n");
	}
	//fprintf(file,"<wait duration=\"2s\"/>\n");

	//  int pingnb = atoi(OptVal[OptPingNb].c_str());
	//  for( unsigned int i=0; i<myloggerlist.size(); i++)
	//  {
	//    fprintf(file,"<dest>%s</dest>\n",myloggerlist[i].getHost().c_str());
	//    for(int id=0; id<pingnb; id++)
	//    {
	//      fprintf(file,"%s\n",myloggerlist[i].pingCmd(2).c_str());
	//    }
	//  }

	fclose(file);
	return true;
}

int applicationTracer::createIntermediateFiles(std::string netFile, std::string traceResultPrefix)
{

	flowvr::xml::DOMDocument* netXML = new flowvr::xml::DOMDocument(netFile);
	if (!netXML->LoadFile())
	{
		std::cout << netFile + " cannot be loaded. ";//, __FUNCTION_NAME__);
		return 1;
	}

	if (!parse_network(netFile.c_str()))
	{
		std::cerr << "ERROR while parsing xml network file '" << "'\n\n";
		return 1;
	}

	std::string gltraceFileName = appName + ".gltrace.xml";
	if (!createGltrace(gltraceFileName.c_str(), traceResultPrefix ))
		std::cerr << "ERROR while creating gltrace-config file '" << "'\n\n";

	std::string prologFileName = appName + ".prolog.xml";
	if (!createProlog(prologFileName.c_str()))
		std::cerr << "ERROR while creating prolog command file '" << "'\n\n";

	std::string epilogFileName = appName + ".epilog.xml";
	if (!createEpilog(epilogFileName.c_str()))
		std::cerr << "ERROR while creating epilog command file '" << "'\n\n";

	return 0;
}

/* this function will be moved in the trace part */
int applicationTracer::inspectApp(flowvr::app::Composite* app, std::list<std::string> &hostsToTrace)
{

	for (flowvr::app::Composite::pComponentIterator it0 = app->getComponentBegin(); it0 != app->getComponentEnd(); it0++)
	{
		if ((*it0)->isComposite())
		{
			inspectApp(((flowvr::app::Composite*) (*it0)), hostsToTrace);
		}

		else
		{

			for (HostList::const_iterator it = (*it0)->hostList.begin(); it != (*it0)->hostList.end(); it++)
			{
				if (find(hostsToTrace.begin(), hostsToTrace.end(), (*it).getName()) == hostsToTrace.end())
				{
					hostsToTrace.push_back((*it).getName());
				}
			}
		}

	}
	return 0;
}

int applicationTracer::addLoggersRunXML(flowvr::xml::DOMDocument &fRun, std::list<std::string> MaListe_, std::string traceResultPrefix)
{

	// add fwrite modules on each host
	for (std::list<std::string>::iterator it = MaListe_.begin(); it != MaListe_.end(); it++)
	{
		flowvr::modules::MetaModuleFWrite* Met = new flowvr::modules::MetaModuleFWrite("Met");

		Met->setId(appName + "/log/" + *it + "/fwrite");
		Host h(*it,"Daemon");
		Met->hostList.push_back(h);
		Met->setParameter("raw", "1");
		Met->setParameter("inFile", traceResultPrefix + "/" + *it);
		Met->configure();

		flowvr::xml::TiXmlNode* fRunNode = fRun.FirstChild("commands");
		fRunNode->InsertEndChild(*(Met->XMLRunBuild()));
	}
	fRun.SaveFile();
	return 0;
}

int applicationTracer::addLoggersADLXml(flowvr::xml::DOMDocument &fAdl, std::list<std::string> MaListe_)
{
	flowvr::xml::TiXmlNode* fAdlOut = fAdl.FirstChild("component");

	for (std::list<std::string>::iterator it = MaListe_.begin(); it != MaListe_.end(); it++)
	{
		flowvr::app::FilterLogger* myFilter = new flowvr::app::FilterLogger(appName + "/log/" + *it + "/logger");
		Host h(*it,"Daemon");
		myFilter->hostList.push_back(h);
		myFilter->setParameter("trace","0"); //Don't trace this module
		fAdl.InsertEndChild(*(myFilter->HierarchyXMLBuild()));
	}
	fAdl.SaveFile();
	return 0;
}

int applicationTracer::addLoggersNetXML(flowvr::xml::DOMDocument &fNet, std::list<std::string> MaListe_)
{

	flowvr::xml::TiXmlNode* fNetwork = fNet.FirstChild("network");

	for (std::list<std::string>::iterator it = MaListe_.begin(); it != MaListe_.end(); it++)
	{

		std::string host = *it;

		flowvr::app::FilterLogger* myFilter = new flowvr::app::FilterLogger(appName + "/log/" + host + "/logger");
		Host h(*it,"Daemon");
		myFilter->hostList.push_back(h);
        myFilter->setParameter("trace","0"); //Don't trace this module
		fNetwork->InsertEndChild(*(myFilter->XMLBuild()) );

		flowvr::modules::ModuleFWrite* Mod = new flowvr::modules::ModuleFWrite(appName + "/log/" + host + "/fwrite/singleton");
		Mod->hostList.push_back(h);
        Mod->setParameter("trace","0"); //Don't trace this module

		fNetwork->InsertEndChild(*(Mod->XMLBuild()));

		//addObjects connections
		flowvr::xml::DOMElement connection = flowvr::xml::DOMElement("connection");
		std::string idStrC = appName + "/log/" + host + "/logger";
		connection.SetAttribute("id", "autogenerated_com/" + host);

		flowvr::xml::DOMElement source = flowvr::xml::DOMElement("source");
		flowvr::xml::DOMElement filterid = flowvr::xml::DOMElement("filterid");
		filterid.SetAttribute("id", appName + "/log/" + host + "/logger");
		filterid.SetAttribute("port", "log");
		source.InsertEndChild(filterid);

		flowvr::xml::DOMElement destination = flowvr::xml::DOMElement("destination");
		flowvr::xml::DOMElement moduleid = flowvr::xml::DOMElement("moduleid");
		moduleid.SetAttribute("id", appName + "/log/" + host + "/fwrite/singleton");
		moduleid.SetAttribute("port", "in");
		destination.InsertEndChild(moduleid);

		connection.InsertEndChild(source);
		connection.InsertEndChild(destination);

		fNetwork->InsertEndChild(connection);

		fNet.SaveFile();

	}
	return 0;
}
