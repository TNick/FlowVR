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
 * File: ./srcfiltermaskrouting.comp.cpp                             *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/
#include "flowvr/app/components/filtermaskrouting.comp.h"
#include "flowvr/app/components/filtermaskroutingprimitive.comp.h"

      


/**
 * \file filtermaskrouting.comp.cpp
 */

namespace flowvr { namespace app {
        
    void  FilterMaskRouting::execute ()
    {
        // set 'outport' parameter  FilterMaskroutingPrimitive will read to
        //  connect its 'out' ports to. 
        setParameter("outport","out");// output port to connect to 
        FilterMaskRoutingPrimitive * pr = addObject<FilterMaskRoutingPrimitive>("MaskRouting");// create filter
        link(getPort("in"),pr->getPort("in"));
    };
        
};}; // end namespace 



