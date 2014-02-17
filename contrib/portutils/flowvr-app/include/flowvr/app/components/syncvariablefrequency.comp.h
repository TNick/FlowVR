/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
 * COPYRIGHT (C) 2003-2011                by                       *
 * Laboratoire d'Informatique Fondamentale d'Orleans               *
 * (EA 4022) ALL RIGHTS RESERVED.                                  *
 *                                                                 *
 * This source is covered by the GNU LGPL, please refer to the     *
 * COPYING-LIB file for further information.                       *
 *                                                                 *
 *-----------------------------------------------------------------*
 *                                                                 *
 *  Original Contributors:                                         *
 *    Sebastien Limet                                              *
 *    Sophie Robert                                                *
 * 	  Yoann Kernoa												   *
 *                                                                 *
 *******************************************************************
 *                                                                 *
 * File: ./include/flowvr/app/components/syncvariablefrequency.comp.h *
 *                                                                 *
 * Contacts:                                                       *
 *     22/07/2009 Y.Kernoa  <yoann.kernoa@etu.univ-orleans.fr>     *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__VARFREQUENCY_H
#define __FLOWVR_APP__VARFREQUENCY_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/sync.comp.h"


/**
 * \file syncvariablefrequency.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class SyncVariableFrequency
     * \brief Send a first message and then send output messages at min (incoming message rate, freq rate) \
     *        Besides, the frequency rate can be modified during all the time the application is running (via the "freq" input)
     */
    class SyncVariableFrequency : public Sync
    {
        public :

            SyncVariableFrequency(const std::string& id_) : Sync(id_, "VariableFrequencySynchronizor") 
            {
                setInfo("Send a first message and then send output messages at min (incoming message rate, freq rate) - Besides, the frequency rate can be modified during all the time the application is running ");

				addPort("freq", INPUT,STAMPS);  // stamp port (contains the frequency :new,min or max)
                addPort("endIt", INPUT,STAMPS); // stamp port
                addPort("out", OUTPUT,STAMPS);  // stamp port

                addParameter("freq");

            };
	  
            virtual Component* create() const { return  new SyncVariableFrequency(getId());};
 
            virtual void setHosts()
            {
            	Port *p = getPort("endIt");

            	std::list<const Port*> primlist;
            	p->getPrimitiveSiblingList(primlist);


            	if ( primlist.empty() )
            		throw  NoPrimitiveConnectedException(p->getId(),getFullId(),__FUNCTION_NAME__);

            	const Port* sibling = *(primlist.begin());
            	p = sibling->getOwner()->getPort("in");
            	if (  p == NULL)
            		throw CustomException("Component "+sibling->getOwner()->getFullId()+": port "+p->getId()+" not found",__FUNCTION_NAME__);

            	p->getPrimitiveSiblingList(primlist);
            	if (  primlist.empty() )
            		throw  NoPrimitiveConnectedException(sibling->getId(),sibling->getOwner()->getFullId(),__FUNCTION_NAME__);


            	sibling = *(primlist.begin());

            	if(sibling->getOwner()->hostList.empty())
            	{
            		throw CustomException("SyncVariableFrequency "+getFullId()+": no host for mapping (one expected)", __FUNCTION_NAME__);
            	}
            	hostList = sibling->getOwner()->hostList;// one host list
            	hostList.front().appendtoOrigin(sibling->getOwner()->getFullId());
            }

    };
};}; // end namespace 

#endif // __FLOWVR_APP__VARFREQUENCY_H


