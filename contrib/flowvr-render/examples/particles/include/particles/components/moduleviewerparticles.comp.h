#ifndef MODULEVIEWERPARTICLES_COMP_H
#define MODULEVIEWERPARTICLES_COMP_H

//Basic components includes
#include <flowvr/app/components/module.comp.h>

using namespace flowvr::app;

namespace particles
{

    class ModuleViewerParticles : public Module
    {	
        public:

            ModuleViewerParticles(const std::string& id_) : Module(id_)
        {

            setInfo("Particles viewer");

            addPort("points", INPUT, FULL, "", Port::ST_BLOCKING );
            addPort("colors", INPUT, FULL, "", Port::ST_BLOCKING );

            addPort("scene", OUTPUT);
        }

            virtual Component* create() const
            {
                return new ModuleViewerParticles(getId());
            }
    };

}

#endif /*MODULEVIEWERPARTICLES_COMP_H*/

