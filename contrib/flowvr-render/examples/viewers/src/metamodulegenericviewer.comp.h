/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                                                                 *
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
*  Contact :                                                      *
*                                                                 *
******************************************************************/

#ifndef METAMODULEGENERICVIEWER_COMP_H_
#define METAMODULEGENERICVIEWER_COMP_H_
// basic components includes
#include <flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h>
#include "modulegenericviewer.comp.h"

namespace flowvr
{
	namespace render
	{
		class MetaModuleGenericViewer : public flowvr::app::MetaModuleFlowvrRunSSHSingleton<ModuleGenericViewer>
		  {
			  public:
				MetaModuleGenericViewer(const std::string& id_)
				  : flowvr::app::MetaModuleFlowvrRunSSHSingleton<ModuleGenericViewer>(id_, flowvr::app::CmdLine(""))
				  {
					addParameter("cmdline");
				  }

				  virtual void configure()
				  {
					  flowvr::app::FlowvrRunSSH* ssh = getRun();
					  ssh->addArg(getParameter<std::string>("cmdline"));
				  }

				  virtual Component* create() const
				  {
					  return new MetaModuleGenericViewer(getId());
				  }

		  };
	}
}


#endif // METAMODULEGENERICVIEWER_COMP_H_
