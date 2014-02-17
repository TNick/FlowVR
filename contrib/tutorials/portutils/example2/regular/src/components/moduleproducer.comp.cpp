#include "components/moduleproducer.comp.h"

using namespace flowvr::app;

namespace example1
{
	ModuleProducer::ModuleProducer( const std::string &id_ )
	: Module( id_ )
	{
		addPort( "data", OUTPUT, FULL );
	}

	Component *ModuleProducer::create() const
	{
		return new ModuleProducer( getId() );
	}

} // namespace example1

