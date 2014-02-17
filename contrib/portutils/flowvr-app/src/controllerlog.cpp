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
 *	File : flowvr/app/core/controllerlog.h                    *
 *                                                                 *
 *	Contact : Jean-Denis Lesage		                  *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/controllerlog.h"
#include "flowvr/app/core/traverse.h"
#include "flowvr/app/core/component.h"

namespace flowvr { namespace app {

    ControllerLog::~ControllerLog()
    {
        for(std::list<InterfaceController*>::iterator it = queueControllers.begin(); it != queueControllers.end(); ++it)
            {
                delete *it;
            }
    }

    void ControllerLog::registerController(InterfaceController* c)
    {
        //look if a controller has the same method (perf improvement)
        std::list<InterfaceController*>::const_iterator it = queueControllers.begin();
        while(it != queueControllers.end() && !(*it)->equals(c))
            {
                ++it;
            }
        if(it == queueControllers.end())
            {
            queueControllers.push_back(c);
            }
    }

    void ControllerLog::applyLogOn(Component* c) const
    {
        for(std::list<InterfaceController*>::const_iterator it = queueControllers.begin(); it != queueControllers.end(); ++it)
            {
                applyLogOn(c, *it);
            }
    }

    void ControllerLog::applyLogOn(Component* c, InterfaceController* it) const
    {
        InterfaceController* controller = it->clone();
        //			Msg::debug("Apply log on id = " + toString<void*>(c) + " ( name is = " + c->getFullId() + " ) " , __FUNCTION_NAME__);
        TraversePointFixe t(c, controller);
        t.silent=true;//switch controller to replay mode -> less verbose
        t();// Thi is time consuming to replay everything. There is room for optimization there. 
        delete controller;

        
        /*
          // Ths code does not work as even when reapplying controllers there are  dependencies that require a fixed point traverse to be properly resolved.
        controller->setComponentClone(c);
        (*controller)();
        delete controller;
        Composite* comp = dynamic_cast<Composite*>(c);
        if(comp)
            {
                for(Composite::pComponentIterator itComp = comp->getComponentBegin(); itComp != comp->getComponentEnd(); ++itComp)
                    {
                        applyLogOn(*itComp, it);
                    }
            }
        */
    }



    const bool ControllerLog::isControllerApplied(const std::string name) const
    {
        for (std::list<InterfaceController*>::const_iterator it = queueControllers.begin(); it != queueControllers.end() ; it++)
            {
                if ( (*it)->name == name ) return true; 
            }
        return false;
    }


    void ControllerLog::copyControllersFrom(const ControllerLog& contlog) 
    {
        
        // First clear the queue
        for(std::list<InterfaceController*>::iterator it = queueControllers.begin(); it != queueControllers.end(); ++it)
            {
                delete (*it);
            }
        queueControllers.clear();
        
        // Next copy
        for(std::list<InterfaceController*>::const_iterator it = contlog.queueControllers.begin(); it != contlog.queueControllers.end(); ++it)
            {
                queueControllers.push_back((*it)->clone());
            }
    }
    
    
    
}; };
