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
 * File: ./include/flowvr/app/components/metamoduleflowvr-run-ssh-parallel.comp.h*
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP__METAMODULEFLOWVRRUNSSHPARALLEL_H
#define __FLOWVR_APP__METAMODULEFLOWVRRUNSSHPARALLEL_H

#include "flowvr/app/components/metamoduleparallel.comp.h"

/**
 * \file metamoduleflowvr-run-ssh-parallel.comp.h
 * \brief implements a metamodule using the flowvr-run-ssh  launcher. 
 */


namespace flowvr{ namespace app{

    /**
     * \class MetaModuleFlowvrRunSSHParallel
     * \brief Metamodule using the flowvr-run-ssh launcher.
     * This metamodule can be used to launch several modules instances of type Module_ in parallel
     * on a remote or local host using the SSH launcher.
     * Number of module instances defined according to the metamoule hostList
     */

    template <class Module_>
        class MetaModuleFlowvrRunSSHParallel : public MetaModuleParallel<Module_, FlowvrRunSSH>
        {
        private :
            /**
             * \brief private default constructor
             */
        	MetaModuleFlowvrRunSSHParallel() : MetaModuleParallel<Module_, FlowvrRunSSH >("") {};

            CmdLine   cmdline; ///<  store the command line before being inserted in the flowvr-run-ssh script line

        public :
            /**
             * \brief constructor
             * \param id_ the id of the metamodule
             * \param run_ the command line required to launch the module (usually the binary name of the module)
             */
            MetaModuleFlowvrRunSSHParallel(const std::string& id_, const CmdLine&  run_);

            /**
             * \brief copy constructor
             */
            MetaModuleFlowvrRunSSHParallel(const MetaModuleFlowvrRunSSHParallel& m)
                : MetaModuleParallel<Module_,FlowvrRunSSH >(m)
                , cmdline(m.cmdline)
            {}

            /**
             * \brief destructor
             */
            virtual ~MetaModuleFlowvrRunSSHParallel(){}


            /**
             * \brief virtual create:  a new instance of the object initialized with same parameters values as this.
             */
            virtual Component* create() const
            {
                return new MetaModuleFlowvrRunSSHParallel(this->getId(),this->cmdline);
            }

    };


    template <class Module_>
    MetaModuleFlowvrRunSSHParallel<Module_>::MetaModuleFlowvrRunSSHParallel(const std::string& id_,  const CmdLine&  run_)
        : MetaModuleParallel<Module_,FlowvrRunSSH >(id_)
        , cmdline(run_)
    {
    	this->setInfo("Flowvr-run-ssh parallel metamodule. Module instances created from the metamodule host list (usually set from .csv file)");

        // Set run using flowvr-run-ssh
        FlowvrRunSSH ssh(this);
        ssh.addCommand(cmdline); /// the command line given in parameter is used as the argument of flowvr-run-ssh 
        ssh.setVerbose();
        ssh.setParallel();
        this->setRun(ssh);
    };

};
};

#endif //__FLOWVR_APP__METAMODULEFLOWVRRUNSSHPARALLEL_H
