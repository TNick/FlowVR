/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
 * INRIA                                                           *
 * ALL RIGHTS RESERVED.                                            *
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
 *  File : ./src/filterfirstonly.comp.h                            *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/connection.comp.h"
#include "flowvr/app/components/filterfirstonly.comp.h"
#include "flowvr/app/components/filterroutingnode.comp.h"

namespace flowvr { namespace app {

    void FilterFirstOnly::execute()
    {
        
        std::list<const Port*> primlist;
        getPort("in")->getPrimitiveSiblingList(primlist);
        if( primlist.empty() )
            {
                throw NoPrimitiveConnectedException("in", getFullId(), __FUNCTION_NAME__); 
            }
        std::list<const Port*>::const_iterator it = primlist.begin();
        FilterRoutingNode * node  = addObject<FilterRoutingNode>("node");
        
        link(getPort("in"), node->getPort("in"));
        node->getPort("in")->addRestrictionToPrimitiveSibling(*it);
        
        link(node->getPort("out"), getPort("out"));

        ++it;
        unsigned int i = 0;
        while(it !=  primlist.end() )
            {
                if(dynamic_cast<Connection*>((*it)->getOwner()) || dynamic_cast<ConnectionStamps*>((*it)->getOwner()))
                    {
                        // need a routing node at the end of the connection
                        FilterRoutingNode * routingNode  = addObject<FilterRoutingNode>("endConnection"+toString<unsigned int>(i++));
                        link(getPort("in"),routingNode->getPort("in"));
                        routingNode->getPort("in")->addRestrictionToPrimitiveSibling(*it);

                    }
                ++it;
            }

    } 

}; };

