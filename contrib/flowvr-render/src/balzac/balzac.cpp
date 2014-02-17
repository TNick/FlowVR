/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                   Balzac FlowVR render frontend                 *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA.  ALL RIGHTS RESERVED.                                    *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING file for further information.                           *
*                                                                 *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: balzac.cpp                                              *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include <signal.h>
#include <iostream>

#include <flowvr/utils/timing.h>
#include <flowvr/utils/backtrace.h>

#include <flowvr/utils/cmdline.h>

#include <flowvr/render/balzac/servicelayer/servicelayer.h>
#include <flowvr/render/balzac/servicelayer/display.h>

#include <flowvr/portutils/portmodule.h>


namespace
{
	bool bDeletion = false;
}



class BalzacApp
{
public:
	BalzacApp( int argc, char **argv )
	: m_module( new flowvr::portutils::Module )
	, m_balzacService( NULL )
	{
		if((*m_module).init("Balzac", argc, argv, false) == false)
			throw std::exception();

		flowvr::portutils::ServicePlug blz = (*m_module).getServiceByName("Balzac");
		if(!blz.isValid())
		{
#if defined(__APPLE__)
			blz = (*m_module).getServiceBySoName( "libBalzacService.dylib" );
#else
			blz = (*m_module).getServiceBySoName( "libBalzacService.so" );
#endif

		}

		if( !blz.isValid() )
			throw std::exception();

		m_balzacService = dynamic_cast<flowvr::render::balzac::ServiceLayer *>(blz.m_layer);

		if(!m_balzacService)
			throw std::exception();
	}

	~BalzacApp()
	{
		delete m_module;
	}

	void close()
	{
		(*m_module).close();
	}

	void process()
	{
		(*m_module).callPreLoopActivation();
		(*m_balzacService).run();
	}

	bool isClosing() const
	{
		return m_module->getIsClosing();
	}

	flowvr::portutils::Module                     *m_module;
	flowvr::render::balzac::ServiceLayer *m_balzacService;
};

BalzacApp *md = NULL;


void termhandler(int)
{
	if(md)
		md->close();

	signal(SIGINT, SIG_DFL);
}

void segvhandler(int)
{
	if( md and !md->isClosing() and !bDeletion) // could be an error in close(), so do not call it again
		md->close();

	flowvr::utils::stacktrace();
	signal(SIGSEGV, SIG_DFL);
}

#if !defined(BALZAC_USE_FREEGLUT)
void cleanup()
{
	if(md)
		(*md).close();

	delete md;
}
#endif // !BALZAC_USE_FREEGLUT



int main( int argc, char **argv )
{

	// setup signal handlers
	(void) signal(SIGINT, termhandler);
	(void) signal(SIGSEGV, segvhandler);

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if !defined(BALZAC_USE_FREEGLUT)
	atexit( cleanup );
#endif // !BALZAC_USE_FREEGLUT

	try
	{
		md = new BalzacApp( argc, argv );
		// start
		(*md).process();
		(*md).close();
	}
	catch( std::exception &e )
	{
		std::cerr << "Caught an exception from Balzac internals. Balsac will quit." << std::endl;
		std::cout << e.what() << std::endl;
	}


	bDeletion = true;

	delete md;
	return 0;
}



