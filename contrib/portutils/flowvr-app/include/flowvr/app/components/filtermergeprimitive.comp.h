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
 * File: ./include/flowvr/app/components/filtermerge.comp.h      *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERMERGE_H
#define __FLOWVR_APP__FILTERMERGE_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"





/**
 * \file filtermerge.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMerge
     * \brief Each time it receives a message on each input port, it sends one message consisting of the stamp of the message received on in0 and
     *  the concatenation of the date received on in0, in1, ... inNb-1
     */
    class FilterMergePrimitive : public Filter
    {
        public :

            FilterMergePrimitive(const std::string& id_) : Filter(id_, "Merge")
            {
                setInfo("Each time it receives a message on each input port, it sends one message consisting of the stamp of the message received on in0 and  the concatenation of the date received on in0, in1, ... inNb-1");

                addPort("out", OUTPUT);

                // Number of input  ports.
                addParameter("nb");

		// Stamp to merge
		addParameter("stamp","");
            };

            void createPorts()
            {
                // get the number of in ports
                int nb = getParameter<int>("nb");
                for(int i = 0; i < nb; ++i)
                    {
                        addPort("in"+toString<int>(i), INPUT);
                    }

            };

            virtual Component* create() const { return  new FilterMergePrimitive(getId()); }

    };


};}; // end namespace

#endif // __FLOWVR_APP__FILTERMERGE_H


