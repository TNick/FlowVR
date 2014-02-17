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
 * File: ./include/flowvr/app/components/filtermergeit.comp.h    *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERMERGEIT_H
#define __FLOWVR_APP__FILTERMERGEIT_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"



/**
 * \file filtermergeit.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMergeIt
     * \brief Each time it  a message  on its order input, it forward the messages accodring to the (optional) stamp and scratch parameters
     */
    class FilterMergeIt : public Filter
    {
        public :

            FilterMergeIt(const std::string& id_) : Filter(id_, "MergeIt")
            {
                setInfo(
					"Merge messages received on input port into one message sent on output port.\
					 More precisely, when it receives a message  on the 'order' port,\
					 it inspects the incoming message queue ('in' port), discard all messages with \
					 a non null 'scratch' stamp value, concatenate all other messages in one message\
					 sent on 'out' port. This message has its 'scratch' stamp set to 0 and its\
					 'stamp' stamp set to the sum of all 'stamp' stamps of the concatenated messages.\
					 The  name of the 'scratch' and 'stamp' stamps are read from the component parameter list.\
					 These parameters are optional.");

                addPort("in", INPUT);
                addPort("order", INPUT,STAMPS); //stamps
                addPort("out", OUTPUT);

                addParameter("stamp","");
                addParameter("scratch","");

            };

            virtual Component* create() const { return  new FilterMergeIt(getId());};
    };


}
} // end namespace

#endif // __FLOWVR_APP__FILTERMERGEIT_H


