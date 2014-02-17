#include "particles/components/particlesource.comp.h"

#include "particles/components/metamodulecreatorparticles.comp.h"
#include "particles/components/metamoduleviewerparticles.comp.h"

using namespace flowvr::app;


namespace particles
{
	ParticleSource::ParticleSource( const std::string &id_ )
	: Composite(id_)
	{
		addPort( "beginIt", INPUT, STAMPS, "", Port::ST_BLOCKING );
		addPort( "endIt", OUTPUT, STAMPS );

		addPort( "scene", OUTPUT, FULL );
	}

	void ParticleSource::execute()
	{
		if( hostList.empty() )
			throw CustomException(
					"absolutely no host to map-to available: "
							+ getFullId() + " ", __FUNCTION_NAME__); // no, no placement

		if( hostList.size() == 1 )
		{
			MetaModuleCreatorParticles *cparts = addObject<MetaModuleCreatorParticles>("create");
			MetaModuleViewerParticles  *vparts = addObject<MetaModuleViewerParticles>("view");

			link( cparts->getPort("points"), vparts->getPort("points") );
			link( cparts->getPort("colors"), vparts->getPort("colors") );

			link( vparts->getPort("scene"), getPort("scene") );

			link( getPort("beginIt"), cparts->getPort("beginIt") );
			link( vparts->getPort("endIt"), getPort("endIt") );
		}
		else
		{
			PMetaModuleCreatorParticles *cparts = addObject<PMetaModuleCreatorParticles>("create");
			PMetaModuleViewerParticles  *vparts = addObject<PMetaModuleViewerParticles>("view");

			link( cparts->getPort("points"), vparts->getPort("points") );
			link( cparts->getPort("colors"), vparts->getPort("colors") );

			link( vparts->getPort("scene"), getPort("scene") );

			link( getPort("beginIt"), cparts->getPort("beginIt") );
			link( vparts->getPort("endIt"), getPort("endIt") );
		}
	}

	Component* ParticleSource::create() const
	{
		return new ParticleSource( getId() );
	}
}
