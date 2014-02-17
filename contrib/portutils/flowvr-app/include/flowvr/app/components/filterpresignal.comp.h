/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
 * INRIA                                                           *
 * ALL RIGHTS RESERVED.	                                           *
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
 * File: ./include/flowvr/app/components/filterpresignal.comp.h    *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERPRESIGNAL_H
#define __FLOWVR_APP__FILTERPRESIGNAL_H

#include "flowvr/app/components/filter.comp.h"




/**
 * \file filterpresignal.comp.h
 * \brief implements pre-signal primitive component
 */

namespace flowvr { namespace app {


    /**
     * \class FilterPreSignal
     * \brief Add nb initial messages,
              then forward incoming messages to output  (used to boot cycles)
     */
    class FilterPreSignal : public Filter
    {
        public :
            FilterPreSignal(const std::string& id_) : Filter(id_, "PreSignal")
            {
                setInfo("Filter that adds nb (a parameter) initial messages, then forward incoming messages to output (used to boot cycles). 1 parameter. 'nb':  number of initial messages");

                addPort("in",INPUT,SAMEAS,"out"); // can handle  either full or stamp messages (depends on context)
                addPort("out",OUTPUT,AUTO); // will forward FULL or STAMP depending of what is expected


                // Number of  messages created and sent at starting time
                addParameter<int>("nb",1);

            }

            /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const { return  new FilterPreSignal(getId()); }

            /**
             * \brief Map the presignal filter on the same host than the first mapped  primitive it is connected to on its out port
             **/
            virtual void setHosts();
    };

}
} // end namespace

#endif // __FLOWVR_APP__FILTERPRESIGNAL_H


