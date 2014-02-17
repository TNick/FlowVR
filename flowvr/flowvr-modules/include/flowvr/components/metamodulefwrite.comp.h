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
 * File: ./include/flowvr/app/components/flowvr-render/metamodulefwrite.comp.h*
 *                                                                 *
 * Contacts:                                                       *
 *                                                                 *
 ******************************************************************/
#ifndef METAMODULEFWRITE_COMP_H_
#define METAMODULEFWRITE_COMP_H_

#include "flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h"
#include "modulefwrite.comp.h"

namespace flowvr {	namespace modules {

    class MetaModuleFWrite : public MetaModuleFlowvrRunSSHSingleton<ModuleFWrite>
    {
        public:
            MetaModuleFWrite(const std::string& id_) :
                MetaModuleFlowvrRunSSHSingleton<ModuleFWrite>(id_, CmdLine("fwrite ") )
            {
                addParameter("inFile");
                addParameter("raw", "0");
            }

            virtual Component* create() const
            {
                return new MetaModuleFWrite(getId());
            }

            virtual void configure(){
                FlowvrRunSSH* ssh = getRun();
                if(getParameter<int>("raw") != 0)
                {
                    ssh->addArg(" -raw");
                }
                ssh->addArg("in="+getParameter<std::string>("inFile"));

            }

    };

} }

#endif /*METAMODULEFWRITE_COMP_H_*/
