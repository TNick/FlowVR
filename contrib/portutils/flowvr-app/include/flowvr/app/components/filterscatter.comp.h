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


#ifndef __FILTERSCATTER_H
#define __FILTERSCATTER_H

#include "flowvr/app/core/component.h"





/**
 * \file filterscatter.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterScatter
     * \brief Split input message in nb parts of size elementsize and send each part to output out0, out1, ..., outNb-1
     * \if message contains data of type X, size of parts will be elementsize*sizeof(X)
     */
    class FilterScatter : public Composite
    {
        public :

            /**
             * \brief constructor with an id
             */
            FilterScatter(const std::string& id_) : Composite(id_, "Shell")
            {
                setInfo("A shell around a filterscatter component with one input port  masking the actual number of ports  of filterscatter");
                addPort("in", INPUT);
                addPort("out", OUTPUT);
            }

            /**
             * \brief add a FilterScatterPrimitive with nb inputs
             *  nb defined according to primitive objects connected to the in port of FilterScatter.
             */
            virtual void execute();

            virtual Component* create() const	{	return new FilterScatter(getId());		};

    };


};}; // end namespace

#endif // __FILTERSCATTER_H


