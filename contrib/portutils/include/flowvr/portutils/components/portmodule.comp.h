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

#ifndef PORTMODULE_COMP_H_
#define PORTMODULE_COMP_H_

#include <flowvr/app/components/module.comp.h>

namespace flowvr
{
	namespace portutils
	{
		/**
		 * @brief module abstraction for PortUtils based modules.
		 *
		 * The PortModule instances respect the configuration file hints during create.
		 */
		class PortModule : public flowvr::app::Module
		{
		public:
			/**
			 * @brief constructor
			 *
			 * - The constructor will throw in case the configuration file is set as empty string.
			 * - Retrieves a copy of the interface from the PortInterfaceCache.
			 * - Sets its info according to the string given by the user / PortInterfaceCache.
			 * - adds all ports as given in the interface description.
			 *
			 * @param strId the flowvr-app id
			 * @param interfaceKey the interface to warp by this PortModule
			 * @param parent a pointer to the Composite on which to call the insertObject() method (optional,
			 *        but if you give NULL here, do nto forget to insert the module to the correct parent after creation)
			 */
			PortModule( const std::string &strId, const std::string &interfaceKey, flowvr::app::Composite *parent = NULL );

			/**
			 * creates a new instance of a PortModule with the same configuration file argument.
			 */
			Component *create() const;
		protected:
		private:

			std::string m_strPortfile; /**< the configuration file name */
		};
	} // namespace portutils
} // namespace flowvr



#endif // PORTMODULE_COMP_H_
