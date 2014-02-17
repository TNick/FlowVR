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
 * File: ./include/flowvr/app/components/comsync.comp.h            *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/patternsyncbase.comp.h"

#ifndef __FLOWVR_APP_COMSYNC_H_
#define __FLOWVR_APP_COMSYNC_H_

namespace flowvr { namespace app {


    /**
     * \file comsync.comp.cpp
     */


    /**
     * \class ComSync
     *  A base pattern for synchronizer
     * \tparam ComIn_ : input communication pattern
     * \tparam ComStamps_ : input communication pattern to forward stamps to synchronizer
     * \tparam ComOut_ : output communication pattern (output from filters)
     * \tparam ComEndIt_ : communication pattern to get sync signal to the synchronizer
     * \tparam Filter_ :   filter component under the order of the synchronizer
     * \tparam Synchronizer_ : the synchronizer to be used
     */



    template<class ComIn_, class ComStamps_, class ComOut_, class ComEndIt_, class Filter_, class Synchronizer_>
        class ComSync : public Composite
    {

        public :


            /**
             * \brief constructor
             * \param id_ id of the component
             */
            ComSync(const std::string& id_)
            : Composite(id_, "Communication")
            {
                setInfo("A communication pattern using the PatternSync component with 6 template parameters. \
                		'ComIn_':  the communication pattern on the in port; \
                		'ComStamps_': the communication pattern to extract stamps from the in port and send them to the synchronizer; \
                		'ComOut_' : the communication pattern on the out port; \
                		'ComEndIt_' : the incoming communication pattern to receive  sync signals \
                		on the sync port; \
                		'Filter_' : the filter type to use; \
                		'Synchronizer': the synchronizer type to use. Classically used to set  greedy patterns. \
                		All other ports of the PatternSync are reproduced on the parent");

                addPort("in", INPUT);
                addPort("out", OUTPUT);
                addPort("sync", INPUT);


                // Reproduce  other patternsync ports
                PatternSyncBase<Filter_,Synchronizer_> pat("patternsync");
                for (ConstpPortIterator it = pat.getPortBegin(); it != pat.getPortEnd(); ++it)
                    {
                        if ((*it)->getId() != "stamps" &&
                            (*it)->getId() != "sync" &&
                            (*it)->getId() != "in" &&
                            (*it)->getId() != "out")
                            addPort((*it)->getId(), (*it)->getType());
                    }
            }

            /**
             * \brief destructor
             */
            virtual ~ComSync() {}


            /**
             * \brief mandatory virtual create method
             */
            virtual Component* create() const { return new ComSync(getId()); }

            /**
             * \brief Execute method
             */
            virtual void execute()
            {
                PatternSyncBase<Filter_,Synchronizer_> * patternSync   = addObject<PatternSyncBase<Filter_,Synchronizer_> >("patternSync");

                ComIn_ * cIn   = addObject<ComIn_>("ComIn");
                ComOut_ * cOut   = addObject<ComOut_>("ComOut");
                ComStamps_ * cStamps   = addObject<ComStamps_>("ComStamps");
                ComEndIt_ * cSync   = addObject<ComEndIt_>("ComSync");

                // Synchronization linkages

                // Set in communication
                link(getPort("in"), cIn->getPort("in"));
                link(cIn->getPort("out"), patternSync->getPort("in"));


                // Set out communication
                link(patternSync->getPort("out"), cOut->getPort("in"));
                link(cOut->getPort("out"), getPort("out"));


                // Set stamps  communication: source on 'in' port
                link(getPort("in"), cStamps->getPort("in"));
                link(cStamps->getPort("out"), patternSync->getPort("stamps"));

                // Set sync communication
                link(getPort("sync"), cSync->getPort("in"));
                link(cSync->getPort("out"), patternSync->getPort("sync"));


                // Connect  patternsync remaining ports  to parent ports
                for (pPortIterator it = patternSync->getPortBegin(); it != patternSync->getPortEnd(); ++it)
				{
					if ((*it)->getId() != "stamps" &&
						(*it)->getId() != "sync" &&
						(*it)->getId() != "in" &&
						(*it)->getId() != "out" )
						link(*it,getPort((*it)->getId()));
				}
            }
    };

}

}

#endif //__FLOWVR_APP_COMSYNC_H_


