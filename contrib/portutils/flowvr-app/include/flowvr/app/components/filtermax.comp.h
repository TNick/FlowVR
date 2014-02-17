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
 *  File : ./include/flowvr/app/components/filtermax.comp.h      *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP_FILTERMAX__
#define __FLOWVR_APP_FILTERMAX__

#include "flowvr/app/core/component.h"

/**
 * \file filtermax.comp.h
 * \brief Max filter
 */

namespace flowvr { namespace app {

    /**
     * \class FilterMax 
     * \brief Max filter: composite shell  encapsulating a FilterMergePrimitive 
     */
    class FilterMax : public Composite
    {
        public :
            /**
             * \brief constructor with an id
             */
            FilterMax(const std::string& id_) : Composite(id_, "Shell")
            {
                setInfo("A shell around a filtermax component with one input port  masking the actual number of ports  of filtermerge");
                addPort("in", INPUT,STAMPS);
                addPort("out", OUTPUT,STAMPS);
            }

            /**
             * \brief add a FilterMaxPrimitive with nb inputs 
             *  nb defined according to primitive objects connected to the in port of FilterMax. 
             */
            virtual void execute();

            virtual Component* create() const	{	return new FilterMax(getId());		};

    };

};};
#endif //__FILTERMAX
