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
 * File: ./include/flowvr/app/components/filterfrequency.comp.h  *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERFREQUENCY_H
#define __FLOWVR_APP__FILTERFREQUENCY_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"

/**
 * \file filterfrequency.comp.h
 * This filter sends messages at the given frequency
 */

namespace flowvr
{
	namespace app
	{

		/**
		 * \class FilterFrequency
		 * \brief Send messages at the given frequency (precision unknown)
		 **/

		class FilterFrequency : public Filter
		{
			public :

				FilterFrequency(const std::string& id_) : Filter(id_, "FrequencyFilter")
				{

					setInfo("Send messages at the given frequency (precision unknown)");

					addPort("out", OUTPUT,FULL);

					// target frequency
					addParameter("freq");
				};


				virtual Component* create() const {return new FilterFrequency(getId());}

				/**
				 * \brief Map the  filter on the same host than the first mapped  primitive it is connected to on its out port
				 **/
				virtual void setHosts();
		};
	}
} // end namespace

#endif // __FLOWVR_APP__FILTERFREQUENCY_H


