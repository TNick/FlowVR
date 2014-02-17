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
 *  File : ./include/flowvr/app/components/filtermerge.comp.h      *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP_FILTERMERGE__
#define __FLOWVR_APP_FILTERMERGE__

#include "flowvr/app/core/component.h"

/**
 * \file filtermerge.comp.h
 * \brief Merge filter
 */

namespace flowvr { namespace app {

    /**
     * \class FilterMerge 
     * \brief Merge filter: composite shell  encapsulating a FilterMergePrimitive 
     */
    class FilterMerge : public Composite
    {
        public :
            /**
             * \brief constructor with an id
             */
            FilterMerge(const std::string& id_) : Composite(id_, "Shell")
            {
                setInfo("A shell around a filtermerge component with one input port  masking the actual number of ports  of filtermerge");
                addPort("in", INPUT,FULL);
                addPort("out", OUTPUT,FULL);
            }

            /**
             * \brief add a FilterMergePrimitive with nb inputs 
             *  nb defined according to primitive objects connected to the in port of FilterMerge. 
             */
            virtual void execute();

            virtual Component* create() const	{	return new FilterMerge(getId());		};

    };

};};
#endif //__FILTERMERGE
