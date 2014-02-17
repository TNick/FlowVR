#include "particles/components/particles.comp.h"
#include <flowvr/app/core/genclass.h>

#include <flowvr/app/components/syncmaxfrequency.comp.h>
#include <flowvr/portutils/components/portmodulerun.comp.h>
#include <flowvr/render/balzac/components/balzaclostinspaceviewer.comp.h>

#include "particles/components/metamodulecreatorparticles.comp.h"
#include "particles/components/metamoduleviewerparticles.comp.h"

#include "particles/components/particlesource.comp.h"

using namespace flowvr::portutils;
using namespace flowvr::render::balzac;

namespace particles
{
	Particles::Particles( const std::string &id_ )
	: Composite(id_)
	{
		addParameter( "balzac-portfile", "balzac-autobounds.xml" );
		addParameter( "navigate-portfile", "particleviewertest-navigate.xml" );
	}

	void Particles::execute()
	{
		BalzacLostInSpaceViewer *balzac = new BalzacLostInSpaceViewer( "renderer", this );

		ParticleSource *source = addObject<ParticleSource>("particles");

//		// PARTICLES
//		MetaModuleCreatorParticles *cparts = addObject<MetaModuleCreatorParticles>("create");
//		MetaModuleViewerParticles  *vparts = addObject<MetaModuleViewerParticles>("view");
//
//		link( cparts->getPort("points"), vparts->getPort("points") );
//		link( cparts->getPort("colors"), vparts->getPort("colors") );
		link( source->getPort("scene"), balzac->getPort("scene") );

		// limit frequency
		SyncMaxFrequency *ff = addObject<SyncMaxFrequency>( "ff" );
		link( ff->getPort("out"), source->getPort("beginIt") );
		link( source->getPort("endIt"), ff->getPort("endIt") );
	}

	GENCLASS(Particles)
}
