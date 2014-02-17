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
 * File: ./include/flowvr/app/components/filtermultiplex.comp.h      *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERMULTIPLEX_H
#define __FLOWVR_APP__FILTERMULTIPLEX_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"





/**
 * \file filtermultiplex.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMultiplex
     * \brief Each time it receives a message on each input port, it sends one message consisting of the stamp of the message received on in0 and
     *  the concatenation of the date received on in0, in1, ... inNb-1
     */
    class FilterMultiplexPrimitive : public Filter
    {
        public :

            FilterMultiplexPrimitive(const std::string& id_) : Filter(id_, "MultiplexFilter")
            {
                setInfo("OR-Filter, any message on any inport is forwarded on OUT");
                addPort("out", OUTPUT, AUTO );

                addParameter("nb");
            }

            void createPorts(unsigned int nb)
            {
            	setParameter<int>("nb", nb);

                for(unsigned int i = 0; i < nb; ++i)
					addPort("in"+toString<int>(i), INPUT, SAMEAS,"out");

            }

            virtual Component* create() const {  return  new FilterMultiplexPrimitive(getId()); }

    };


};}; // end namespace

#endif // __FLOWVR_APP__FILTERMULTIPLEX_H


