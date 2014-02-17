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
 *  File : ./src/filtermerge2d.comp.h                              *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/filtermerge2d.comp.h"
#include "flowvr/app/components/filtermerge2dprimitive.comp.h"

namespace flowvr { namespace app {

    void FilterMerge2D::execute()
    {
        Port* p = getPort("in");
        std::list<const Port*> primlist;
        p->getPrimitiveSiblingList(primlist);
        unsigned int numberInputs = primlist.size();
        if (numberInputs != 2)
            throw  CustomException("Component "+getType()+" id="+getFullId()
                                   +" port id=in: connected to "+ toString<unsigned int>(numberInputs)+ " primitives (2 expected)"
                                   , __FUNCTION_NAME__);

        FilterMerge2DPrimitive  * fmerge =  addObject<FilterMerge2DPrimitive>("Merge2D");


        // link port in0 and in1
        unsigned int counter = 0;
        for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
            {
                link(p, fmerge->getPort("in" + toString<unsigned int>(counter)));
                fmerge->getPort("in" + toString<unsigned int>(counter))->addRestrictionToPrimitiveSibling(*it);
                counter++;
            }
        link(fmerge->getPort("out"), getPort("out"));
    }

}; };

