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


#ifndef __FLOWVR_APP_FILTERMERGEINDEXPOS__
#define __FLOWVR_APP_FILTERMERGEINDEXPOS__

#include "flowvr/app/core/component.h"

/**
 * \file filtermergeindexpos.comp.h
 * \brief MergeIndexPos filter
 */

namespace flowvr { namespace app {

    /**
     * \class FilterMergeIndexPos 
     * \brief MergeIndexPos filter: composite shell  encapsulating a FilterMergeIndexPosPrimitive 
     */
    class FilterMergeIndexPos : public Composite
    {
        public :
            /**
             * \brief constructor with an id
             */
            FilterMergeIndexPos(const std::string& id_) : Composite(id_, "Shell")
            {
                setInfo("A shell around a filtermergeindexpos component with one input port  masking the actual number of ports  of filtermergeindexpos");
                addPort("in", INPUT,FULL);
                addPort("out", OUTPUT,FULL);
            }

            /**
             * \brief add a FilterMergePrimitive with nb inputs 
             *  nb defined according to primitive objects connected to the in port of FilterMerge. 
             */
            virtual void execute();

            virtual Component* create() const	{	return new FilterMergeIndexPos(getId());		};

    };

};};
#endif //__FILTERMERGEINDEXPOS
