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

#ifndef PORTINTERFACECACHE_H_
#define PORTINTERFACECACHE_H_

#include <string>
#include <vector>
#include <map>
#include <flowvr/app/core/port.h>

#include <flowvr/portutils/portparameter.h>

namespace flowvr
{
	namespace portutils
	{
		/**
		 * @brief Management structure to avoid the duplicate parsing of configuration
		 *
		 * This is very simple map that caches an already parsed and configured interface for a module
		 * in a PortDescriptor and a structure to capture the arguments that are mapped from Parameters.
		 * It is implemented as a singleton, this structure is not thread safe.
		 */
		class PortInterfaceCache
		{
		public:
			/**
			 * @brief the layout of the port structure of the module
			 *
			 * Captures the name of the port, a same-as name as well as direction (in/out) and msg type (full/stamps)
			 * This is a container class, which simply keeps the values as they are read from the configuration.
			 *
			 */
			class PortDescriptor
			{
			public:
				/**
				 * default constructor, assigns direction as input and type as AUTO
				 */
				PortDescriptor();

				/**
				 * @brief initialization constructor
				 *
				 * Copies all arguments to the members of the object
				 *
				 * @param strName a name (non-empty, though not checked)
				 * @param tp the direction (in/out)
				 * @param mtp the type (full/stamps/auto/same-as)
				 * @param strSameAsName a same-as name (not empty in case of same-as mtp (though not checked))
				 */
				PortDescriptor( const std::string &strName,
								const std::string &portType,
								const std::string &portMessageType,
								const std::string &strSameAsName,
								const std::string &strExport,
								const std::string &strBlocking,
								const std::string &strMultiplicity,
								const std::string &strFileName );

				size_t createPorts( flowvr::app::Component &parent, bool bForceExport = false ) const;

				bool isExportPort() const { return convertTo<bool>(m_Export); }
				std::string getName() const { return m_Name; }
				std::string getFileName() const { return m_strFileName; }
				size_t getMultiplicity() const { return convertTo<size_t>( m_Multiplicity ); }

			private:
				bool isResolved() const;

				PortDescriptor resolved( const flowvr::app::Component &context ) const;

				std::string m_Name,
							m_PortType,
							m_MessageType,
							m_SameAsName,
							m_Export,
							m_Flags,
							m_Multiplicity,
							m_strFileName;
			};



			/**
			 * @brief describes the interface of a module
			 *
			 * The interface class which comprises PortDescriptor and Arg maps to query.
			 *
			 */
			class Interface
			{
				friend class PortInterfaceCache;
			public:
				/**
				 * @brief default constructor
				 *
				 * Sets valid to false. The flag is set later on by the PortInterfaceCache to true
				 * when the interface description was read properly.
				 */
				Interface();


				/**
				 * @brief check vlaidity of the interface
				 *
				 * @return validity state
				 *         - true if valid
				 *         - false else
				 */
				bool getIsValid() const;

				/// Convenience typedef... saves some typing.
				typedef std::map<std::string, PortDescriptor> DESCMAP;

				DESCMAP m_mpDescriptors; /**< the map of descriptors for the ports on this interface */

				/**
				 * @brief returns the parameters to be given to the services of the module
				 *
				 * We impose a structure on the parameter container, it contains at least one [services] section
				 * and one [plugins] and one [module] section.
				 *
				 * @return a read-only container to get the arguments derived from the services used by the interface
				 * @see getPluginParameters(), getModuleParameters()
				 */
				const ParameterContainer &getServiceParameters() const;

				/**
				 * @brief returns the parameters to be given to the plugins of the module
				 *
				 * We impose a structure on the parameter container, it contains at least one [services] section
				 * and one [plugins] and one [module] section.
				 *
				 * @return a read-only container to get the arguments derived from the plugins used by the interface
				 * @see getServiceParameters(), getModuleParameters()
				 */

				const ParameterContainer &getPluginParameters() const;


				/**
				 * @brief returns the parameters to be given to the module
				 *
				 * We impose a structure on the parameter container, it contains at least one [services] section
				 * and one [plugins] and one [module] section.
				 *
				 * @return a read-only container to get the arguments derived from the module used by the interface
				 * @see getServiceParameters(), getPluginParameters()
				 */
				const ParameterContainer &getModuleParameters() const;


				/**
				 * @brief the argument / parameter tree to be passed to this interface.
				 *
				 * This should be treated as a read-only structure. We expose it currently to avoid some copies
				 * as the argument tree can be big.
				 */
				ARGS        m_mpARGS;

				std::string m_Info; /**< the info text as given by the user */

			private:
				/** validity flag, can only be set by PortInterfaceCache. If false, this interface is not valid, i.e. do not use it.
				 *  @see getIsValid()
				 */
				bool m_valid;
			};


			/**
			 * constructor.
			 * To get a hand on the cache, use the getSingleton() method.
			 * @see getSingleton()
			 */
			PortInterfaceCache();

			/**
			 * destructor.
			 */
			virtual ~PortInterfaceCache();


			/**
			 * @brief retrieve the PortInterfaceCache singleton
			 *
			 * In case it was not created, it is created after this call.
			 * Note that the memory is collected by an std::auto_ptr at the moment of unloading
			 * the module.
			 *
			 * @return never NULL
			 */
			static PortInterfaceCache *getSingleton();

			/**
			 * @brief retrieve the interface using a symbolic name
			 *
			 *
			 * @param strName the symbolic name under which the interface was registered beforehand
			 *        using loadInterface()
			 *
			 * @return an interface
			 *         - Interface::getIsValid() == true in case the interface was found
			 *         - Interface::getIsValid() == false else
			 */
			Interface getInterface( const std::string &strName ) const;

			/**
			 * @brief test whether something is registered as interface or not
			 *
			 * @param strName the symbol under which an interface was probably registered using loadInterface()
			 *
			 * @return interface exists?
			 * 			- true in case getInterface(strName) returns a valid interface
			 *          - false else
			 */
			bool      hasInterface( const std::string &strName ) const;


			/**
			 * @brief create an interface description based on an xml/file description
			 *
			 * 'loads' an xml tree from the filename given, parses it and create the interface
			 * that is described in the file and caches it using the xml file name as a key.
			 * Interfaces that were already loaded are not loaded again. This can be overridden
			 * by setting bForceReplace to true, as this will force the reload and overwrite of
			 * an existing interface registered usaing the same key.
			 *
			 * @param strCompId the component id is used for throwing an exception and better output
			 *        of flowvr-app
			 * @param strXmlDescFileName the xml file to load, parse create an interface from.
			 *        This is again the key under which interfaces can be queried using hasInterface(),
			 *        or getInterface().
			 * @param bForceReplace in case this is set to true, an interface will replace an old version
			 *        in case it was read earlier.
			 *
			 * @return the interface loaded/read from cache or an invalid interface.
			 *
			 */
			Interface loadInterface( const std::string &strCompId,
									 const std::string &strXmlDescFileName,
									 bool bForceReplace = false );
		private:
			/// convenience typedef... saves some typing...
			typedef std::map<std::string, Interface> INTMAP;


			INTMAP m_mpInterfaces; /**< the interface map */
		};
	} // namespace portutils
} // namespace flowvr




#endif // PORTINTERFACECACHE_COMP_H_
