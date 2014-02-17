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
 *  File : ./src/filtermax.comp.h                                  *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/filtermax.comp.h"
#include "flowvr/app/components/filtermaxprimitive.comp.h"

namespace flowvr { namespace app {

    void FilterMax::execute()
    {
        Port* p = getPort("in");
        std::list<const Port*> primlist;
        p->getPrimitiveSiblingList(primlist);
        unsigned int numberInputs = primlist.size();
        if (numberInputs == 0)
            throw NoPrimitiveConnectedException("in", getFullId(), __FUNCTION_NAME__);

        FilterMaxPrimitive *  fmax =  addObject<FilterMaxPrimitive>("Max");
        fmax->setParameter("nb",toString<unsigned int>(numberInputs));
		
        fmax->createPorts();

        unsigned int counter = 0;
        for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
            {
                link(p, fmax->getPort("in" + toString<unsigned int>(counter)));
                fmax->getPort("in" + toString<unsigned int>(counter))->addRestrictionToPrimitiveSibling(*it);
                counter++;
            }

        link(fmax->getPort("out"), getPort("out"));
    }

}; };

