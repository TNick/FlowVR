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
 * File: ./include/flowvr/app/components/filterit.comp.h         *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERIT_H
#define __FLOWVR_APP__FILTERIT_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filterit.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterIt
     * \brief Forward on its  output  the messages received on its input having the same it numbers than the ones received on the order port. Discard other messages (filter usually used with a synchronizer).
     **/

    class FilterIt : public Filter
    {
        public :
      
            FilterIt(const std::string& id_) : Filter(id_, "FilterIt") 
            {
	
                setInfo("Forward on its  output  the messages received on its input having the same it numbers than the ones received on the order port. Discard other messages  (filter usually used with a synchronizer).");
	
                addPort("in", INPUT,FULL);
                addPort("order", INPUT, STAMPS); // stamps
                addPort("out", OUTPUT,FULL);
            };
      
            virtual Component* create() const {return new FilterIt(getId());};  
    };
    
};}; // end namespace 

#endif // __FLOWVR_APP__FILTERIT_H


