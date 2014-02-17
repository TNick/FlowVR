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


#ifndef __FLOWVR_APP__FILTERMERGE2DPRIMITIVE_H
#define __FLOWVR_APP__FILTERMERGE2DPRIMITIVE_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filtermerge2dprimitive.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMerge2D
     * \brief Each time it receives a message on each input port consisting of a 2D array, it sends one message consisting of the association of the 2 input 2D arrays. It uses the stamps P and N to define how to combine these arrays. 
     */
    class FilterMerge2DPrimitive : public Filter
    {
        public :

            FilterMerge2DPrimitive(const std::string& id_) : Filter(id_, "Merge2D") 
            {
                setInfo("Each time it receives a message on each input port consisting of a 2D array, it sends one message consisting of the association of the 2 input 2D arrays. It uses the stamps P and N to define how to combine these arrays.");

                addPort("in0", INPUT,FULL);
                addPort("in1", INPUT,FULL);
                addPort("out", OUTPUT,FULL);

            };
	  
            virtual Component* create() const { return new FilterMerge2DPrimitive(getId());};
	  
    };
    

};}; // end namespace 

#endif // __FLOWVR_APP__FILTERMERGE2DPRIMITIVE_H


