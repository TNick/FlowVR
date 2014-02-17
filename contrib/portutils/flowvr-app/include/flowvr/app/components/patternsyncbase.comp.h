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
 * File: ./include/flowvr/app/components/patternsyncbase.comp.h    *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/com1ton.comp.h"
#include "flowvr/app/components/connection.comp.h"
#include "flowvr/app/components/filterpresignal.comp.h"
#include "flowvr/app/components/filterroutingnode.comp.h"
#include "flowvr/app/components/patternparallelfromports.comp.h"

#ifndef __FLOWVR_APP_PATTERNSYNCBASE_H_
#define __FLOWVR_APP_PATTERNSYNCBASE_H_

namespace flowvr { namespace app {
   

    /**
     * \file patternsyncbase.comp.h
     */

#define FILTERPREFIX "filter"
#define SYNCPREFIX  "sync"


    /**
     * \class PatternSyncBase
     *  A base pattern for synchronizer 
     * \tparam Filter_ :   filter component under the order of the synchronizer
     * \tparam Synchronizer_ : the synchronizer to be used
     */
        
    template<class Filter_, class Synchronizer_>
        class PatternSyncBase : public Composite
    {
        public :
            
            /**
             * \brief constructor
             * \param id_ id of the component
             */
            PatternSyncBase(const std::string& id_) : Composite(id_,"Communication")
            {
                setInfo("A pattern with  two templates defining one  synchronizer and one filter (n instances created if the context requires it). The synchronizer waits for messages on the sync port (a presignal is set on the way to avoid deadlocks). It also receives stamps messages on the stamps port. When it receives a sync message, it emits an orer message broadcatsed to the filters. Filters receive full messages from the in port, use the order to process these messages, and send the result on the out port. This pattern is classicaly used for greedy communications. Notice that communication patterns must be added on all ports. If the filter and the synchronizer have other ports they are automatically exposed at this level (port name are prefixed to avoid issues with identical port ids).");


                addPort("in", INPUT);
                addPort("out", OUTPUT);
                addPort("stamps", INPUT);
                addPort("sync", INPUT);
                    
                // Reproduce  other filter_ ports (add  'filter' prefix)
                Filter_ m("filter");
                for (ConstpPortIterator it = m.getPortBegin(); it != m.getPortEnd(); ++it)
                    {
                        if ((*it)->getId() != "stamps" &&
                            (*it)->getId() != "order" &&
                            (*it)->getId() != "sync" &&
                            (*it)->getId() != "in" &&
                            (*it)->getId() != "out" &&
                            (*it)->getId() != "endIt") 
                            addPort(FILTERPREFIX+(*it)->getId(), (*it)->getType());
                    }

                // Reproduce other syncrhonizer_ port ( (add  'sync' prefix)
                for (ConstpPortIterator it = m.getPortBegin(); it != m.getPortEnd(); ++it)
                    {
                        if ((*it)->getId() != "stamps" &&
                            (*it)->getId() != "order" &&
                            (*it)->getId() != "sync" &&
                            (*it)->getId() != "in" &&
                            (*it)->getId() != "out" &&
                            (*it)->getId() != "endIt")
                            addPort(SYNCPREFIX+(*it)->getId(), (*it)->getType());
                    }
            };

            /**
             * \brief destructor
             */
            virtual ~PatternSyncBase(){};
            
            /**
             * \brief mandatory virtual create method
             */
            virtual Component* create() const { return new PatternSyncBase(getId());};


            
            /**
             * \brief Execute method
             */
            virtual void execute()
            {
            	Synchronizer_ * sync  = addObject<Synchronizer_>("sync");
            	PatternParallelFromPorts<Filter_> * filter   = addObject<PatternParallelFromPorts<Filter_> >("filter");

            	// Number of filters is set according to instances on output port
            	filter->setParameter("instances_from_port", "out");

            	Com1ToN<FilterRoutingNode> * cOrder = addObject<Com1ToN<FilterRoutingNode> >("bcastOrder");


            	// Synchronization linkages

            	// Connect in port to filter in port
            	link(getPort("in"), filter->getPort("in"));

            	// Connect out  port to  filter  out
            	link(getPort("out"), filter->getPort("out"));

            	// Connect stamps port to  synchronizer stamps  port
            	link(getPort("stamps"), sync->getPort("stamps"));

            	// Connect sync order port to filter order port using a 1toN broadcast com
            	link(sync->getPort("order"), cOrder->getPort("in"));
            	link(cOrder->getPort("out"), filter->getPort("order"));

            	// Connect sync port to endit port of synchronizer  including a presignal filter on the way

            	// Actually we do not need the presignal as all synchronizers have the
            	// advance parameter set to 1 by default (starting token emission). See flowvr.plugins.Synchronizor.cpp
            	// Presignal required to unlock cycle.
            	//FilterPreSignal * filterPreSignal  = addObject<FilterPreSignal>("PreSignal");
            	//link(getPort("sync"), filterPreSignal->getPort("in"));
            	//link(filterPreSignal->getPort("out"),cSync->getPort("in"));
            	link(getPort("sync"),sync->getPort("endIt"));

            	// Connect  filter remaining ports  to parent ports
            	for (pPortIterator it = filter->getPortBegin(); it != filter->getPortEnd(); ++it)
            	{
            		if ((*it)->getId() != "stamps" &&
            				(*it)->getId() != "order" &&
            				(*it)->getId() != "sync" &&
            				(*it)->getId() != "in" &&
            				(*it)->getId() != "out" &&
            				(*it)->getId() != "endIt")
            			link(*it,getPort(FILTERPREFIX+(*it)->getId()));
            	}

            	// Connect  synchronizer  remaining ports  to parent ports
            	for (pPortIterator it = sync->getPortBegin(); it != sync->getPortEnd(); ++it)
            	{
            		if ((*it)->getId() != "stamps" &&
            				(*it)->getId() != "order" &&
            				(*it)->getId() != "sync" &&
            				(*it)->getId() != "in" &&
            				(*it)->getId() != "out" &&
            				(*it)->getId() != "endIt")
            			link(*it,getPort(SYNCPREFIX+(*it)->getId()));
            	}
            };

            /**
              * \brief host mapping
              # expect to find one host and map all enclosing components on that host.
              */
        	void setHosts()
        	{
                    // Filters are mapped through the patternparallefromhost
                    // simply put the sync on the first host found when looking for primitive sibling on the "out" port of the pattern.


                    Component * sync= this->getComponent("sync");
                    Port * p = this->getPort("out");

                    std::list<const Port*> primlist;
                    p->getPrimitiveSiblingListSkipConnections(primlist);

                    if ( primlist.empty() )
                        throw NoPrimitiveConnectedException("out",getFullId(), __FUNCTION_NAME__);
                    
                    const Port * sibling = *(primlist.begin());

                    if (sibling->getOwner())
                        {
                            if (!sibling->getOwner()->hostList.empty())
                                {
                                    sync->hostList.push_back(sibling->getOwner()->hostList.front());
                                    sync->hostList.front().appendtoOrigin(sibling->getOwner()->getFullId());// Set origin for new local host list
                                    Msg::debug("Host mapping : Component, Id = " + sync->getFullId() + " mapped on " + sync->hostList.front().getName(), __FUNCTION_NAME__);
                                }
                        }

                    if (sync->hostList.empty()) 
                        { 
                            Msg::warning("Host mapping : Tried to map component, Id = " + sync->getFullId() + " from " + getFullId() + " but no host found at this point", __FUNCTION_NAME__);
                        } 
                 }
    };

};};//namespaces

#endif //__FLOWVR_APP_PATTERNSYNCBASE_H_
