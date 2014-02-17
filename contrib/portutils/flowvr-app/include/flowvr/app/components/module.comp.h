/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
 * INRIA                                                           *
 * ALL RIGHTS RESERVED.	                                          *
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
 * File: ./include/flowvr/app/components/module.comp.h           *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP__MODULE_H
#define __FLOWVR_APP__MODULE_H

#include "flowvr/app/core/component.h"


/**
 *
 * \brief implements base class for  all modules
 */

namespace flowvr {
	namespace app {

     
		/**
		 * \class Module
		 * \brief this class is the parent of all module
		 */
		class Module : public Primitive
		{
			public:
				/**
				 * \brief constructor
				 * \param id_ : the module id.
				 * \note in a module, we have a beginIt and an endIt ports
				 */
				Module(const std::string& id_) : Primitive(id_,"module")
				{
					setInfo("A module is a FlowVR primitive component reading  messages from input ports and sending  messages to output ports. Have by default a beginIt input port (block iteration as long as no message is present) and a endIt output port (send a message at the end of each iteration)");
					addPort("beginIt", INPUT,STAMPS, "", Port::ST_BLOCKING );
					addPort("endIt", OUTPUT,STAMPS);

					//Add default traces for modules
					myTraceList.push_back(std::string("waitBegin"));
					myTraceList.push_back(std::string("waitEnd"));
				};

				/**
				 * \brief copy constructor
				 */
				Module(const Module& m) : Primitive(m) {}

				/**
				 * \brief virtual create:  a new instance of the object initialized with same id as this.
				 */
				Component* create() const { return new Module(getId()); }

				/**
				 * \brief destructor
				 */
				virtual ~Module() {}


				bool isPrimitive() const {return true; }
				/**
				 * \brief return the compoenent type (OBJECT)
				 */
				TypeComponent getTypeComponent() const { return OBJECT; }


		};

	}
}

#endif //__FLOWVR_APP__MODULE_H
