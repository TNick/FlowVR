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
 * File: ./include/flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h*
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP__METAMODULEFLOWVRRUNSSHSINGLETON_H
#define __FLOWVR_APP__METAMODULEFLOWVRRUNSSHSINGLETON_H

#include "flowvr/app/components/metamodulesingleton.comp.h"

/**
 * \file metamoduleflowvr-run-ssh-singleton.comp.h
 * \brief implements a singleton metamodule using the flowvr-run-ssh  launcher. 
 */


namespace flowvr{ namespace app{

    /**
     * \class MetaModuleFlowvrRunSSHSingleton
     * \brief Metamodule using the flowvr-run-ssh launcher.
     * This metamodule can be used to launch modules of type Module_
     * on a remote or local host using the SSH launcher.
     * It is not dedicated to parallel launching but only for Singleton
     * modules (developed to run one module instance only)
     */

    template <class Module_>
        class MetaModuleFlowvrRunSSHSingleton : public MetaModuleSingleton<Module_, FlowvrRunSSH >
    {
        private :
            /**
             * \brief private default constructor
             */
            MetaModuleFlowvrRunSSHSingleton() : MetaModuleSingleton< Module_, FlowvrRunSSH >("") {};

            CmdLine   cmdline; ///<  store the command line before being inserted in the flowvr-run-ssh script line

        public :
            /**
             * \brief constructor
             * \param id_ the id of the metamodule
             * \param run_ the command line required to launch the module (usually the binary name of the module)
             */
            MetaModuleFlowvrRunSSHSingleton(const std::string& id_, const CmdLine&  run_);

            /**
             * \brief copy constructor
             */
            MetaModuleFlowvrRunSSHSingleton(const MetaModuleFlowvrRunSSHSingleton& m)
                : MetaModuleSingleton<Module_, FlowvrRunSSH >(m)
                , cmdline(m.cmdline)
            {}

            /**
             * \brief destructor
             */
            virtual ~MetaModuleFlowvrRunSSHSingleton()
            {}


            /**
             * \brief virtual create:  a new instance of the object initialized with same parameters values as this.
             */
            virtual Component* create() const
            {
                return new MetaModuleFlowvrRunSSHSingleton(this->getId(),this->cmdline);
            }


            /**
             * \brief Overwrite the default built of launching command as singleton metamodule requires a "singleton" suffix to be added.
             *
             */
            virtual xml::DOMElement* XMLRunBuild() const;

    };


    template <class Module_>
        MetaModuleFlowvrRunSSHSingleton<Module_>::MetaModuleFlowvrRunSSHSingleton(const std::string& id_,  const CmdLine&  run_)
            : MetaModuleSingleton<Module_, FlowvrRunSSH>(id_)
            , cmdline(run_)
       {

        // Set run using flowvr-run-ssh
        FlowvrRunSSH ssh(this);
        ssh.addCommand(cmdline); /// the command line given in parameter is used as the argument of flowvr-run-ssh
        ssh.setVerbose();
        this->setRun(ssh);

        this->setInfo("Singleton Metamodule using flowvr-run-ssh starting a single module instance.");
       }




    template <class Module_>
    	xml::DOMElement* MetaModuleFlowvrRunSSHSingleton<Module_>::XMLRunBuild() const
		{

    	xml::DOMElement* result = MetaModuleSingleton<Module_,FlowvrRunSSH >::XMLRunBuild();

    	result->RemoveAttribute("metamoduleid");
		result->SetAttribute("metamoduleid", this->getFullId()+"/singleton");

		return result;
	}

};
};

#endif //__FLOWVR_APP__METAMODULEFLOWVRRUNSSHSINGLETON_H
