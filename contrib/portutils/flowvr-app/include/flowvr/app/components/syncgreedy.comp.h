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
 * File: ./include/flowvr/app/components/syncgreedy.comp.h       *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__SYNCGREEDY_H
#define __FLOWVR_APP__SYNCGREEDY_H

#include <climits>

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/sync.comp.h"


      


/**
 * \file syncgreedy.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class SyncGreedy
     * \brief Every time it receives a message on endIt port,  it looks for the most recent message received on stamps port and sends to order port  the stamp of this message. Used for implementing  greedy (subsampling) filtering. 
     */
    class SyncGreedy : public Sync
    {
        public :

            SyncGreedy(const std::string& id_) : Sync(id_, "GreedySynchronizor") 
            {
                setInfo("Every time it receives a message on endIt port, \
                		it looks for the most recent message received on \
                		stamps port and sends to order port  the stamp of \
                		this message. Used for implementing  greedy \
                		(subsampling) filtering. It uses 4 different parameters.\
                		 min: minimum number of messages left in the queue \
                		(0: will not order to repeat previously sent messages. \
                		1: will repeat previously forwarded message if \
                		nothing newer available). max: maximum number of messages\
                		that are kept in the queue. buffer: todo. stampname: todo");


                //stamps only ports -> false parameter
                addPort("endIt", INPUT,STAMPS);
                addPort("stamps", INPUT,STAMPS);
                addPort("order", OUTPUT,STAMPS);

                addParameter<int>("min",0);
                addParameter<int>("max",INT_MAX);
                addParameter<int>("buffer",4);
                addParameter("stampname","it");

            };
	  
            virtual Component* create() const { return  new SyncGreedy(getId());};
    };
    

};}; // end namespace 

#endif // __FLOWVR_APP__SYNCGREEDY_H


