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
 * File: ./include/flowvr/app/components/filterrotateprimitive.comp.h*
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERROTATE_H
#define __FLOWVR_APP__FILTERROTATE_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


/**
 * \file filterrotateprimitive.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterRotate
     * \brief Alternatively dispatch messages received on input on out0, out1, ..., outn.
     */
    class FilterRotatePrimitive : public Filter
    {
        public :

            FilterRotatePrimitive(const std::string& id_) : Filter(id_, "RotateFilter")
            {
                setInfo(" Alternatively dispatch messages received on input on out0, out1, ..., outn");

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

            virtual Component* create() const   { return new FilterRotatePrimitive(getId());}

    };


};}; // end namespace

#endif // __FLOWVR_APP__FILTERROTATE_H


