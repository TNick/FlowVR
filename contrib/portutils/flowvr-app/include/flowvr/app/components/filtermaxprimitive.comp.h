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
 * File: ./include/flowvr/app/components/filtermax.comp.h      *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERMAX_H
#define __FLOWVR_APP__FILTERMAX_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"





/**
 * \file filtermax.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMax
     * \brief Each time it receives a message on each input port, it sends the maximum value on output port (input data must be an unsigned int)
     */
    class FilterMaxPrimitive : public Filter
    {
        public :

            FilterMaxPrimitive(const std::string& id_) : Filter(id_, "Max")
            {
                setInfo("Each time it receives a message on each input port, it sends the maximum value on output port (input data must be an unsigned int)");

                addPort("out", OUTPUT,  STAMPS);

                // Number of input  ports.
                addParameter("nb");
            };

            void createPorts()
            {
                // get the number of in ports
                int nb = getParameter<int>("nb");
                for(int i = 0; i < nb; ++i)
                    {
                        addPort("in"+toString<int>(i), INPUT,  STAMPS);
                    }

            };

            virtual Component* create() const { return new FilterMaxPrimitive(getId()); }

    };


};}; // end namespace

#endif // __FLOWVR_APP__FILTERMAX_H


