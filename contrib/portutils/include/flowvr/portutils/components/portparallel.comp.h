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

#ifndef PORTPARALLEL_COMP_H_
#define PORTPARALLEL_COMP_H_

#include <flowvr/app/core/component.h>

namespace flowvr
{
	namespace portutils
	{
		/**
		 * @brief parallel execution of a bunch of modules
		 *
		 * Inspired by PatternParallel in the original code base, but without templates,
		 * using a prototype for cloning of child components.
		 */
		class PortParallel : public flowvr::app::Composite
		{
		public:
			/**
			 * @brief constructor, sets up parameters and ports (cloning from component)
			 *
			 * Sets up parameters:
			 * - LOOKUP_NB_INSTANCES_METHOD, can be
			 *   - STATIC
			 *   - FROM_PORT
			 *   - everything else... (using the CSV)
			 * - PORT_NAME_INSTANCES: used when LOOKUP_NB_INSTANCES_METHOD is set to FROM_PORT
			 *   and is the name of the port to use to determine the number of parallel branches
			 *   by counting the number of sibling links
			 * - NB_INSTANCES: used when LOOKUP_NB_INSTANCES_METHOD is set to STATIC, or <i>set</i>
			 *   when another method is used to communicate the construction during execute()
			 *
			 *   @param id_ the id for this composite
			 *   @param md the prototype to clone and memorize
			 */
			PortParallel(const std::string& id_, const Component &md);

			/**
			 * @brief destructor
			 *
			 * deletes the clone of the composite prototype
			 */
			virtual ~PortParallel();


			/**
			 * creates a clone of this composite.
			 */
			virtual Component* create() const;

			/**
			 * @brief construction of the composite
			 *
			 * Constructs parallel execution branches determined on the paramteres as described on top of
			 * the constructor.
			 */
			virtual void execute();

			/**
			 * @brief override classic implementation for setting the hosts
			 *
			 * Depending on the lookup type, see above.
			 */
			virtual void setHosts();
		private:
			Component *module;
		};
	} // namespace portutils
} // namespace flowvr


#endif // PORTPARALLEL_COMP_H_
