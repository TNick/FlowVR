#ifndef METAMODULEVIEWERPARTICLES_COMP_H
#define METAMODULEVIEWERPARTICLES_COMP_H

//Basic components includes
#include <flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h>

//ViewerParticles specific components includes
#include "particles/components/moduleviewerparticles.comp.h"

using namespace flowvr::app;

namespace particles
{
  class MetaModuleViewerParticles : public MetaModuleFlowvrRunSSHSingleton<ModuleViewerParticles>
  {
      public:
          MetaModuleViewerParticles(const std::string& id_) : MetaModuleFlowvrRunSSHSingleton<ModuleViewerParticles>(id_, CmdLine(""))
          {
            addParameter<unsigned int>("version",1);
          }

          virtual void configure()
          {
              FlowvrRunSSH* ssh = getRun();
              ssh->addArg("viewerparticles"+toString<unsigned int>(getParameter<unsigned int>("version")));
          }

          virtual Component* create() const
          {
              return new MetaModuleViewerParticles(getId());
          }

  };

  class PMetaModuleViewerParticles : public MetaModuleFlowvrRunSSHParallel<ModuleViewerParticles>
  {
      public:
	  	  PMetaModuleViewerParticles (const std::string& id_) : MetaModuleFlowvrRunSSHParallel<ModuleViewerParticles>(id_, CmdLine(""))
          {
            addParameter<unsigned int>("version",1);
          }

          virtual void configure()
          {
              FlowvrRunSSH* ssh = getRun();
              ssh->addArg("viewerparticles"+toString<unsigned int>(getParameter<unsigned int>("version")));
          }

          virtual Component* create() const
          {
              return new PMetaModuleViewerParticles (getId());
          }

  };
}

#endif /*METAMODULEVIEWERPARTICLES_COMP_H*/

