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
 * File: ./include/flowvr/app/components/filtermaskmerge.comp.h  *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERMASKMERGE_H
#define __FLOWVR_APP__FILTERMASKMERGE_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"

/**
 * \file filtermaskmerge.comp.h
 */

namespace flowvr
{
	namespace app
	{
		/**
		 * \class FilterMaskMerge
		 * \brief Each time it receives a message on each input port, it sends one message consisting of a combinaison of nb messages receved on inputs in0, ..., innb-1 (combining order given by the mask). Ouput message has stamp of first input message optionally adding the stamp given in parameter.
		 */
		class FilterMaskMerge : public Filter
		{
			public :

				FilterMaskMerge(const std::string& id_) : Filter(id_, "MaskMerge")
				{
					setInfo("Each time it receives a message on each input port, it sends one message consisting of a combination of nb messages received on inputs in0, ..., innb-1 (combining order given by the mask). Ouput message has stamp of first input message optionally adding the stamp given in parameter.");

					addPort("mask", INPUT,FULL);
					addPort("out", OUTPUT,FULL);

					// Number of input  ports.
					addParameter("nb");

					// stamps to combine
					addParameter("stamp");
				}

				void createPorts()
				{
					// get the number of in ports
					int nb = getParameter<int>("nb");

					for(int i = 0; i < nb; ++i)
						{
							addPort("in"+toString<int>(i), INPUT);
						}

				}

				virtual Component* create() const { return new FilterMaskMerge(getId());};

		};


	}
} // end namespace

#endif // __FLOWVR_APP__FILTERMASKMERGE_H


