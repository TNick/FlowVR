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
 * File: ./src/filtermaskroutingprimitive.comp.cpp                   *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/components/filtermaskroutingprimitive.comp.h"


/**
 * \file filtermaskroutingprimitive.comp.cpp
 */

namespace flowvr
{
	namespace app
	{
		void  FilterMaskRoutingPrimitive::execute ()
		{
			// get parent input port
			const Component * parent = this->getParent();
			if ( ! parent  )
				throw CustomException("Parent of component id='"+getFullId()+"' is NULL", __FUNCTION_NAME__);


			// get the number of in ports
			int nb = getParameter<int>("nb");

			// create ports.
			Port * p;
			for (int i = 0; i < nb; ++i)
			{
				p = addPort("out" + toString<int> (i), OUTPUT, FULL);
				try
				{
					link(p, parent->getPort(getParameter<std::string> ("outport")));
				}
				catch (CustomException& e)
				{
					throw CustomLinkException("Port p=(" + getFullId() + ","
							+ p->getId() + "): unable to link to parent port p=("
							+ parent->getFullId() + "," + getParameter<std::string> (
							"outport") + ")", __FUNCTION_NAME__);
				}
			}
		}
		

	}
} // end namespace



