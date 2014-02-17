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
 * File: ./include/flowvr/app/components/filtermerge2d.comp.h      *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERMERGE2D_H
#define __FLOWVR_APP__FILTERMERGE2D_H

#include "flowvr/app/core/component.h"


/**
 * \file filtermerge2d.comp.h
 */

namespace flowvr
{
	namespace app
	{


		/**
		 * \class FilterMerge2D
		 * \brief Merge 2D data  filter: composite shell  encapsulating a FilterMerge2DPrimitive
		 */
		class FilterMerge2D : public Composite
		{
			public :

				/**
				 * \brief constructor with an id
				 */
				FilterMerge2D(const std::string& id_) : Composite(id_, "Shell")
				{
					setInfo("A shell around a filtermerge2d component masking  its 2 input ports ");
					addPort("in", INPUT);
					addPort("out", OUTPUT);
				};

				/**
				 * \brief add a FilterMerge2D primitive and links it to in and out ports.
				 */
				virtual void execute();


				virtual Component* create() const { return new FilterMerge2D(getId());};

		};
	}
} // end namespace

#endif // __FLOWVR_APP__FILTERMERGE2D_H


