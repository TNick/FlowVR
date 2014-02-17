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
 * File: ./include/flowvr/app/components/filtermaskrouting.h     *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTER_MASKROUTING_H
#define __FLOWVR_APP__FILTER_MASKROUTING_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filtermaskrouting.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMaskRouting
     * \brief Route a message on one of the nb outputs according to a mask  applied on the mask stamp
     **/

    class FilterMaskRouting : public Composite
    {
        public :
            /**
             * \brief constructor with an id
             */
            FilterMaskRouting(const std::string& id_) : Composite(id_,"Shell") 
            {
                setInfo("A shell for FilterMaskRoutingPrimitive. Set the and 'outport' parameter for the MaskRoutingPrimitive filter.  MaskRouting Filter should always be used through  this component rather than  directly with FilterMaskRoutingPrimitive");
                addPort("in", INPUT);
                addPort("out", OUTPUT);
            };
	  
            /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const { return  new FilterMaskRouting(getId());};



            /**
             * \brief execute creates on instance of primitive presignal 
             */
            virtual void execute ();


    };

};}; // end namespace 

#endif // __FLOWVR_APP__FILTER_MASKROUTING_H


