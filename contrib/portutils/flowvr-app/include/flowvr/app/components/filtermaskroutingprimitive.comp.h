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
 * File: ./include/flowvr/app/components/filtermaskroutingprimitive.h     *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTER_MASKROUTINGPRIMITIVE_H
#define __FLOWVR_APP__FILTER_MASKROUTINGPRIMITIVE_H

#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filtermaskrouting.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMaskRoutingPrimitive
     * \brief Route a message on one of the nb outputs according to a mask  applied on the mask stamp
     **/

    class FilterMaskRoutingPrimitive : public Filter
    {
        public :
            FilterMaskRoutingPrimitive(const std::string& id_) : Filter(id_, "MaskRouting") 
            {
                setInfo("Route a message on one of the nb outputs according to a mask  applied on the mask stamp. Should always be used in conjonction with the FilterMaskRouting shell component.  Non classical primitive component because the number of out ports  is not known when calling the constructor (build latter   when calling the execute method).    2 parameters. 'nb':  number of outputs.  'outport' : name of parent  port  to link to. ");

                addPort("in", INPUT,FULL);

                addParameter("outport");
                addParameter("nb");
            };

            /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const { return new  FilterMaskRoutingPrimitive(getId());};

            /**
             * \brief execute create on instance of primitive maskrouting filter with the expecetd number of outputs.
             */
            virtual void execute ();
    
    };

};}; // end namespace 

#endif // __FLOWVR_APP__FILTER_MASKROUTINGPRIMITIVE_H


