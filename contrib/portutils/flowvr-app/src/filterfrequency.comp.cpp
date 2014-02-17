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
 * File: ./srcfilterfrequency.comp.cpp                             *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/filterfrequency.comp.h"


/**
 * \file filterfrequency.comp.cpp
 */

namespace flowvr { namespace app {


void FilterFrequency::setHosts()
{
	Component* owner;

	std::list<const Port*> primitivePorts;
	getPort("out")->getPrimitiveSiblingList(primitivePorts);
	if (primitivePorts.empty())
		throw NoPrimitiveConnectedException("out", getFullId(), __FUNCTION_NAME__);
	else
	{
		bool nodeIsMapped = false;
		std::list<const Port*>::iterator it = primitivePorts.begin();
		do
		{
			// Look for the first primitive (connections are ignored)
			// with a non empty host list and use the host available to map the current component.
			owner = (*it)->getOwner();
			if (!owner->hostList.empty())
			{
				hostList.push_back(owner->hostList.front());
				hostList.front().appendtoOrigin(owner->getFullId());
				nodeIsMapped = true;
			}
			++it;
		}
		while( (!nodeIsMapped) && it != primitivePorts.end());

		if (!nodeIsMapped)
			throw CustomException("Frequency Filter id=" + getFullId() + ": No host for mapping (one expected)", __FUNCTION_NAME__);
	}
};

}} // end namespace
