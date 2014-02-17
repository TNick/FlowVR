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
 * File: ./include/flowvr/app/components/filterit2start.h        *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTER_IT2START_H
#define __FLOWVR_APP__FILTER_IT2START_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filterit2start.comp.h
 * \brief Send  it of the messages read by a module and send the it + the max number of allowed buffers
 */

namespace flowvr { namespace app {


    /**
     * \class FilterIt2Start
     * \brief Send it messages on output according to it stamp  recevied on input
     **/

    class FilterIt2Start : public Filter
    {
        public :
            FilterIt2Start(const std::string& id_) : Filter(id_, "It2Start") 
            {
      
                setInfo("Send it messages on output according to it stamp  received on input");
      
                // stamp ports
                addPort("it", INPUT, STAMPS);
                addPort("out", OUTPUT, STAMPS);
            };
      
            virtual Component* create() const { return  new FilterIt2Start(getId());};  
    };

};}; // end namespace 

#endif // __FLOWVR_APP__FILTER_IT2START_H


