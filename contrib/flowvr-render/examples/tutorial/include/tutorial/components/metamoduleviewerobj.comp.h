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
 *  Contact : Jean-Denis.Lesage@imag.fr                            *
 *                                                                 *
 ******************************************************************/

// basic components includes
#include <flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h>
#include <flowvr/render/components/moduleviewer.comp.h>

// primes specific components includes

#ifndef _METAMODULEVIEWEROBJ_H_
#define _METAMODULEVIEWEROBJ_H_

using namespace flowvr::app;
using namespace flowvr::render;

namespace tutorial {

    // The goal of this component is to encapsulate viewer component and add a command line to launch it
    // ModuleViewer is a generic class with the common interface of viewer components. Use it if your viewer has only a scene and a matrix ports.
    class MetaModuleViewerObj : public MetaModuleFlowvrRunSSHSingleton<ModuleViewer>
    {
        public :
            MetaModuleViewerObj(const std::string& id_ ) : MetaModuleFlowvrRunSSHSingleton<ModuleViewer>(id_, CmdLine("viewerobj") )
        {
            setInfo("Metamodule launching viewer obj  module (render a obj file). The parameter OBJFILE is the path to the .obj file");
            addParameter("OBJFILE", "$FLOWVR_RENDER_PREFIX/share/flowvr-render/data/models/shuttledist16.fmesh");
        };

            virtual void configure()
            {
                FlowvrRunSSH* ssh = this->getRun();
                ssh->addArg(getParameter<std::string>("OBJFILE"));
            }

            //Mandatory virtual destructor
            virtual ~MetaModuleViewerObj(){};

            // Mandatory create  method
            virtual Component* create() const { return new MetaModuleViewerObj(getId());};
    };

};

#endif //_METAMODULEVIEWEROBJ_H_
