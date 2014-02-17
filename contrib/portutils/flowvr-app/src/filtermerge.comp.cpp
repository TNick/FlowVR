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
 *  File : ./src/filtermerge.comp.h                                *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/filtermerge.comp.h"
#include "flowvr/app/components/filtermergeprimitive.comp.h"

namespace flowvr { namespace app {

    void FilterMerge::execute()
    {
        Port* p = getPort("in");
        std::list<const Port*> primlist;
        p->getPrimitiveSiblingList(primlist);
        unsigned int numberInputs = primlist.size();
        if (numberInputs == 0)
            throw NoPrimitiveConnectedException("in", getFullId(), __FUNCTION_NAME__);
        FilterMergePrimitive  * fmerge =  addObject<FilterMergePrimitive>("Merge");

        fmerge->setParameter<unsigned int>("nb",numberInputs);

        fmerge->createPorts();

        unsigned int counter = 0;
        for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
            {
                link(p, fmerge->getPort("in" + toString<unsigned int>(counter)));
                fmerge->getPort("in" + toString<unsigned int>(counter))->addRestrictionToPrimitiveSibling(*it);
                counter++;
            }
        
        link(fmerge->getPort("out"), getPort("out"));
    }
      
  }; 
};

