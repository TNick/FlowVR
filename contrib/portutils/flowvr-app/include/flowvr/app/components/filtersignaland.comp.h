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
 *  File : ./include/flowvr/app/components/filtersignaland.h       *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP_SHELLFILTESIGNALAND__
#define __FLOWVR_APP_SHELLFILTESIGNALAND__

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/filtersignalandprimitive.comp.h"

/**
 * \file filtersignaland.comp.h
 * \brief FilterSIgnalAnd filter 
 */

namespace flowvr { namespace app {

    /**
     * \class FilterSignalAnd
     * \brief SignalAnd filter: composite shell encapsulating a FilterSignalAndPrimitive component
     */
    class FilterSignalAnd : public Composite
    {
        public :
            /**
             * \brief constructor with an id
             */
            FilterSignalAnd(const std::string& id_) : Composite(id_, "Shell")
            {
                setInfo("A shell around a filtersignaland component with one input port  masking the actual number of ports  of filtersignaland.");
                addPort("in", INPUT);
                addPort("out", OUTPUT);
                addParameter("nb");
            }

            /**
             * \brief add a FilterSignalAnd  with nb inputs
             *  nb defined according to primitive objects connected to the in port of FilterSignalAnd. 
             */
            virtual void execute();

            virtual Component* create() const	{	return new FilterSignalAnd(getId());		};
    };

		
};};
#endif //__FILTERSIGNALANDSHELL__
