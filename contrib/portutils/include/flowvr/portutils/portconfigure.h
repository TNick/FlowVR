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

#ifndef PORTCONFIGURE_H_
#define PORTCONFIGURE_H_

#include <vector>
#include <map>
#include <flowvr/portutils/portcomm.h>
#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/portservicelayer.h>
#include <flowvr/portutils/portparameter.h>

namespace flowvr
{
	class ModuleAPI;
	class StampList;

	namespace xml
	{
		class TiXmlHandle;
	}
}


namespace flowvr
{
	namespace portutils
	{
		// ########################################################################
		// PLUGINS
		// ########################################################################

		/**
		 * @brief Configure plugins
		 * Configure plugins given in a subtree of an xml file.
		 * @param pluginConf the xml tree root pointing to the configuration of the plugins
		 *        - root: \<plugins\>
		 *          - \<plugin ... \>
		 *          - ...
		 *
		 * @see createServices(), configureServiceLayers()
		 * @param services the services that may be given to the plugins during creation
		 * @param vecPlugsOutput the output vector to store successfully configured plugins to
		 *
		 * @return parse success
		 *         - false if the structure of the xml tree did not suffice. In case there are 'local errors'
		 *           (in the plugin texts), we will try to parse as many plugins as possible, skipping errors
		 *         - true if the xml structure was ok, it may be that the PLGS vector is empty, though
		 */
		bool configurePlugins( const flowvr::xml::TiXmlHandle &pluginConf,
				const SERV &services,
				PortPlugin::PLGS &vecPlugsOutput,
				const ARGS &args);

		/**
		 * @brief dispose once opened plugins
		 *
		 * The dispose function comprises
		 * - calling the dispose method
		 * - closing the so file
		 *
		 * The elements in vecPlugs are untouched, you should consider them unusable after a call to this method.
		 *
		 * @param vecPlugs the plugin vector to process for disposal
		 * @return always true
		 */
		bool disposePlugins( const PortPlugin::PLGS &vecPlugs );


		// ########################################################################
		// PORTS AND STAMPS
		// ########################################################################

		/**
		 * @typedef STAMPMAP
		 *
		 * Maps a symbolic name to a StampList instance (we share StampLists, as they are dictionaries on the
		 * memory to look for stamps).
		 */
		typedef std::map<std::string, flowvr::StampList*> STAMPMAP;


		/**
		 * @brief load and configure StampList instances from xml configuration
		 *
		 * The function expects the StampList root as first argument and stores all sucessfully read stamps
		 * to map. It may recurse and load the StampList from another file.
		 * Note that StampLists are allocated on the heap and have to be deleted by someone later on.
		 *
		 * @param stampConf the root for the stamplists
		 *        - root: \<stamplists\>
		 *          - \<stamplist ... \>
		 *            - \<stamp ... \>
		 *
		 * @param map the stamp map to store the configured stamp lists to.
		 *
		 * @param sSelectName argument used for recursing (external file selection),
		 *         root calls should not set this parameter.
		 *
		 * @param strPrefix the directory prefix to prefix when trying to load the resource
		 */
		void configureStampMap( const flowvr::xml::TiXmlHandle &stampConf,
								STAMPMAP &map,
								const std::string &sSelectName = "",
								const std::string &strPrefix = "");

		/**
		 * @brief configure ports from an xml file
		 *
		 * Ports are configured from the xml file. They use stamplists that were preconfigures.
		 * This method watches for:
		 * - double definition
		 * - name and direction attributes given
		 * - eventsource flags
		 *
		 * It creates a map of ports created (for simple lookup) and a vector of ports (for ordering and
		 * passing to flowvr::moduleAPI::init().
		 *
		 * @param applConf xml root node of the ports subtree
		 *        - root: \<ports\>
		 *          - \<port ... \>
		 *          - ...
		 * @param stamps the pre-configured stamp lists to use for the creation of the ports
		 * @param vePortMap a lookup structure for simple finding of ports when using their names
		 * @param vePorts the port vector to pass to flowvr
		 *
		 * @return the number of ports actually created.
		 */
		int configurePorts( const flowvr::xml::TiXmlHandle &applConf,
							const STAMPMAP &stamps,
										   std::map<std::string, flowvr::Port*> &vePortMap,
										   std::vector<flowvr::Port*> &vePorts,
										   const ARGS &args );


		// ########################################################################
		// CODE PATHS
		// ########################################################################

		/**
		 * @brief parses all code paths
		 *
		 * Plugins (PLGS, ports and services have to be configured before calling this function.
		 *
		 * @param codeRoot
		 *        - root: \<paths\>
		 *          - \<path ... \>
		 *
		 * @param veCodeVector the (ordered) code vector that will be configured by this function
		 * @param mpPorts for quick lookup: pass the ports as a map (name to port)
		 * @param vecPlugs the preconfigured vector of plugins to use in the code paths
		 *
		 * @return always true
		 */
		bool configurePortComm( const flowvr::xml::TiXmlHandle &codeRoot,
								std::vector<flowvr::portutils::PortComm::PortMap> &veCodeVector,
								const std::map<std::string, flowvr::Port*> &mpPorts,
								const PortPlugin::PLGS &vecPlugs );


		/**
		 * @brief get rid of the code paths
		 * Dispose handlers associated to the code paths, makes sure that there is no double deletion
		 * (handlers can be assigned more than once in a code run)
		 *
		 * @param veCodeVector the code paths to dispose (precofnigured by configurePortComm)
		 * @param vecPlugs the plugin structure to use (to get disposal method)
		 *
		 * @return always true
		 */
		bool disposePortComm( std::vector<flowvr::portutils::PortComm::PortMap> &veCodeVector,
							  const PortPlugin::PLGS &vecPlugs );




		// ########################################################################
		// SERVICE LAYER
		// ########################################################################

		/**
		 * @brief physically open a service layer.
		 *
		 * This function opens a so given by path soName and creates a configured service plug.
		 * It looks for the presence of "C" functions with names
		 * - createService
		 * - disposeService
		 * - createArgs (optional)
		 *
		 * The only error uttered will be that the so file was not opened by dlopen.
		 *
		 * Normally, there is no need to call this function on user level, use configureServiceLayers()
		 * instead.
		 *
		 * @see configureServiceLayers(), closeServiceLayers()
		 *
		 * @param soName the absolute or relative file path for the so to open for the service
		 * @param strObjectName the name to store in the ServicePlug name field
		 *
		 * @return a service plug (it has an empty name and NULL so handle in case it is invalid)
		 */
		ServicePlug openServiceLayer( const std::string &soName, const std::string &strObjectName );

		/**
		 * @brief closes (and disposes) a service plug
		 * In case the disposal method was defined in the plug, and there is an active layer,
		 * this method calls dispose on the layer. It calls dlclose on the so handle, it sets all
		 * members of the service plug to NULL.
		 */
		void closeServiceLayer( ServicePlug &pLayer );

		/**
		 * @brief configure the service layers as given by the xml subtree
		 *
		 * The services are configured from the xml file as given by sub-tree root.
		 * In case two services are configured to have the same name, the second definition will be skipped.
		 *
		 * @param serviceRoot
		 *        - root: \<services\>
		 *           - \<service ...\>
		 * @param services the vector to store configured services to
		 *
		 * @return always true
		 */
		bool configureServiceLayers( const flowvr::xml::TiXmlHandle &serviceRoot, SERV &services );

		/**
		 * @brief setup argument dispatching for the service
		 *
		 * This function calls the setupArgs() method for all services that have one configured.
		 * The setup goes through the vector one by one as configured.
		 *
		 * @return always true
		 */
		bool prepareServices( const SERV &services );

		/**
		 * @brief physically create the services
		 * The setup process for services is a two-step mechanism. The first step will open the so files
		 * and search for all the resources needed (function pointers), while the second step will acutally
		 * create the services and pass arguments. This is the function to do this second step.
		 *
		 * @param services the vector of services preconfigured using configureServiceLayers() and prepareServices()
		 * @param args the command line arguments to pass for paramter traversal
		 *
		 * @return always true
		 */
		bool createServices( SERV &services, Module &parent, const ARGS &args );


		/**
		 * @brief get rid of services and their resources
		 *
		 * Macro function to call closeService() on each member of the argument vector
		 *
		 * @see closeService()
		 * @param services the vector of services to close
		 * @return always true
		 */
		bool disposeServiceLayers( SERV &services );


		void *openDynamicLib( const std::string &infix, int flags, std::string &resolvedName );
	} // namespace portutils
} // namespace flowvr

#endif /* PORTCONFIGURE_H_ */
