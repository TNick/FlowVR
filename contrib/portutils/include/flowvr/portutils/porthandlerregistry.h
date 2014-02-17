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

#ifndef PORTUTILSPORTHANDLERREGISTRY_H_
#define PORTUTILSPORTHANDLERREGISTRY_H_

#include <map>
#include <string>

namespace flowvr
{
	namespace portutils
	{
		class PortHandlerBase;

		/**
		 * @brief Utility class to manage PortHandlerBase instances
		 *
		 * This is a simple lookup structure to manage PortHandlerBase instances that were created
		 * during the configuration of the module. It is implemented as a singleton, it is not
		 * considered to be used within different threads.
		 * Port handlers can be registered and unregistered using a symbolic name.
		 */
		class PortHandlerRegistry
		{
		public:
			/**
			 * Currently does nothing, especially, it does not delete any registered pointer.
			 * Note that the destructor is not virtual, it is not expected that someone subclasses it.
			 */
			~PortHandlerRegistry();

			/**
			 * @brief get registry singleton
			 * The registry will be created upon the first call to getSingleton().
			 * Note that the pointer is collected using std::auto_ptr(), which means at the
			 * unloading of the binary from memory.
			 */
			static PortHandlerRegistry *getSingleton();



			/**
			 * @brief register a PortHandlerBase
			 * Registers a PortHandlerBase instance with this registry. The registration is done by
			 * force, that is: already registered instances on the symbol will be forgotten.
			 * The same pointer can be registered using different symbols.
			 *
			 * @param strSymbol the nsymbolic name to use for the PortHandlerBase.
			 * @param pHandler the handler to register under the name strSymbol
			 */
			void registerPortHandler( const std::string &strSymbol, PortHandlerBase *pHandler);

			/**
			 * @brief remove a handler from the set of registered handlers by querying the name.
			 *
			 * Removes a handler that was registered with the name strSymbol.
			 * @param  strSymbol the name to use for lookup of the handler
			 * @return success state
			 *         - true if a handler that was calles strSymbol was removed
			 *         - false else
			 */
			bool unregisterPortHandler( const std::string &strSymbol );

			/**
			 * @brief check, if there is a handler called strSymbol
			 *
			 * Simple check/query routine to see if a symbol 'strSymbol' is alreday mapped to
			 * a handler.
			 *
			 * @param strSymbol the name to lookup
			 * @return success state
			 *         - true if there is a mapping from strSymbol to a handler
			 *         - false else
			 */
			bool getIsPortHandler( const std::string &strSymbol ) const;

			/**
			 * @brief retrieve port handler
			 *
			 * Retrieves a port handler called strSymbol.
			 *
			 * @return two cases:
			 *         - the PortHandlerBase in case getIsPortHandler(strSymbol) == true
			 *         - NULL else
			 */
			PortHandlerBase *getPortHandler( const std::string &strSymbol ) const;
		private:
			PortHandlerRegistry();

			/// Convenience typedef... saves some typing
			typedef std::map<std::string, PortHandlerBase*> HNDMAP;
			HNDMAP m_map; /**< the map to store handlers to */
		};
	} // namespace portutils
} // namespace flowvr

#endif /* PORTUTILSPORTHANDLERREGISTRY_H_ */
