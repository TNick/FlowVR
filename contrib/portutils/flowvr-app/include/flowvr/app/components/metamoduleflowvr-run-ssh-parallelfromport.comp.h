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
 * File: ./include/flowvr/app/components/metamoduleflowvr-run-ssh-parallelfromport.comp.h*
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP__METAMODULEFLOWVRRUNSSHPARALLELFROMPORT_H
#define __FLOWVR_APP__METAMODULEFLOWVRRUNSSHPARALLELFROMPORT_H

#include "flowvr/app/components/metamoduleparallelfromport.comp.h"


/**
 * \file metamoduleflowvr-run-ssh-parallelfromPort.comp.h
 * \brief implements a metamodule using the flowvr-run-ssh  launcher. 
 */


namespace flowvr{ namespace app{

     /**
     * \class MetaModuleFlowvrRunSSHParallelFromPort
     * \brief Metamodule using the flowvr-run-ssh launcher.
     * This metamodule can be used to launch several modules instances of type Module_ in parallel
     * on a remote or local host using the SSH launcher.
     * Inherit your metamodule from this class if you want your module to be launched in parallel.
     */

    template <class Module_>
        class MetaModuleFlowvrRunSSHParallelFromPort : public MetaModuleParallelFromPort< Module_,  FlowvrRunSSH >
        {
        private :
            /**
             * \brief private default constructor
             */
                MetaModuleFlowvrRunSSHParallelFromPort() : MetaModuleParallelFromPort< Module_, FlowvrRunSSH >("") {};

            CmdLine   cmdline; ///<  store the command line before being inserted in the flowvr-run-ssh script line

        public :
            /**
             * \brief constructor
             * \param id_ the id of the metamodule
             * \param run_ the command line required to launch the module (usually the binary name of the module)
             */
            MetaModuleFlowvrRunSSHParallelFromPort(const std::string& id_, const CmdLine&  run_);

            /**
             * \brief copy constructor
             */
            MetaModuleFlowvrRunSSHParallelFromPort(const MetaModuleFlowvrRunSSHParallelFromPort& m)
                : MetaModuleParallelFromPort< Module_,FlowvrRunSSH >(m)
                , cmdline(m.cmdline)
            {}

            /**
             * \brief destructor
             */
            virtual ~MetaModuleFlowvrRunSSHParallelFromPort(){}


            /**
             * \brief virtual create:  a new instance of the object initialized with same parameters values as this.
             */
            virtual Component* create() const
            {
                return new MetaModuleFlowvrRunSSHParallelFromPort(this->getId(),this->cmdline);
            }

            /**
             * \brief configure enable the configuration such as getting parameters and command line completion.
             *
             * This method is called by execute().
             * @see execute()
             */
            virtual void configure()
            {}

    };



    template <class Module_>
    MetaModuleFlowvrRunSSHParallelFromPort<Module_>::MetaModuleFlowvrRunSSHParallelFromPort(const std::string& id_,  const CmdLine&  run_)
        : MetaModuleParallelFromPort<Module_,FlowvrRunSSH>(id_)
        , cmdline(run_)
    {
    	// Should probably be moved to metamoduleparallel
    	this->setInfo("FLowvr-run-ssh parallel metamodule creating its module instances according to the primitives connected on port defined by parameter instances_from_port");

        // Set run using flowvr-run-ssh
        FlowvrRunSSH ssh(this);
        ssh.addCommand(cmdline); /// the command line given in parameter is used as the argument of flowvr-run-ssh 
        ssh.setVerbose();
        ssh.setParallel();
        this->setRun(ssh);
    };


};
};

#endif //__FLOWVR_APP__METAMODULEFLOWVRRUNSSHPARALLELFROMPORT_H
