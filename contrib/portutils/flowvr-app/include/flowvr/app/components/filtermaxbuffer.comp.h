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
 * File: ./include/flowvr/app/components/filtermaxbuffer.h       *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTER_MAXBUFFER_H
#define __FLOWVR_APP__FILTER_MAXBUFFER_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filtermaxbuffer.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMaxBuffer
     * \brief When receiving a signal with stamp i, remove  i messages on portIt, and send  on output a message with stamp i+nb (nb given as parameter)
     **/

    class FilterMaxBuffer : public Filter
    {
        public :

            FilterMaxBuffer(const std::string& id_) : Filter(id_, "MaxBuffer") 
            {
	  
                setInfo("When receiving a signal with stamp i, remove  i messages on portIt, and send  on output a message with stamp i+nb (nb given as parameter)");
	  
                // stamp ports
                addPort("portIt", INPUT,STAMPS);
                addPort("signal", INPUT,STAMPS);
                addPort("out", OUTPUT,STAMPS);
	  
                addParameter("nb", "NULL");
                //setParameter("nb",NULL,REQ);
            }
      

            virtual Component* create() const { return new FilterMaxBuffer(getId());};
      
    };
    
};}; // end namespace 

#endif // __FLOWVR_APP__FILTER_MAXBUFFER_H


