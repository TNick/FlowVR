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
 * File: ./include/flowvr/app/components/metamoduleparallelfromport.comp.h*
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP__METAMODULEPARALLELFROMPORT_H
#define __FLOWVR_APP__METAMODULEPARALLELFROMPORT_H

#include "flowvr/app/components/metamodule.comp.h"

/**
 * \file metamoduleflowvr-run-ssh-parallelfromport.comp.h
 * \brief metamodule creates n instances of module  according to primitives connected on port defined by parameter 'instances_from_port'
 */


namespace flowvr{ namespace app{

     /**
     * \class MetaModuleParallelFromPort
     * \brief metamodule creates n instances of module  according to primitives connected on port defined by parameter 'instances_from_port'
     */

    template <class Module_,class Run_>
        class MetaModuleParallelFromPort : public MetaModuleRun<Run_>
        {
        private :
            /**
             * \brief private default constructor
             */
        	MetaModuleParallelFromPort() : MetaModuleRun<Run_>("") {};

        public :
            /**
             * \brief constructor
             * \param id_ the id of the metamodule
             * \param run_ the command line required to launch the module (usually the binary name of the module)
             */
            MetaModuleParallelFromPort(const std::string& id_);

            /**
             * \brief copy constructor
             */
            MetaModuleParallelFromPort(const MetaModuleParallelFromPort& m)
            : MetaModuleRun<Run_>(m)
            {}

            /**
             * \brief destructor
             */
            virtual ~MetaModuleParallelFromPort(){}


            /**
             * \brief virtual create:  a new instance of the object initialized with same parameters values as this.
             */
            virtual Component* create() const
            {
                return new MetaModuleParallelFromPort(this->getId());
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
             * Create n module instances
             * according to number of primitive components connected to target port (given by 'instances_from_port' parameter)
             */
            virtual void execute();


            /** \brief  Map modules to hosts according to hosts of primitives connected to 
             * target port (given by NSTANCES_FROM_PORT parameter). Aslo set the metamodule 
             * hostList (used to generated the metamodule run command)
             */
            virtual void setHosts();

    };


    template <class Module_, class Run_>
        MetaModuleParallelFromPort<Module_,Run_>::MetaModuleParallelFromPort(const std::string& id_)
            : MetaModuleRun<Run_>(id_)
    {
    	this->setInfo("Parallel metamodule setting its instances according to the number of primitives connected to its port defined by parameter 'instances_from_port'");
        this->addParameter("instances_from_port"); // The name of the port where we are looking for primitives to define the number of module instances to create"

        Module_ m(id_); ///< create a temporary  instance of module just to get its  port names.

        // create for the metamodule the same ports than for a  module. 
        for(Component::pPortIterator it = m.getPortBegin(); it != m.getPortEnd(); ++it)
            {
                if(!this->hasPort((*it)->getId()))
                    this->addPort((*it)->getId(), (*it)->getType());
            }
    };


    template <class Module_, class Run_>
        void MetaModuleParallelFromPort<Module_,Run_>::execute()
    {
    	const std::string portName = Component::getParameter<std::string>("instances_from_port");
        Port* pPort = this->getPort(portName);
        unsigned int i = 0;
        
        std::list<const Port*> primlist;
        pPort->getPrimitiveSiblingList(primlist);
        
        if( primlist.empty() )
            {
                throw CustomException("MetaModuleParallelFromPort " + this->getFullId() + ", instances_from_port=" + portName + " but detected number of primitive instances is  0 (No Primitive linked to this port)", __FUNCTION_NAME__);
            }

        // Create NB_INSTANES of module and connect it
        for(std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
            {
                Module_ * mod = Composite::addObject<Module_>(toString<unsigned int>(i));
                for (Component::pPortIterator itLink = mod->getPortBegin(); itLink!= mod->getPortEnd(); ++itLink)
                    {
                        if ((*itLink)->getId() == portName)
                            {
                                link(pPort,*itLink);
                                (*itLink)->addRestrictionToPrimitiveSibling(*it);
                            }
                        else
                            {
                                Port* p = this->getPort((*itLink)->getId());
                                try
                                    {
                                        std::list<const Port*> pprimlist;
                                        p->getPrimitiveSiblingList(pprimlist);
                                        
                                        if (pprimlist.size() != primlist.size())
                                            link(*itLink, p);
                                        else
                                            {
                                                std::list<const Port*>::const_iterator itP=pprimlist.begin();
                                                for(unsigned int ip = 0; ip != i; ++ip)
                                                    {
                                                        ++itP;
                                                    }
                                                link(p,*itLink);
                                                (*itLink)->addRestrictionToPrimitiveSibling(*itP);
                                            }
                                    }
                                catch(CustomException e)
                                    {
                                        link(*itLink, p);
                                    }
                            }
                    }
                ++i;
            }
        configure();
    };


    template <class Module_,class Run_>
        void MetaModuleParallelFromPort<Module_,Run_>::setHosts()
    {

        const std::string portName = Component::getParameter<std::string>("instances_from_port");

        if(  ! this->hostList.empty() )
            Msg::warning("MetaModuleParallelFromPort "+this->getFullId()+" clearing non empty host list when starting setHost (host list will be filled  from hosts of primitive connected to port "+portName, __FUNCTION_NAME__);

        this->hostList.clear();
        
        Port* pPort = this->getPort(portName);
        unsigned int i = 0;

        std::list<const Port*> primlist;
        pPort->getPrimitiveSiblingListSkipConnections(primlist);

        if ( primlist.size() == 0)
            throw NoPrimitiveConnectedException(portName, this->getFullId(), __FUNCTION_NAME__);

        for(std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
            {
                Component* mod = this->getComponent(toString<unsigned int>(i));
                Component* siblingComp = (*it)->getOwner();

                // siblingComp is a primitive (but not a connection)  so one host expected
                if(siblingComp->hostList.size() > 1 )
                    throw CustomException("Primitive "+this->getFullId()+" looking for host of Primitive " + siblingComp->getFullId() + ": "+ toString<int>(this->hostList.size())+ " hosts (one expected)", __FUNCTION_NAME__);
                else if( siblingComp->hostList.empty() )
                    throw CustomException("Primitive "+this->getFullId()+" looking for host of Primitive " + siblingComp->getFullId() + ": no host (one expected)", __FUNCTION_NAME__);

                // Set host for module
                mod->hostList.push_back(siblingComp->hostList.front());
                mod->hostList.back().appendtoOrigin(siblingComp->getFullId());

                // Add host to memtamodule host list (used to retreive the  host list used by the run script)
                this->hostList.push_back(siblingComp->hostList.front());
                this->hostList.back().appendtoOrigin(siblingComp->getFullId());

                ++i;
            }
    };


};
};

#endif //__FLOWVR_APP__METAMODULEPARALLELFROMPORT_H
