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


#ifndef __FLOWVR_APP__FILTERSTOP_H
#define __FLOWVR_APP__FILTERSTOP_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filterstop.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterStop
     * \brief Forward input messages on outputs only if open (0 received on open port means closed)
     **/

    class FilterStop : public Filter
    {
        public :
      
            FilterStop(const std::string& id_) : Filter(id_, "Stop") 
            {
	
                setInfo("Forward input messages on outputs only if open (0 received on open port means closed)");
	
                addPort("in", INPUT,SAMEAS,"out");//// can handle  either full or stamp messages (depends on context)
                addPort("open", INPUT,STAMPS);
                addPort("out", OUTPUT,AUTO);// will forward FULL or STAMP depending of what is expected
            };
      
            virtual Component* create() const {return new FilterStop(getId());};  
    };
    
};}; // end namespace 

#endif // __FLOWVR_APP__FILTERSTOP_H


