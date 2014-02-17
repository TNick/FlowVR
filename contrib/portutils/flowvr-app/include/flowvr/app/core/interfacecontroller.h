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
 *	File : flowvr/app/core/interfacecontroller.h               *
 *                                                                 *
 *	Contact : Jean-Denis Lesage                                *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP_INTERFACE_CONTROLLER__
#define __FLOWVR_APP_INTERFACE_CONTROLLER__

#include <cstddef>
#include <string>

namespace flowvr { namespace app {

    class Component; ///< // Forward declaration for cyclic dependency. Class defined into \sa objectInterface.h. 
	
    /**
     * \brief Interface for all controller
     */
    class InterfaceController
    {
        public :
            bool isModified; ///< controller has succeeded to apply on last components
            std::string name; ///< Name of the controller (to be used in user messages)
        protected:
            Component* idComp; ///< the next component where apply method
        public :
            /**
             * \brief constructor. 
             * \warning The next component to apply has to be set after
             */
            InterfaceController(std::string name_) :isModified(false),  name(name_),idComp(NULL) {};
			
            /**
             * \brief apply method on component
             */
            virtual void operator() ()=0;
			
            /**
             * \brief clone the controller
             */
            virtual InterfaceController* clone() const =0;
			
            /**
             * \brief set the next component to execute
             */
            virtual void setComponent(Component* c) =0;
			
            /**
             * depreciated ???
             */
            virtual void setComponentClone(Component* c) =0;

            /**
             * \brief equality between controller
             * depreciated ???
             */
            virtual bool equals(const InterfaceController* const i) const =0;
    };


}; };

#endif //__INTERFACE_CONTROLLER__

