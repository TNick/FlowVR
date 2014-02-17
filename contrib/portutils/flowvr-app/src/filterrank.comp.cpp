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
* File: ./src/filterrank.comp.cpp                               *
*                                                                 *
* Contacts:                                                       *
*  01/06/2008 Helene Coullon <helene.coullon@univ-orleans.fr>     *
*                                                                 *
******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/filterrank.comp.h"
#include "flowvr/app/components/filterrankprimitive.comp.h"

namespace flowvr { namespace app {

	void FilterRank::execute()
    	{
		Port* p = getPort("out");
		std::list<const Port*> primlist;
		p->getPrimitiveSiblingList(primlist);
		unsigned int numberOutputs = primlist.size();
		if (numberOutputs == 0)
		    throw NoPrimitiveConnectedException("out", getFullId(), __FUNCTION_NAME__);


		FilterRankPrimitive * fsource = addObject<FilterRankPrimitive>("FilterRank");        
		
		fsource->setParameter("nb",toString<unsigned int>(numberOutputs));
		fsource->setParameter("elementsize",toString<unsigned int>(1));
		
		fsource->createPorts();
		
		unsigned int counter = 0;

		//link the output port to out0,out1, ..., outn
		for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
		    {
		        link(p, fsource->getPort("out" + toString<unsigned int>(counter)));
		        fsource->getPort("out" + toString<unsigned int>(counter))->addRestrictionToPrimitiveSibling(*it);
		        counter++;
		    }
		link(fsource->getPort("in"), getPort("in"));
		link(fsource->getPort("order"), getPort("order"));
   	}

}; };

