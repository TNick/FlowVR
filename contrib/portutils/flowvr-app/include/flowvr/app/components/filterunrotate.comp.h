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
 * File: ./include/flowvr/app/components/filterunrotate.comp.h   *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERUNROTATE_H
#define __FLOWVR_APP__FILTERUNROTATE_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"





/**
 * \file filtermerge.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterUnrotate
     * \brief Alternatively receive an message on each  input and forward it on output
     */
    class FilterUnrotate : public Filter
    {
        public :

            FilterUnrotate(const std::string& id_) : Filter(id_, "UnrotateFilter")
            {
                setInfo("Alternatively receive an message on each  input and forward it on output");

                addPort("out", OUTPUT,FULL);

                // Number of input  ports.
                addParameter("nb");
                //setParameter("nb","",REQ);
            };

            void createPorts()
            {
                // get the number of in ports
                int nb = getParameter<int>("nb");

                for(int i = 0; i < nb; ++i)
                    {
                        addPort("in"+toString<int>(i), INPUT,FULL);
                    }

            };

            virtual Component* create() const { return new FilterUnrotate(getId());};

    };


};}; // end namespace

#endif // __FLOWVR_APP__FILTERUNROTATE_H


