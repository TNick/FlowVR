/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                         Base Libraries                          *
 *                                                                 *
 *-----------------------------------------------------------------*
 * COPYRIGHT (C) 2003-2011                by                       *
 * INRIA and                                                       *
 * Laboratoire d'Informatique Fondamentale d'Orleans               *
 * (FRE 2490). ALL RIGHTS RESERVED.                                *
 *                                                                 *
 * This source is covered by the GNU LGPL, please refer to the     *
 * COPYING file for further information.                           *
 *                                                                 *
 *-----------------------------------------------------------------*
 *                                                                 *
 *  Original Contributors:                                         *
 *    Jeremie Allard,                                              *
 *    Ronan Gaugne,                                                *
 *    Valerie Gouranton,                                           *
 *    Loick Lecointre,                                             *
 *    Sebastien Limet,                                             *
 *    Bruno Raffin,                                                *
 *    Sophie Robert,                                               *
 *    Emmanuel Melin.                                              *
 *                                                                 *
 *******************************************************************
 *                                                                 *
 * File: ./include/flowvr/app/components/filterrenum.comp.h      *
 *    							          *
 * Contacts:                                                       *
 *  25/06/2008 khalid Zaida <khalid.zaida@gmail.com>               *
 ******************************************************************/

#ifndef __FLOWVR_APP__FILTERRENUM_H
#define __FLOWVR_APP__FILTERRENUM_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"
   
/**
 * \file filterrenum.comp.h
 */

namespace flowvr { namespace app {

    /**
     * \class FilterRenum
     * \brief forward incoming message to output  
     */
    class FilterRenum : public Filter
    {
        public :

            FilterRenum(const std::string& id_) : Filter(id_, "FilterRenum") 
            {
                setInfo("renum it");
                addPort("in", INPUT);
                addPort("it", INPUT,  STAMPS);
                addPort("out", OUTPUT);	      
            };
	  
            virtual Component* create() const { return  new FilterRenum(getId());};
    };
    

};}; // end namespace 

#endif // __FLOWVR_APP__FILTERRENUM_H


