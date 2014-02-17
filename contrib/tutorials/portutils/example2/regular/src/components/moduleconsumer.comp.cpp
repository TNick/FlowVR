#include "components/moduleconsumer.comp.h"

using namespace flowvr::app;

namespace example1
{
	ModuleConsumer::ModuleConsumer( const std::string &id_ )
	: Module( id_ )
	{
		addPort( "eatdata", INPUT, FULL, "", Port::ST_BLOCKING );
	}

	Component *ModuleConsumer::create() const
	{
		return new ModuleConsumer( getId() );
	}

} // namespace example1

