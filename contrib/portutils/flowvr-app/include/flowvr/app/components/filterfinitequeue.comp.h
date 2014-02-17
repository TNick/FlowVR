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
 * File: ./include/flowvr/app/components/filterdiscard.comp.h    *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTER_FINITEQUEUE_H
#define __FLOWVR_APP__FILTER_FINITEQUEUE_H

#include "flowvr/app/components/filter.comp.h"

/**
 *
 * \brief Implements the filter to maintain a finite number of slots/messages
 */

namespace flowvr
{
	namespace app
	{
		/**
		 * \class FilterFiniteQueue
		 * \brief the finite queue filter maintains a user definable number of messages in a queue,
		          and can deliver a number of the recent messages by request.
		 **/

		class FilterFiniteQueue : public Filter
		{
			public :

			FilterFiniteQueue(const std::string& id_) : Filter(id_, "FilterFiniteQueue")
			{
				setInfo("collect N messages at maximum and optionally forward them.");

				addPort("in", INPUT,SAMEAS,"out");
				addPort("trigger", INPUT,STAMPS);
				addPort("out", OUTPUT,AUTO);

				addParameter("queue_length", 1);
			}

				virtual Component* create() const {return new FilterFiniteQueue(getId());}
		};

	}
} // end namespace

#endif // __FLOWVR_APP__FILTER_FINITEQUEUE_H


