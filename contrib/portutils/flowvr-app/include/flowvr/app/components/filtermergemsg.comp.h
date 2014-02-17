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
 * File: ./include/flowvr/app/components/filtermergemsg.comp.h   *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERMERGEMSG_H
#define __FLOWVR_APP__FILTERMERGEMSG_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      


/**
 * \file filtermergemsg.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterMergeMsg
     * \brief Merge nb successive messages received and send on output.
     *        Messages can be discarded using the stamp given with the scratch parameter.
     *        The stamp given with the stamp parameter is combined in the merged message.
     */
    class FilterMergeMsg : public Filter
    {
        public :

            FilterMergeMsg(const std::string& id_) : Filter(id_, "MergeMsg") 
            {
                setInfo("Merge nb successive messages received and send on output.\
                		 Messages can be discarded using the stamp given with the scratch parameter.\
                		 The stamp given with the stamp parameter is combined in the merged message.");

                addPort("in", INPUT,SAMEAS,"out");
                addPort("out", OUTPUT,AUTO);

                addParameter("stamp", ""); // optional argument
                addParameter("scratch", ""); // optional argument
                addParameter<int>("number", 2); // default mirrors implementation default
            };
	  
            virtual Component* create() const { return  new FilterMergeMsg(getId());};
    };
    

};}; // end namespace 

#endif // __FLOWVR_APP__FILTERMERGEMSG_H


