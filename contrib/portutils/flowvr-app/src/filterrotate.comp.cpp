/******* COPYRIGHT ************************************************
*                                                                 *
*                         FlowVR VRPN                             *
*                    FlowVR Coupling Modules                      *
*                                                                 *
*-----------------------------------------------------------------*
 * COPYRIGHT (C) 2003-2011                by                       *
* Laboratoire d'Informatique Fondamentale d'Orleans               *
* (EA 4022) ALL RIGHTS RESERVED.                                  *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING-LIB file for further information.                       *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Sebastien Limet,                                             *
*    Sophie Robert,                                               *
*    Emmanuel Melin,                                              *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: ./include/flowvr/app/components/filterrotate.comp.h       *
*                                                                 *
* Contacts:                                                       *
*  01/06/2008 Helene Coullon <helene.coullon@univ-orleans.fr>     *
*                                                                 *
******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/filterrotate.comp.h"
#include "flowvr/app/components/filterrotateprimitive.comp.h"

namespace flowvr { namespace app {

    void FilterRotate::execute()
    {
        Port* p = getPort("out");
        std::list<const Port*> primlist;
        p->getPrimitiveSiblingList(primlist);
        unsigned int numberOutputs = primlist.size();
        if (numberOutputs == 0)
            throw NoPrimitiveConnectedException("out", getFullId(), __FUNCTION_NAME__);


        FilterRotatePrimitive * frotate = addObject<FilterRotatePrimitive>("RotateFilter");        
        
        frotate->setParameter("nb",toString<unsigned int>(numberOutputs));
        frotate->setParameter("elementsize",toString<unsigned int>(1));
        
        
        frotate->createPorts();
        
        unsigned int counter = 0;

	//link the output port to out0,out1, ..., outn
        for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
            {
                link(p, frotate->getPort("out" + toString<unsigned int>(counter)));
                frotate->getPort("out" + toString<unsigned int>(counter))->addRestrictionToPrimitiveSibling(*it);
                counter++;
            }
        link(frotate->getPort("in"), getPort("in"));
    }

}; };

