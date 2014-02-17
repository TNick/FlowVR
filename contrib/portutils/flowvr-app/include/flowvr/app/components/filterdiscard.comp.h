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
 * File: ./include/flowvr/app/components/filterdiscard.comp.h    *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTER_DISCARD_H
#define __FLOWVR_APP__FILTER_DISCARD_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filterdiscard.comp.h
 * \brief Implements the Discard  filter
 */

namespace flowvr { namespace app {


    /**
     * \class FilterDiscard
     * \brief filter discarding all messages when not open
     **/

    class FilterDiscard : public Filter
    {
        public :
      
            FilterDiscard(const std::string& id_) : Filter(id_, "Discard") 
            {
	
                setInfo("Discard all messages from input  if filter not open, forward them all on output otherwise");
	
                addPort("in", INPUT,SAMEAS,"out");
                addPort("open", INPUT,STAMPS);
                addPort("out", OUTPUT,AUTO);
            };
      
            virtual Component* create() const {return new FilterDiscard(getId());};  
    };
    
};}; // end namespace 

#endif // __FLOWVR_APP__FILTER_DISCARD_H


