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
 * File: ./include/flowvr/app/components/filterroutingnode.comp.h  *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERROUTINGNODE_H
#define __FLOWVR_APP__FILTERROUTINGNODE_H

#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filterroutingnode.comp.h
 * \brief implements routingnode primitve component
 */

namespace flowvr { namespace app {


    /**
     * \class FilterRoutingNode
     *  A filter for routing messages. It chooses the type of its ports (full/stamps) at runtime according to its neighbor
     *  components. 
     */
    class FilterRoutingNode : public Filter
    {
        public :
       
	
            /**
             * \brief constructor
             */
            FilterRoutingNode(const std::string& id_) : Filter(id_, "PreSignal") 
            {
                setInfo("A filter for routing messages (full or stamps). Just send each message received on 'in' port on  'out' port.  Uses the  FilterPreSignal with parameter 'nb' set to 0 (no presignal) by default. ");

                addPort("in",INPUT,SAMEAS,"out"); // can handle either full or stamp messages (depends on context)
                addPort("out",OUTPUT,AUTO); // will forward FULL or STAMP depending of what is expected


                // Number of  messages created and sent at starting time
                addParameter("nb","0");

            }
            /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const { return  new FilterRoutingNode(getId()); }
    };

}
} // end namespace

#endif // __FLOWVR_APP__FILTERROUTINGNODE_H


