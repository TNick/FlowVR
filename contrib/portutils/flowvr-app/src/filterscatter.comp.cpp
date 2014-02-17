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
 *  File : ./src/filterscatter.comp.h                                *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/filterscatter.comp.h"
#include "flowvr/app/components/filterscatterprimitive.comp.h"

namespace flowvr { namespace app {

    void FilterScatter::execute()
    {
        Port* p = getPort("out");
        std::list<const Port*> primlist;
        p->getPrimitiveSiblingList(primlist);
        unsigned int numberOutputs = primlist.size();
        if (numberOutputs == 0)
            throw NoPrimitiveConnectedException("out", getFullId(), __FUNCTION_NAME__);
        //        FilterScatterPrimitive scatter("Scatter");


        FilterScatterPrimitive * fscatter = addObject<FilterScatterPrimitive>("Scatter");
        
     
        
        fscatter->setParameter("nb",toString<unsigned int>(numberOutputs));
        fscatter->setParameter("elementsize",toString<unsigned int>(1));
        
        fscatter->createPorts();
        
        unsigned int counter = 0;
       for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
            {
                link(p, fscatter->getPort("out" + toString<unsigned int>(counter)));
                fscatter->getPort("out" + toString<unsigned int>(counter))->addRestrictionToPrimitiveSibling(*it);
                counter++;
            }
       link(fscatter->getPort("in"), getPort("in"));
    }

}; };

