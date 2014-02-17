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
 * File: ./include/flowvr/app/components/filterfirstonly.comp.h  *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERFIRSTONLY_H
#define __FLOWVR_APP__FILTERFIRSTONLY_H

#include "flowvr/app/core/component.h"





/**
 * \file filterfirstonly.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterFirstOnly
     * \brief Forward only first port (it is a composite not a standard filter) 
     **/

    class FilterFirstOnly: public Composite
    {
        public:

            FilterFirstOnly(const std::string& id_): Composite(id_, "FilterFirstOnly")
            {
                addPort("in", INPUT);
                addPort("out", OUTPUT);
                setInfo("Forward only first port (it is a composite not a standard filter) ");
            }

            virtual Component* create() const { return new FilterFirstOnly(getId()); }

            virtual void execute();
	
    };


};}; // end namespace 

#endif // __FLOWVR_APP__FILTERFIRSTONLY_H


