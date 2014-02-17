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
 * File: ./include/flowvr/app/components/filtermaskscatter.comp.h*
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERMASKSCATTER_H
#define __FLOWVR_APP__FILTERMASKSCATTER_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"





/**
 * \file filtermaskscatter.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMaskScatter
     * \brief Split input message in nb parts of size elementsize and send parts to output out0, out1, ..., outNb-1 according to mask
     */
    class FilterMaskScatter : public Filter
    {
        public :

            FilterMaskScatter(const std::string& id_) : Filter(id_, "MaskScatter")
            {
                setInfo("Split input message in nb parts of size elementsize and send parts to output out0, out1, ..., outNb-1 according to mask");

                addPort("in", INPUT,FULL);

                // Number of output   ports.
                addParameter("nb");
                //setParameter("nb","",REQ);

                // Number of output   ports.
                addParameter("elementsize");
                //setParameter("elementsize","",REQ);
            };

            void createPorts()
            {
                // get the number of in ports
                int nb = getParameter<int>("nb");

                for(int i = 0; i < nb; ++i)
                    {
                        addPort("out"+toString<int>(i), OUTPUT,FULL);
                    }

            };

            virtual Component* create() const { return new FilterMaskScatter(getId());};

    };


};}; // end namespace

#endif // __FLOWVR_APP__FILTERMASKSCATTER_H


