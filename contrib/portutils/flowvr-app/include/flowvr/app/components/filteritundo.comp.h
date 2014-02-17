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
 * File: ./include/flowvr/app/components/filteritundo.comp.h     *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERITUNDO_H
#define __FLOWVR_APP__FILTERITUNDO_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filteritundo.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterItUndo
     * \brief 'Undo' filtering performed  by FilterIt
     **/

    class FilterItUndo : public Filter
    {
        public :
      
            FilterItUndo(const std::string& id_) : Filter(id_, "UnfilterIt") 
            {
	
                setInfo("'Undo' filtering performed  by FilterIt");
	
                addPort("in", INPUT,FULL);
                addPort("order", INPUT,STAMPS);
                addPort("stamps", INPUT,STAMPS);
                addPort("out", OUTPUT,FULL);
            };
      
            virtual Component* create() const {return new FilterItUndo(getId());};  
    };
    
};}; // end namespace 

#endif // __FLOWVR_APP__FILTERITUNDO_H


