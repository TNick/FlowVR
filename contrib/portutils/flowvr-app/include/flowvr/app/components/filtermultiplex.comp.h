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
 *  File : ./include/flowvr/app/components/filtermultiplex.comp.h      *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP_FILTERMULTIPLEX__
#define __FLOWVR_APP_FILTERMULTIPLEX__

#include "flowvr/app/core/component.h"

/**
 * \file filtermultiplex.comp.h
 * \brief Multiplex filter
 */

namespace flowvr { namespace app {

    /**
     * \class FilterMultiplex 
     * \brief Multiplex filter: composite shell  encapsulating a FilterMultiplexPrimitive 
     */
    class FilterMultiplex : public Composite
    {
        public :
            /**
             * \brief constructor with an id
             */
            FilterMultiplex(const std::string& id_) : Composite(id_, "Shell")
            {
                setInfo("A shell around a filtermultiplex component with one input port.");
                addPort("in",  INPUT);
                addPort("out", OUTPUT);
            }

            /**
             * \brief add a FilterMultiplexPrimitive with nb inputs 
             *  nb defined according to primitive objects connected to the in port of FilterMultiplex. 
             */
            virtual void execute();

            virtual Component* create() const	{ return new FilterMultiplex(getId());};

    };

};};
#endif //__FILTERMULTIPLEX
