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

#ifndef PORTMODULE_H_
#define PORTMODULE_H_


#include <string>
#include <vector>


#include <flowvr/portutils/portcomm.h>
#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/portservicelayer.h>
#include <flowvr/portutils/portconfigure.h>

namespace flowvr
{
	class ModuleAPI;
	class Allocator;
}

namespace flowvr
{
	namespace portutils
	{
		/**
		 * @brief basic module configuration and state machine
		 *
		 * Encapsulation of the state machine of a simple module.
		 * Simple in that case means that the module is only working on one flowvr module
		 * at the same time.
		 * The main routine that might interest you is the call to process().
		 * This calls PortComm::process() repeatedly until this method returns a RET_FAIL
		 * that indicates a severe error, or the end iteration of flowvr.
		 * The module can be closed by a call to close(). Note that this is a flowvr call, which
		 * means that the module is purged from the daemon.
		 * The module does the bulk work of setting up the module from the description file given as
		 * an argument to the constructor.
		 */
		class Module
		{
		public:
			Module( );

			/**
			 * - deletes the flowvr module (does not call close before)
			 * - deletes all stamplists that were created during configuration
			 * - disposes (and closes) all code paths
			 * - disposes (and closes) all plugins loaded
			 * - disposes (and closes) all services configured
			 */
			~Module();

			/**
			 * @param strModuleText the absolute or relative file path to the xml file that is used for
			 *        the configuration of this module.
			 * @param argc the number of arguments passed to main (or the number of pointers in the argv array
			 * @param argv the argument array to be passed as a commandline
			 */
			bool init( const std::string &strModuleText, int argc, char **argv, bool bStrictParse = true );

			void getArgs( int &argc, char **&argv ) const;

			/**
			 * Main loop of the module. Calls PortComm::process() until this returns PortComm::RET_FAIL.
			 * @return always 0
			 */
			int process();

			/**
			 * calls flowvr::moduleAPI::close() on the flowvr module configured (if configured).
			 * Note that this will purge the module and can be called during signals or by other threads.
			 */
			void close();
			bool getIsClosing() const;


			void registerServicePlug( const ServicePlug &plug );


			ServicePlug getServiceBySoName( const std::string &strSoName ) const;
			ServicePlug getServiceByName( const std::string &strName ) const;

			void        callPreLoopActivation();
			PortComm   &getPortComm() const;


			flowvr::Allocator &getAllocator() const;
			flowvr::ModuleAPI *getModuleAPI() const;
		private:

			mutable PortComm  *loop; /**< the bulk worker for runtime */
			mutable flowvr::ModuleAPI *flowvr; /**< the moduleAPI to work on */

			bool isClosing;

			std::vector<PortComm::PortMap> mpPortComms; /**< for cleanup: the code paths configured */
			PortPlugin::PLGS plugs; /**< for cleanup: the plugs loaded */
			SERV services; /**< for cleanup: services loaded */

			flowvr::portutils::STAMPMAP stampMap; /**< for cleanup: all stamplists created during setup of the module */

			int    m_argc;
			char **m_argv;
		};
	} // namespace portutils
} // namespace flowvr


#endif // PORTMODULE_H_
