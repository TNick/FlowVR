/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                        PortUtils library                        *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
*                                                                 *
*   The source code is  covered by different licences,            *
*   mainly  GNU LGPL and GNU GPL. Please refer to the             *
*   copyright.txt file in each subdirectory.                      *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*  Ingo Assenmacher,                                              *
*  Bruno Raffin                                                   *
*                                                                 *
*******************************************************************/

#include <flowvr/portutils/portmodule.h>
#include <signal.h>
#include <iostream>

#include <flowvr/utils/timing.h>
#include <flowvr/utils/backtrace.h>


flowvr::portutils::Module *md = NULL;

void termhandler(int)
{
	if(md)
		md->close();

	signal(SIGINT, SIG_DFL);
}

void segvhandler(int)
{
	if(md)
		md->close();

	flowvr::utils::stacktrace();
	signal(SIGSEGV, SIG_DFL);
}

int main( int argc, char **argv )
{

	// setup signal handlers
	(void) signal(SIGINT, termhandler);
	(void) signal(SIGSEGV, segvhandler);

	// this is a debugging hack if you want to step through the
	// construction of flowvr::portutils::Module(), set b to true,
	// attach with a debugger, flip to false again and go.
	bool b = false;
	while(b)
	{
		flowvr::utils::microsleep(1000);
	}
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	try
	{
		md = new flowvr::portutils::Module();

		md->init(": generic flowvr binary, using port utils", argc, argv );

		// start
		(*md).callPreLoopActivation();
		(*md).process();
		(*md).close();
	}
	catch( std::exception &e )
	{
		std::cerr << "flowvr-portbinary exception: " << e.what() << std::endl;
	}
	delete md;

	return 0;
}

