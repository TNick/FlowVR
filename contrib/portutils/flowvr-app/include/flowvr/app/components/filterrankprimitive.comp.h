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
 * File: ./include/flowvr/app/components/filterrankprimitive.comp.h      *
 *                                                                 *
 * Contacts:                                                       *
 *  01/06/2008 Helene Coullon <helene.coullon@univ-orleans.fr>     *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP__FILTERRANK_H
#define __FLOWVR_APP__FILTERRANK_H


#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


/**
 * \file filterrankprimitive.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterRankPrimitive
     * \brief Route a message on one of the outputs according to the source of the stamps
     **/

    class FilterRankPrimitive : public Filter
    {
        public :

	    FilterRankPrimitive(const std::string& id_) : Filter(id_, "FilterRank")
            {
                setInfo("Route a message on one of the nb outputs according to the rank stamp in the message of order port. Should always be used in conjonction with the FilterRank shell component.  Non classical primitive component because the number of out ports  is not known when calling the constructor");

                addPort("in", INPUT,FULL);
		addPort("order", INPUT, FULL);

                addParameter("nb");
                addParameter("elementsize");
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

            virtual Component* create() const   { return new FilterRankPrimitive(getId());}
    
    };

};}; // end namespace 

#endif // __FLOWVR_APP__FILTER_RANKPRIMITIVE_H


