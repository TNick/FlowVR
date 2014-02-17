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
 * File: ./include/flowvr/app/components/metamodulesingleton.comp.h*
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP__METAMODULESINGLETON_H
#define __FLOWVR_APP__METAMODULESINGLETON_H

#include "flowvr/app/components/metamodule.comp.h"

/**
 * \file metamodulesingleton.comp.h
 * \brief implements a metamodule creating a single module instance.
 */


namespace flowvr{ namespace app{

    /**
     * \class MetaModuleSingleton
     * \brief metamodule creating a single module instance.
     */

    template <class Module_, class Run_>
        class MetaModuleSingleton : public MetaModuleRun<Run_>
    {
        private :
            /**
             * \brief private default constructor
             */
            MetaModuleSingleton() : MetaModuleRun<Run_>("") {};

        public :
            /**
             * \brief constructor
             * \param id_ the id of the metamodule
             * \param run_ the command line required to launch the module (usually the binary name of the module)
             */
            MetaModuleSingleton(const std::string& id_);

            /**
             * \brief copy constructor
             */
            MetaModuleSingleton(const MetaModuleSingleton& m): MetaModuleRun<Run_>(m){};

            /**
             * \brief destructor
             */
            virtual ~MetaModuleSingleton(){};


            /**
             * \brief virtual create:  a new instance of the object initialized with same parameters values as this.
             */
            virtual Component* create() const
            {
                return new MetaModuleSingleton(this->getId());
            };

            /**
             * \brief configure enable the configuration such as getting parameters and command line completion.
             *
             * This method is called by execute().
             * @see execute()
             */
            virtual void configure(){};

            /**
             * \brief execute create one  module (moduleid = metamoduleid/singleton). The module ports are reproduced on the metamodule.
             * 	So the metamodule get exaclty the same ports than the contained module. The metamodule and module ports of the same name are linked together.
             */
            virtual void execute();
     
            /**
             * \brief execute create one  module (moduleid = metamoduleid/singleton). The module ports are reproduced on the metamodule.
             * 	So the metamodule get exactly the same ports than the contained module. The metamodule and module ports of the same name are linked together.
             */
            virtual void setHosts();

    };



    template <class Module_,class Run_>
        MetaModuleSingleton<Module_,Run_>::MetaModuleSingleton(const std::string& id_)
        : MetaModuleRun<Run_>(id_)
       {

        this->setInfo("Singleton Metamodule creating a single module instance.");
        Module_ m(id_); ///< create a temporary  instance of module just to get its  port names.

        // create for the metamodule the same ports than for a  module.
        for(Component::pPortIterator it = m.getPortBegin(); it != m.getPortEnd(); ++it)
            {
                if(!this->hasPort((*it)->getId()))
                    this->addPort((*it)->getId(), (*it)->getType());
            }
       }


    
    
    template <class Module_,class Run_>
        void MetaModuleSingleton<Module_,Run_>::execute()
    {
        // Metamodule host list set in constructor (from CSV file)
        if ( this->hostList.size() > 1 )
            {
                throw CustomException("MetaModuleSingleton " +  this->getFullId() + ": singleton metamodule but "+ toString<int>(this->hostList.size())+ " hosts given for mapping (one expected)", __FUNCTION_NAME__);
            }
        else if( this->hostList.empty() )
            {
                throw CustomException("MetaModuleSingleton " + this->getFullId() + ": singleton metamodule but no host given for mapping (one expected)", __FUNCTION_NAME__);
            }
        
        // Add Module
        Module_* mod = Composite::addObject<Module_>("singleton");
        
        // Link module ports to parent ports.
        for (Component::pPortIterator itLink = mod->getPortBegin(); itLink != mod->getPortEnd(); ++itLink)
            {
                Port* parentPort = this->getPort((*itLink)->getId());
                link(*itLink, parentPort);
            }
        configure();
    }
                


    template <class Module_,class Run_>
        void MetaModuleSingleton<Module_,Run_>::setHosts()
        { 
            // Metamodule host list set in constructor (from CSV file)
            if(this->hostList.size() > 1 )
                throw CustomException("MetaModuleSingleton " + this->getFullId() + ": singleton metamodule but "+ toString<int>(this->hostList.size())+ " hosts given for mapping (one expected)", __FUNCTION_NAME__);
            else if( this->hostList.empty() )
		{
                    throw CustomException("MetaModuleSingleton " + this->getFullId() + ": singleton metamodule but no host given for mapping (one expected)", __FUNCTION_NAME__);
		}
            
            // Copy host to child  module (if list not empty before, content is  deleted)
            (* this->children.begin())->hostList.clear();
            (* this->children.begin())->hostList.push_back(this->hostList.front());
            (* this->children.begin())->hostList.front().appendtoOrigin(this->getId());
        }
    
    
    

};
};

#endif //__FLOWVR_APP__METAMODULESINGLETON_H
