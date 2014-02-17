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
 *  Original Contributors:  
 *    Helene Coullon,                                              *
 *    Sebastien Limet,                                             *
 *    Sophie Robert,                                               *
 *    Emmanuel Melin,                                              *
 *                                                                 * 
 *******************************************************************
 *                                                                 *
 * File: ./include/flowvr/app/components/filterrank.comp.h       *
 *                                                                 *
 * Contacts:                                                       *
 *  01/06/2008 Helene Coullon <helene.coullon@univ-orleans.fr>     *
 *                                                                 *
 ******************************************************************/

#ifndef __FILTERRANK_H
#define __FILTERRANK_H

#include "flowvr/app/core/component.h"

/**
 * \file filterrank.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterSource
     * \first iteration, gives work to every instance of the output module
     * \then brief Route a message to the module source
     **/

    class FilterRank : public Composite
    {
        public :

	    FilterRank(const std::string& id_) : Composite(id_, "Shell")
            {
		setInfo("This filter gives work to the module the stamp rank of the message in order indicates. FilterRank should always be used through  this component rather than  directly with FilterRankPrimitive");

                addPort("in", INPUT);
		addPort("order", INPUT, FULL);
                addPort("out", OUTPUT);

            }

            /**
             * \brief add a FilterrotatePrimitive with nb inputs
             *  nb defined according to primitive objects
             */
            virtual void execute();

            virtual Component* create() const	{	return new FilterRank(getId());		};


    };

};}; // end namespace 

#endif // __FILTERRANK_H


