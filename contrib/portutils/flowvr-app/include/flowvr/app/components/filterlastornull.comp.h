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
 * File: ./include/flowvr/app/components/filterlastornull.comp.h   *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERLASTORNULL_H
#define __FLOWVR_APP__FILTERLASTORNULL_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filterlastornull.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterLastOrNull
     * \brief Forward on  output messages  the last input message or null message if there is no input. Discard other messages.
     **/

    class FilterLastOrNull : public Filter
    {
        public :
      
            FilterLastOrNull(const std::string& id_) : Filter(id_, "FilterLastOrNull") 
            {
	
                setInfo("Forward on  output messages  the last input message or null message if there is no input. Discard other messages.");
	
                addPort("in", INPUT);
                addPort("order", INPUT, STAMPS); // stamps
                addPort("out", OUTPUT);
            };
      
            virtual Component* create() const {return new FilterLastOrNull(getId());};  
    };
    
};}; // end namespace 

#endif // __FLOWVR_APP__FILTERLASTORNULL_H


