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
 * File:                                                           *
 *./include/flowvr/app/components/filtersignalandprimitive.comp.h  *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERSIGNALAND_H
#define __FLOWVR_APP__FILTERSIGNALAND_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"





/**
 * \file filtersignalandprimitive.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterSignalAnd
     * \brief Wait for receiving one message per input (nb inputs), and then send  on output (use stamp  read from message on in0)
     */
    class FilterSignalAndPrimitive : public Filter
    {
        public :

            FilterSignalAndPrimitive(const std::string& id_) : Filter(id_, "SignalAnd")
            {
                setInfo("Wait for receiving one message per input (nb inputs), and then send  on output (use stamp  read from message on in0)");

                addPort("out", OUTPUT,STAMPS);

                // Number of input  ports.
                addParameter("nb");
            };

            void createPorts()
            {
                // get the number of in ports
                int nb = getParameter<int>("nb");

                for(int i = 0; i < nb; ++i)
                    {
                        addPort("in"+toString<int>(i), INPUT, STAMPS);
                    }

            };

            virtual Component* create() const { return  new FilterSignalAndPrimitive(getId()); }

    };


};}; // end namespace

#endif // __FLOWVR_APP__FILTERSIGNALAND_H


