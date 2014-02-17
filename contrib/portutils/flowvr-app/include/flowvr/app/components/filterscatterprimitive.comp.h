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
 * File: ./include/flowvr/app/core/objects/filterscatter.comp.h    *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERSCATTER_H
#define __FLOWVR_APP__FILTERSCATTER_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


/**
 * \file filterscatter.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterScatter
     * \brief Split input message in nb parts of size elementsize and send each part to output out0, out1, ..., outNb-1
     */
    class FilterScatterPrimitive : public Filter
    {
        public :

            FilterScatterPrimitive(const std::string& id_) : Filter(id_, "Scatter")
            {
                setInfo("Split input message in nb parts of size elementsize and send each part to output out0, out1, ..., outNb-1");

                addPort("in", INPUT);

                // Number of output   ports.
                addParameter("nb");

                //
                addParameter("elementsize");
            }

            void createPorts()
            {
                // get the number of in ports
                int nb = getParameter<int>("nb");
                
                for (int i = 0; i < nb; ++i)
                    {
                        addPort("out"+toString<int>(i), OUTPUT);
                    }
                
            }
            
            virtual Component* create() const { return  new FilterScatterPrimitive(getId()); }

    };
   }
} // end namespace

#endif // __FLOWVR_APP__FILTERSCATTER_H


