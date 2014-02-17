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
 * File: ./include/flowvr/app/components/filterstamp.h           *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTER_STAMP_H
#define __FLOWVR_APP__FILTER_STAMP_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filterstamp.comp.h
 * \brief Implements the stamp filter
 */

namespace flowvr { namespace app {


    /**
     * \class FilterStamp
     * \brief Forward on output  the message received on input corresponding to the it stamp or received order.
     **/

    class FilterStamp : public Filter
    {
        public :
            FilterStamp(const std::string& id_) : Filter(id_, "FilterStamp") 
            {
	
                setInfo("Forward on output  the message received on input corresponding to the it stamp or received order.");

                addPort("in", INPUT,FULL);
                addPort("order", INPUT,STAMPS);
                addPort("out", OUTPUT,FULL);
            };
      
            virtual Component* create() const { return  new FilterStamp(getId()); };
    };

};}; // end namespace 

#endif // __FLOWVR_APP__FILTER_STAMP_H


