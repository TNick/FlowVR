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
 * File: ./include/flowvr/app/components/syncmaxfrequency.comp.h *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__MAXFREQUENCY_H
#define __FLOWVR_APP__MAXFREQUENCY_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/sync.comp.h"


/**
 * \file syncmaxfrequency.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class SyncMaxFrequency
     * \brief Send a first message  and then send output messages  at min (incoming message rate, freq rate) 
     */
    class SyncMaxFrequency : public Sync
    {
        public :

            SyncMaxFrequency(const std::string& id_) : Sync(id_, "MaxFrequencySynchronizor") 
            {
                setInfo("Send a first message  and then send output messages  at min (incoming message rate, freq rate) ");


	      
                addPort("endIt", INPUT,STAMPS); // stamp port
                addPort("out", OUTPUT,STAMPS); // stamp  port

                addParameter("freq");

            };
	  
            virtual Component* create() const { return  new SyncMaxFrequency(getId());};



			/**
			 * \brief Map the  filter on the same host than the first mapped  primitive it is connected to on its out port
			 **/
            virtual void setHosts()
            {
         	   Component* owner;

         	   std::list<const Port*> primitivePorts;
         	   getPort("endIt")->getPrimitiveSiblingList(primitivePorts);
         	   if (primitivePorts.empty())
         		   throw NoPrimitiveConnectedException("endIt", getFullId(), __FUNCTION_NAME__);
         	   else
         	   {
         		   bool nodeIsMapped = false;
         		   std::list<const Port*>::iterator it = primitivePorts.begin();
         		   do
         		   {
         			   // Look for the first primitive (connections are ignored)
         			   // with a non empty host list and use the host available to map the current component.
         			   owner = (*it)->getOwner();
         			   if (!owner->hostList.empty())
         			   {
         				   hostList.push_back(owner->hostList.front());
         				   hostList.front().appendtoOrigin(owner->getFullId());
         				   nodeIsMapped = true;
         			   }
         			   ++it;
         		   }
         		   while( (!nodeIsMapped) && it != primitivePorts.end());

         		   if (!nodeIsMapped)
         			   throw CustomException("Max Frequency Synchronizer id=" + getFullId() + ": No host for mapping (one expected)", __FUNCTION_NAME__);
         	   }
            };

    };
    

};}; // end namespace 

#endif // __FLOWVR_APP__MAXFREQUENCY_H


