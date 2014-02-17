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
 * File: ./include/flowvr/app/components/filterminit.comp.h      *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERMINIT_H
#define __FLOWVR_APP__FILTERMINIT_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filterminit.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMinIt
     * \brief  After receiving one new  message per  input, forward on output  the last  new message received.  The it of this message is changed for the minimum it of the new   messages received  on inputs.
     */
    class FilterMinIt : public Filter
    {
        public :

            FilterMinIt(const std::string& id_) : Filter(id_, "MinIt") 
            {
                setInfo("After receiving one new  message per  input, forward on output  the last  new message received.  The it of this message is changed for the minimum it of the new   messages received  on inputs.");

                // stamp ports
                addPort("in0", INPUT,STAMPS);
                addPort("in1", INPUT,STAMPS);
                addPort("out", OUTPUT,STAMPS);
            };
	  
            virtual Component* create() const { return new FilterMinIt(getId());};  
	  
    };
    

};}; // end namespace 

#endif // __FLOWVR_APP__FILTERMINIT_H


