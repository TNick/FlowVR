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

#ifndef APPLICATIONTRACER_H_
#define APPLICATIONTRACER_H_

#include <string>
#include <vector>
#include <getopt.h>
#include "flowvr/xml.h"
#include "flowvr/app/core/msg.h"
#include "flowvr/app/core/component.h"
#include "flowvr/app/core/applicationTracer.h"

namespace appTracer{

using namespace std;
class Object;

class applicationTracer
{

public:

    /**
     * \brief constructor
     * \param appName_ : the name of the application
     */
	applicationTracer(std::string appName_) : appName(appName_){};


    /**
     * \brief Create intermediate command files : appName.prolog.xml & appName.epilog.xml
     * \param netFile : the name of the network file : appName.net.xml
     */
	int createIntermediateFiles(std::string netFile, std::string traceResultPrefix);


    /**
     * \brief Recursively inspect the main component of the application, and populate a list of hosts to be logged
     * \param app : the main component of the application
     * \param HostsToTrace : the list containing the hosts to be logged
     */
	int inspectApp(flowvr::app::Composite* app, std::list<std::string> &HostsToTrace);

    /**
     * \brief Add objects needed to trace application in the network file
     * \param fNet : a pointer to the main node of the network xml description
     * \param MaListe_ : a list containing host's name
     */
	int addLoggersNetXML(flowvr::xml::DOMDocument &fNet, std::list<std::string> MaListe_);

    /**
     * \brief Add objects needed to trace application in the run file
     * \param fRun : a pointer to the main node of the run xml description
     * \param MaListe_ : a list containing host's name
     * \param traceResultPrefix
     */
	int addLoggersRunXML(flowvr::xml::DOMDocument &fRun, std::list<std::string> MaListe_, std::string traceResultPrefix);

	int addLoggersADLXml(flowvr::xml::DOMDocument &fAdl, std::list<std::string> MaListe_);

private:
	std::string appName; ///< the name of the current FlowVR Application


    /**
     * \brief Create appName.epilog.xml file, using the network description (appName.net.xml)
     * \param filename : the net.xml file.
     */
	bool createEpilog(const char* filename);

    /**
     * \brief Create appName.prolog.xml file, using the network description (appName.net.xml)
     * \param filename : the net.xml file.
     */
	bool createProlog(const char* filename);

    /**
     * \brief Create appName.gltrace.xml file, used by the "flowvr-gltrace" executable to visualize traces.
     * \param filename : the net.xml file.
     */
	bool createGltrace(const char* filename, std::string TraceResultPrefix);

    /**
     * \brief search for constrained connections between src and dest
     * \param src : the source of the connection
     * \param dest : the dest of the connection
     */
	bool hasConstraint(Object* src, Object* dest);

    /**
     * \brief parse the network file
     * \param filename : the network file
     */
	bool parse_network(const char* filename);

};

template<class T>
class MyList: public vector<T>
{
	T *current;

public:
	bool add(T t)
	{
		current = find(t);

		if (!current)
		{
			this->push_back(t);
			current = &this->back();
			return true;
		}

		return false;
	}

	T* find(T t)
	{
		for (unsigned int i = 0; i < this->size(); i++)
			if (this->at(i) == t)
				return &this->at(i);

		return NULL;
	}

	T* find(string ID)
	{
		for (unsigned int i = 0; i < this->size(); i++)
			if (this->at(i).getID() == ID)
				return &this->at(i);

		return NULL;
	}

	T* lastAdded()
	{
		return current;
	}

	MyList() : current(NULL){}
};

class HostToTrace
{
protected:
	std::string name;
	vector<string> logger;

public:
	std::string getID()
	{
		return name;
	}

	bool operator ==(const HostToTrace &other) const
	{
		return name == other.name;
	}

	std::string getName()
	{
		return name;
	}

	void addLogger(string log)
	{
		for (unsigned int i = 0; i < logger.size(); i++)
			if (logger[i] == log)
				return;

		logger.push_back(log);
	}

	flowvr::xml::DOMElement hostDesc()
	{
		flowvr::xml::DOMElement host = flowvr::xml::DOMElement("host");
		host.SetAttribute("id", name);
		host.SetAttribute("method", "FASTEST");

		for (unsigned int i = 0; i < logger.size(); i++)
		{
			flowvr::xml::DOMElement log = flowvr::xml::DOMElement("logger");
			log.SetAttribute("id", logger[i]);
			host.InsertEndChild(log);
		}

		return host;
	}

	HostToTrace(string n) : name(n){}
};

class Logger
{
protected:
	std::string name;
	std::string host;
	std::string prefix;

public:
	std::string getID()
	{
		return name;
	}

	bool operator ==(const Logger &other) const
	{
		return (name == other.name)/* and (host == other.host)*/;
	}

	std::string getName()
	{
		return name;
	}
	std::string getHost()
	{
		return host;
	}
	std::string getprefix()
	{
		return prefix;
	}

	std::string pingCmd(int id)
	{
		char buf[32];
		sprintf(buf, "%d", id);
		std::string ping = "<ping id=\"" + (string) buf + "\" logger=\"" + (string) name + "\" />";
		return ping;
	}

	flowvr::xml::DOMElement fileDesc(std::string traceResultPrefix)
	{
		flowvr::xml::DOMElement file = flowvr::xml::DOMElement("tracefile");
		file.SetAttribute("file", traceResultPrefix + host);
		return file;
	}

	Logger(string n, std::string h, std::string p) : name(n), host(h), prefix(p){}};

class Object
{
protected:
	std::string name;
	std::string host;
	std::string type;

public:
	std::string getID()
	{
		return name;
	}

	bool operator ==(const Object &other) const
	{
		return (name == other.name);
	}

	std::string getName()
	{
		return name;
	}
	std::string getHost()
	{
		return host;
	}
	std::string getType()
	{
		return type;
	}

	flowvr::xml::DOMElement objectDesc()
	{
		flowvr::xml::DOMElement object = flowvr::xml::DOMElement("object");

		if (type == "module")
			object.SetValue("module");
		if (type == "filter")
			object.SetValue("filter");
		if (type == "synchronizer")
			object.SetValue("synchronizer");

		object.SetAttribute("id", name);
		object.SetAttribute("host", host);

		flowvr::xml::DOMElement display = flowvr::xml::DOMElement("objdisplay");
		display.SetAttribute("active", "YES");

		flowvr::xml::DOMElement text = flowvr::xml::DOMElement("objtext");
		text.SetAttribute("text", name + ":" + host);
		text.SetAttribute("color", "WHITE");

		flowvr::xml::DOMElement line = flowvr::xml::DOMElement("objline");

		if (type == "module")
			line.SetAttribute("color", "GREEN");
		if (type == "filter")
		{
			line.SetAttribute("color", "ORANGE");
			line.SetAttribute("width", "thin");
		}
		if (type == "synchronizer")
		{
			line.SetAttribute("color", "PINK");
			line.SetAttribute("width", "thin");
		}

		display.InsertEndChild(text);
		display.InsertEndChild(line);
		object.InsertEndChild(display);
		return object;
	}

	Object(string n, std::string h, std::string t) : name(n), host(h), type(t){}
};

class Trace
{
protected:
	std::string object;
	std::string name;
	int id;
	std::string logger;
	std::string host;

public:
	std::string getID()
	{
		return (object + (string) ":" + name);
	}

	bool operator ==(const Trace &other) const
	{
		return (name == other.name) and (object == other.object);
	}

	std::string getobject()
	{
		return object;
	}
	std::string getname()
	{
		return name;
	}
	std::string gethost()
	{
		return host;
	}
	std::string getLogger()
	{
		return logger;
	}
	int getId()
	{
		return id;
	}

	std::string startCmd()
	{
		char buf[32];
		sprintf(buf, "%d", id);
		return "<action id=\"" + object + "\"><traceStart name=\"" + name + "\" id=\"" + buf + "\" logger=\"" + logger + "\"/></action>";
	}

	std::string stopCmd()
	{
		return "<action id=\"" + object + "\"><traceStop name=\"" + name + "\"/></action>";
	}

	flowvr::xml::DOMElement traceDesc()
	{
		flowvr::xml::DOMElement trace = flowvr::xml::DOMElement("trace");
		trace.SetAttribute("object", object);
		trace.SetAttribute("name", name);

		flowvr::xml::DOMElement shot = flowvr::xml::DOMElement("shot");
		shot.SetAttribute("id", id);

		trace.InsertEndChild(shot);
		return trace;
	}

	flowvr::xml::DOMElement eventDesc()
	{
		flowvr::xml::DOMElement event = flowvr::xml::DOMElement("event");
		event.SetAttribute("name", object + ":" + name);

		flowvr::xml::DOMElement trace = flowvr::xml::DOMElement("trace");
		trace.SetAttribute("object", object);
		trace.SetAttribute("name", name);

		flowvr::xml::DOMElement display = flowvr::xml::DOMElement("evtdisplay");
		display.SetAttribute("active", "YES");

		flowvr::xml::DOMElement line = flowvr::xml::DOMElement("evtline");
		if (name == "waitBegin")
			line.SetAttribute("color", "RED");
		else if (name == "waitEnd")
			line.SetAttribute("color", "GREEN");
		else if (name == "begintIt")
			line.SetAttribute("color", "WHITE");
		else if (name == "endIt")
			line.SetAttribute("color", "BLUE");
		else{
			line.SetAttribute("color", "YELLOW"); // TODO: differentiate between InputPort, OutputPort, and user events
		}

		display.InsertEndChild(line);
		event.InsertEndChild(trace);
		event.InsertEndChild(display);

		return event;
	}

	Trace(string o, std::string n, int i, std::string l, std::string h) : object(o), name(n), id(i), logger(l), host(h)	{}};

class Connection
{
	std::string name;
	std::string src;
	std::string dest;
	std::string type;
	bool constraint; ///< Add a rank constraint between src and dest
public:
	std::string getID()
	{
		return name;
	}

	bool operator ==(const Connection &other) const
	{
		return (name == other.name) or ((src == other.src) and (dest == other.dest));
	}

	std::string getName()
	{
		return name;
	}
	std::string getSource()
	{
		return src;
	}
	std::string getDestination()
	{
		return dest;
	}
	std::string getType()
	{
		return type;
	}
	bool getConstraint()
	{
		return constraint;
	}

	flowvr::xml::DOMElement linkDesc()
	{
		flowvr::xml::DOMElement link = flowvr::xml::DOMElement("link");
		link.SetAttribute("name", name);

		flowvr::xml::DOMElement source = flowvr::xml::DOMElement("source");
		source.SetAttribute("name", src);

		flowvr::xml::DOMElement destination = flowvr::xml::DOMElement("destination");
		destination.SetAttribute("name", dest);

		flowvr::xml::DOMElement match = flowvr::xml::DOMElement("match");
		if (type == "connection")
			match.InsertEndChild(flowvr::xml::DOMElement("SrcFromDest"));
		if (type == "connectionstamps")
			match.InsertEndChild(flowvr::xml::DOMElement("SrcFromDest"));
		if (type == "wait")
			match.InsertEndChild(flowvr::xml::DOMElement("DestFromSrc"));
		//    if(type == "latence")
		//      match.InsertEndChild( flowvr::xml::DOMElement("SrcFromDest") );

		flowvr::xml::DOMElement display = flowvr::xml::DOMElement("lnkdisplay");
		display.SetAttribute("active", "YES");

		flowvr::xml::DOMElement line = flowvr::xml::DOMElement("line");
		if (type == "connection")
		{
			line.SetValue("lnkline");
			line.SetAttribute("srccolor", "YELLOW");
			line.SetAttribute("destcolor", "BLUE");
		}
		if (type == "connectionstamps")
		{
			line.SetValue("lnkline");
			line.SetAttribute("srccolor", "DARK_YELLOW");
			line.SetAttribute("destcolor", "DARK_BLUE");
		}
		if (type == "wait")
		{
			line.SetValue("objline");
			line.SetAttribute("color", "RED");
		}
		//    if(type == "latence")
		//    {
		//      line.SetValue("objline");
		//      line.SetAttribute("color","GRAY");
		//      line.SetAttribute("width","thin");
		//    }

		display.InsertEndChild(line);
		link.InsertEndChild(source);
		link.InsertEndChild(destination);
		link.InsertEndChild(match);
		link.InsertEndChild(display);

		return link;
	}

	Connection(string n, std::string s, std::string d, std::string t, bool c = false) : name(n), src(s), dest(d), type(t), constraint(c){}};
}
#endif /* APPLICATIONTRACER_H_ */
