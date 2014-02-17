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
 * File: ./include/flowvr/app/components/flowvr-render/metamodulefread.comp.h*
 *                                                                 *
 * Contacts:                                                       *
 *                                                                 *
 ******************************************************************/
#ifndef METAMODULEFREAD_COMP_H_
#define METAMODULEFREAD_COMP_H_

#include "flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h"
#include "modulefread.comp.h"

namespace flowvr {	namespace modules {

    class MetaModuleFRead : public MetaModuleFlowvrRunSSHSingleton<ModuleFRead>
    {
        public:
            MetaModuleFRead(const std::string& id_) :
                MetaModuleFlowvrRunSSHSingleton<ModuleFRead>(id_, CmdLine("fread out=") )
            {
            }

            virtual Component* create() const
            {
                return new MetaModuleFRead(getId());
            }

            virtual void configure(){
                FlowvrRunSSH* ssh = getRun();
                ssh->addArg(getParameter<std::string>("outFile"));

            }

    };

} }

#endif /*METAMODULEFREAD_COMP_H_*/
