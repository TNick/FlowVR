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
 * File: ./include/flowvr/app/components/metamoduleparallel.comp.h *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP__METAMODULEPARALLEL_H
#define __FLOWVR_APP__METAMODULEPARALLEL_H

#include "flowvr/app/components/metamodule.comp.h"
#include "flowvr/app/components/connection.comp.h"

/**
 * \file metamoduleparallel.comp.h
 * \brief metamodule creating n module intances from the metamodule hostlist 
 */


namespace flowvr{ namespace app{

    /**
     * \class MetaModuleParallel
     * \brief metamodule creating n module intances from the metamodule hostlist 
     */

    template <class Module_, class Run_>
        class MetaModuleParallel : public MetaModuleRun<Run_>
        {
        private :
            /**
             * \brief private default constructor
             */
        	MetaModuleParallel() : MetaModuleRun<Run_>("") {};

        public :
            /**
             * \brief constructor
             * \param id_ the id of the metamodule
             * \param run_ the command line required to launch the module (usually the binary name of the module)
             */
            MetaModuleParallel(const std::string& id_);

            /**
             * \brief copy constructor
             */
            MetaModuleParallel(const MetaModuleParallel& m)
            : MetaModuleRun<Run_>(m)
            {}

            /**
             * \brief destructor
             */
            virtual ~MetaModuleParallel(){}


            /**
             * \brief virtual create:  a new instance of the object initialized with same parameters values as this.
             */
            virtual Component* create() const
            {
                return new MetaModuleParallel(this->getId());
            }

            /**
             * \brief configure enable the configuration such as getting parameters and command line completion.
             *
             * This method is called by execute().
             * @see execute()
             */
            virtual void configure()
            {}

            /**
             * \brief Create  module instances   (moduleid = metamoduleid/i).
             *  Create n instances according  to number of hosts in hosts list
             */
            virtual void execute();


            /** \brief  Map modules to hosts according to metamodule hostList
             */
            virtual void setHosts();

    };


    template <class Module_,class Run_>
        MetaModuleParallel<Module_, Run_>::MetaModuleParallel(const std::string& id_)
        : MetaModuleRun<Run_>(id_)
    {
    	// Should probably be moved to metamoduleparallel
    	this->setInfo("Flowvr-run-ssh parallel metamodule. Module instances created from the metamodule host list (usually set from .csv file)");

        Module_ m(id_); ///< create a temporary  instance of module just to get its  port names.

        // create for the metamodule the same ports than for a  module. 
        for(Component::pPortIterator it = m.getPortBegin(); it != m.getPortEnd(); ++it)
            {
                if(!this->hasPort((*it)->getId()))
                    this->addPort((*it)->getId(), (*it)->getType());
            }
    };

  template <class Module_,class Run_>
      void MetaModuleParallel<Module_,Run_>::execute()
    {
    	// Create module instances according to number of hosts in hostlist
        if(this->hostList.empty())
            {
                throw CustomException("MetaModuleParallel " + this->getFullId() + ": empty host list (at least one expected)", __FUNCTION_NAME__);
            }
        
        unsigned int i = 0;
        for (HostList::const_iterator itHost = this->hostList.begin(); itHost != this->hostList.end(); ++itHost, ++i)
            {
                Module_* mod =	Composite::addObject<Module_>(toString<	unsigned int>(i));
                for (Component::pPortIterator itLink =mod->getPortBegin(); itLink != mod->getPortEnd(); ++itLink)
                    {
                        Port* parentPort = this->getPort((*itLink)->getId());
                        link(*itLink, parentPort);
                    }
            }
        
        configure();
    };
  

    template <class Module_, class Run_>
        void MetaModuleParallel<Module_,Run_>::setHosts()
    {
	if ( this->hostList.empty() )
            throw CustomException("MetaModuleParallel " + this->getFullId() + ": empty host list (at least one host expected)", __FUNCTION_NAME__);
        
        unsigned int i = 0;
        for (HostList::const_iterator itHost = this->hostList.begin(); itHost != this->hostList.end(); ++itHost, ++i)
            {
                Component* mod = this->getComponent(toString<unsigned int>(i));
                if (dynamic_cast< ConnectionBase* >(mod)  == NULL)
                    {
                        mod->hostList.push_back(*itHost);
                        mod->hostList.back().appendtoOrigin(this->getId());
                    }
            }
        
    };



};
};

#endif //__FLOWVR_APP__METAMODULEPARALLEL_H
