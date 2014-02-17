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
 *     File : flowvr/app/core/controllerlog.h                      *
 *                                                                 *
 *      Contact : Jean-Denis Lesage                                *
 *                                                                 *
 ******************************************************************/

#include<list>
#include "flowvr/app/core/interfacecontroller.h"

#ifndef __FLOWVR_APP_CONTROLLERLOG_H__
#define __FLOWVR_APP_CONTROLLERLOG_H__

namespace flowvr { namespace app {

    // Need to define component to avoid cyclic dependence between h files
    class Component;

    /**
     * \brief This class register controller calls history on a specified component (controller sucessfully applied)
     */
    class ControllerLog
    {
        private :
            std::list<InterfaceController*> queueControllers; ///< history of applied controller

        public :
            ControllerLog() : queueControllers() {};
            ~ControllerLog();
				
            /**
             * \brief copy constructor that clones each InterfaceController of the list
             */
            //            ControllerLog(ControllerLog * contlog): queueControllers() { contlog->copyQueueControllers(queueControllers);};


            /**
             * \brief add a controller to the history
             */
            void registerController(InterfaceController* c); 

            /**
             * \brief replay controller history on a component
             */
            void applyLogOn(Component* c) const;

            /**
             * \brief test if a given  controller has been applied
             *
             * @param name the name of the controller (based on the registered controller name)
             * @return true if controler applied
             */
            const bool isControllerApplied(const std::string name) const;

            /**
             * @brief set the current controller log to be identical to the one given in argument. 
             * @param contlog the controller log to be cloned
             * \param contlog  push back in this list clones of the controller queue
             */
            void copyControllersFrom(const ControllerLog& contlog);

        private :
            /**
             * \brief apply a controller on a component and all its children
             */
            void applyLogOn(Component* c, InterfaceController* it) const;

    };


};};


#endif //__CONTROLLERLOG__
