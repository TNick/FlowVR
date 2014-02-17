#ifndef MODULECREATORPARTICLES_COMP_H
#define MODULECREATORPARTICLES_COMP_H

//Basic components includes
#include <flowvr/app/components/module.comp.h>

using namespace flowvr::app;

namespace particles
{

    class ModuleCreatorParticles : public Module
    {	
        public:

            ModuleCreatorParticles(const std::string& id_) : Module(id_)
        {

            setInfo("Particles generator");

            addPort("points", OUTPUT);
            addPort("colors", OUTPUT);
        }

            virtual Component* create() const
            {
                return new ModuleCreatorParticles(getId());
            }
    };

}

#endif /*MODULECREATORPARTICLES_COMP_H*/

