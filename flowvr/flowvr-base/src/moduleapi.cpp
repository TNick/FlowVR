/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                         Base Libraries                          *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA and                                                       *
* Laboratoire d'Informatique Fondamentale d'Orleans               *
* (FRE 2490). ALL RIGHTS RESERVED.                                *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
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
* File: src/moduleapi.cpp                                         *
*                                                                 *
* Contacts:                                                       *
*  12/10/2003 Jeremie Allard <Jeremie.Allard@imag.fr>             *
*                                                                 *
******************************************************************/
#include "flowvr/moduleapi.h"
#include "flowvr/moduleapifactory.h"
#include "flowvr/parallel.h"
#include "flowvr/stamp.h"
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <time.h>
#include <string.h>
#include <sstream>
#include <sys/stat.h>
//#include <stdlib.h>


namespace flowvr
{

// ############################################################################
// PORTS
// ############################################################################
Port::Port(const std::string& myname, StampList* mystamps, bool bOwn)
: name(myname)
, stamps(mystamps)
, module(NULL)
, privateInfo((void*)~0) // invalidate
, trace(TypedTrace<int>(myname))
, stampsOwned(false)
{
	if (stamps == NULL)
	{
		stamps = new StampList;
		stampsOwned = true;
	}
	else
	{
		// check for the "owns" flag for later cleanup
		stampsOwned = bOwn;
	}
}

Port::~Port()
{
	if(stampsOwned)
		delete stamps;
}

/// Check if this port is connected (only valid after the first wait).
bool Port::isConnected() const
{
	ModuleAPI* module = getModule();
	if (module == NULL)
		return false;
	else
		return module->isPortConnected(this);
}

InputPort::InputPort(const std::string& myname, StampList* mystamps, bool bOwn,
					 bool bIsNonBlockingPort)
: Port(myname, mystamps, bOwn)
, isNonBlocking(bIsNonBlockingPort)
{
}

OutputPort::OutputPort(const std::string& myname, StampList* mystamps, bool bOwn) :
	Port(myname, mystamps, bOwn)
{
}

/// Returns true.
bool InputPort::isInput() const
{
	return true;
}

/// Returns false.
bool InputPort::isOutput() const
{
	return false;
}

bool InputPort::isNonBlockingPort() const
{
	return isNonBlocking;
}

void InputPort::setNonBlockingFlag(bool bBlock)
{
	isNonBlocking = bBlock;
}

/// Returns false.
bool OutputPort::isInput() const
{
	return false;
}

/// Returns true.
bool OutputPort::isOutput() const
{
	return true;
}


// ############################################################################
// moduleAPI
// ############################################################################

ModuleAPI::ModuleAPI()
: topology(false)
{
	topo_cpuset_zero(&cpumask);
}

/// Mandatory virtual destructor.
ModuleAPI::~ModuleAPI()
{
}

topo_cpuset_t ModuleAPI::getCPUMask() const
{
	return cpumask;
}

bool      ModuleAPI::setCPUMask(topo_cpuset_t mask, const Topo &topo)
{
	if(topo.getIsValid())
		topology = topo;

	if(topo_set_cpubind(topology.topology.t_topology, &mask, 0 ) == 0)
	{
		memcpy( &cpumask, &mask, sizeof( topo_cpuset_t) );
		return true;
	}
	return false;
}

int       ModuleAPI::getCurrentCPU() const
{
#if FLOWVR_SCHED_HAVE_GETCPU == 1 // explicitly test for the value being 1
	/// @todo check: is there a pthreads call for this?
	return sched_getcpu(); // define me in subclass
#else
	return -1;
#endif // FLOWVR_SCHED_HAVE_GETCPU
}

// ############################################################################
// ############################################################################


namespace
{
	void writePID(const std::string &name)
	{
		//log the module PID in file [FLOWVR_PREFIX/share/log/time_name_PID.log]

		// form the path
		std::ostringstream path;
		path << getenv("HOME") << "/.flowvr/";
		std::string dir = path.str();

		if (access(dir.c_str(), W_OK) != 0)
		{
			if (mkdir(dir.c_str(), 0744) != 0)
			{
				std::cerr << "could not create flowvr home dir @ " << dir
						<< std::endl;
				return;
			}
		}


		std::ofstream d(dir.c_str());

		//get time
		time_t time_date;
		struct tm *current_time;
		time_date = time(NULL);
		current_time = localtime(&time_date);
		path << current_time->tm_hour << "h" << current_time->tm_min << "-";

		// add machine name, to distinguish one module instantiated on 2 hosts
		char current_host[255];
		gethostname(current_host, sizeof(current_host));
		path << current_host << "_";

		// if module hasn't name, call it "noname" to avoid error writing the file
		if (name == "")
		{
			path << getpid();
		}
		else
		{
			path << name;
		}

		path << "_PID.log";
		std::string tmp = path.str();

		std::ofstream f(tmp.c_str());
		if (f.is_open())
		{
			f << getpid() << std::endl;
			//query the name of the process:
				// read the name in /proc/proc_id
				std::ostringstream nameQuery;
				nameQuery << std::string("/proc/");
				nameQuery << getpid() ;
				nameQuery << std::string("/cmdline");
				std::string tmp(nameQuery.str());

			std::ifstream proc_file(tmp.c_str());
			std::string process_name;
			std::getline( proc_file, process_name);
			f << process_name;
		}
		else{
			std::cout << "Exception opening/reading file " << std::endl;
		}

		f.close();
	}
} // anonymous namespace for writePID

/// Registers and initializes the module. Returns NULL in case of error.
flowvr::ModuleAPI *initModule(const std::vector<flowvr::Port*> &ports,
		const std::string &instancename,
		const std::string &strmodulename)
{

	flowvr::ModuleAPI * module;
	std::string instName = instancename; // create a copy, as we might modify it below

    // All modules are initialized in parallel mode by default.
	// They then turn to sequential mode if the environment variables
	// FLOWVR_RANK and FLOWVR_NBPROC are not set when starting the module.

	if (!Parallel::isInitialized())
	{
		Parallel::init();
	}
	if (Parallel::isParallel())
	{
		char buf[10];
		sprintf(buf, "%d", Parallel::getRank());
		if (!instName.empty())
			instName += "/";
		instName += buf;
	}

	module = flowvr::ModuleAPIFactory::registerModule(instName, strmodulename);

	if (module == NULL)
	{
		std::cerr << "ModuleAPI::initModule: registerModule Failed."
				  << std::endl;
		return NULL;
	}

	if (module->init(ports) == 0)
	{
		std::cerr << "ModuleAPI::initModule: Failed." << std::endl;
		module->close();
		delete module;
		return NULL;
	}

	writePID(strmodulename);
	return module;
}

/// Registers and initializes the module with traces. Returns NULL in case of error.
flowvr::ModuleAPI *initModule(const std::vector<flowvr::Port*> &ports,
			      const std::vector<flowvr::Trace*> &traces,
			      const std::string &instancename,
			      const std::string &modulename)
{

	flowvr::ModuleAPI * module;
	std::string instName = instancename;
	if (!Parallel::isInitialized())
	{
		Parallel::init();
	}
	if (Parallel::isParallel())
	{
		char buf[10];
		sprintf(buf, "%d", Parallel::getRank());
		if (!instName.empty())
			instName += "/";
		instName += buf;
	}
	module = flowvr::ModuleAPIFactory::registerModule(instName, modulename);
	if (module == NULL)
	{
		std::cerr << "ModuleAPI::initModule:  registerModule Failed."
				<< std::endl;
		return NULL;
	}
	if (module->init(ports, traces) == 0)
	{
		std::cerr << "ModuleAPI::initModule: ModuleAPI Init Failed."
				<< std::endl;
		module->close();
		delete module;
		return NULL;
	}
	writePID(modulename);
	return module;
}

} // namespace flowvr
