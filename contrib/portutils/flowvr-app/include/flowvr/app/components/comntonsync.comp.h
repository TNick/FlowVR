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
 *  File : ./include/flowvr/app/components/comntonsync.comp.h      *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP_COMNTONSYNC_H_
#define __FLOWVR_APP_COMNTONSYNC_H_

#include <flowvr/app/components/connection.comp.h>
#include <flowvr/app/components/com1ton.comp.h>
#include <flowvr/app/components/comnto1.comp.h>
#include <flowvr/app/components/comnton.comp.h>
#include <flowvr/app/components/filterincrement.comp.h>
#include <flowvr/app/components/filtermax.comp.h>
#include <flowvr/app/components/filterrenum.comp.h>
#include <flowvr/app/components/filterroutingnode.comp.h>
#include <flowvr/app/components/patternparallelfromports.comp.h>

/**
 * \file comnto1.comp.h
 * \brief N to 1 communication  pattern 
 */


namespace flowvr { namespace app {


    /**
     * \class ComNtoN
     * \brief Communication patern N ("in" port) to 1 ("out" port) 
     */


    // For backward compatibility
    #define ComNtoNSync ComNtoNSync

    template <class Connect>
        class ComNtoNSync : public Composite
    {

        public :

            /**
             * \brief Constructor. N (in port) to N (out port)  communication pattern based on a tree. We assume all message are synchronized (same frequencies) and the stamps it is set to max(it) of sources.
             * \tparam Connect : communication component to use for each point-to-point connection
             * \param id_ the component id
             */
            ComNtoNSync(const std::string& id_) : Composite(id_)
            {
                Component::setInfo("N to N (point-to-point) communication. 1 parameter (inherited from PatternParallel). 'instances_from_port' : name of the port to look at to define N (number of primitive components connected). 1 template. 'Connect': the communication component to use for each point-to-point connection");
                addPort("in", INPUT);
                addPort("out", OUTPUT);
            };

            /**
             * \brief destructor
             */
            virtual ~ComNtoNSync(){};

            /**
             * \brief the execute method
             */
            virtual void execute();

            /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const { return new ComNtoNSync(this->getId()); };

    };

    template <class Connect>
        void ComNtoNSync<Connect>::execute()
    {
        // Add the N To N connections
        ComNtoN<Connect>* cIn = addObject<ComNtoN<Connect> >("cIn");
        ComNtoN<Connect>* cOut = addObject<ComNtoN<Connect> >("cOut");

        link(getPort("in"), cIn->getPort("in"));
        link(cOut->getPort("out"), getPort("out"));

        // Add the renum filters
        PatternParallelFromPorts<FilterRenum>* filtersRenum = addObject<PatternParallelFromPorts<FilterRenum> >("filtersrenum");
        link(cIn->getPort("out"), filtersRenum->getPort("in"));
        link(filtersRenum->getPort("out"), cOut->getPort("in"));
 
        // Max It from sources
        ComNto1<FilterMax>* max = addObject<ComNto1<FilterMax> >("max");
        link(getPort("in"), max->getPort("in"));
 
        // Broadcast the resut to filterit
        Com1toN<FilterRoutingNode>* cNewIt = addObject<Com1toN<FilterRoutingNode> >("cNewIt");
        link(max->getPort("out"), cNewIt->getPort("in"));
        link(cNewIt->getPort("out"), filtersRenum->getPort("it"));
 
        // Assume it always increases (compare with previous+1)
        FilterIncrement* filterinc = addObject<FilterIncrement>("filterinc");
        link(cNewIt->getPort("out"), filterinc->getPort("in"));
        link(filterinc->getPort("out"), max->getPort("in"));
 

    }

}; };

#endif //__COMNTONSYNC_H_
