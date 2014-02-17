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
*    Benjamin Petit,                                              *
*    Jeremie Allard,                                              *
*    Thomas Arcila,                                               *
*    Jean-Denis Lesage.                                           *	
*    Clement Menier,                                              *
*    Bruno Raffin                                                 *
*                                                                 *
*******************************************************************
*                                                                 *
* File: ./include/flowvr/render/components/metamoduleviewer.comp.h*
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef METAMODULEVIEWER_COMP_H
#define METAMODULEVIEWER_COMP_H

//Basic components includes
#include <flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h>

//Viewer specific components includes
#include "flowvr/render/components/moduleviewer.comp.h"

using namespace flowvr::app;

namespace flowvr { namespace render {

  class MetaModuleViewer : public MetaModuleFlowvrRunSSHSingleton<ModuleViewer>
  {
      public:
          MetaModuleViewer(const std::string& id_) : MetaModuleFlowvrRunSSHSingleton<ModuleViewer>(id_, CmdLine(""))
          {
              addParameter<std::string>("ViewerExecName","viewer");
              addParameter<std::string>("ViewerExecParameters"," ");
          }

          virtual void configure()
          {
              FlowvrRunSSH* ssh = getRun();
              ssh->addArg(" ");
              ssh->addArg(getParameter<std::string>("ViewerExecName"));
              if(getParameter<std::string>("ViewerExecParameters")!=" ")
                  {
                      ssh->addArg(" ");
                      ssh->addArg(getParameter<std::string>("ViewerExecParameters"));
                  }
          }

          virtual Component* create() const
          {
              return new MetaModuleViewer(getId());
          }

  };
}}

#endif /*METAMODULEVIEWER_COMP_H*/

