#ifndef METAMODULECREATORPARTICLES_COMP_H
#define METAMODULECREATORPARTICLES_COMP_H

//Basic components includes
#include <flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h>
#include <flowvr/app/components/metamoduleflowvr-run-ssh-parallel.comp.h>

//CreatorParticles specific components includes
#include "particles/components/modulecreatorparticles.comp.h"

using namespace flowvr::app;

namespace particles
{
  class MetaModuleCreatorParticles : public MetaModuleFlowvrRunSSHSingleton<ModuleCreatorParticles>
  {
      public:
          MetaModuleCreatorParticles(const std::string& id_)
          : MetaModuleFlowvrRunSSHSingleton<ModuleCreatorParticles>(id_, CmdLine("creatorparticles"))
          {
          }

          virtual Component* create() const
          {
              return new MetaModuleCreatorParticles(getId());
          }

  };

  class PMetaModuleCreatorParticles : public MetaModuleFlowvrRunSSHParallel<ModuleCreatorParticles>
  {
  public:
      PMetaModuleCreatorParticles(const std::string& id_)
      : MetaModuleFlowvrRunSSHParallel<ModuleCreatorParticles>(id_, CmdLine("creatorparticles"))
      {
      }

      virtual Component* create() const
      {
          return new PMetaModuleCreatorParticles(getId());
      }

  };
}

#endif /*METAMODULECREATORPARTICLES_COMP_H*/

