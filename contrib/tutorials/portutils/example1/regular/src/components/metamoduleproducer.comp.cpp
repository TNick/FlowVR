#include "components/metamoduleproducer.comp.h"

using namespace flowvr::app;

namespace example1
{
	MetaModuleProducer::MetaModuleProducer(const std::string& id_ )
	: MetaModuleFlowvrRunSSHSingleton<ModuleProducer>(id_, CmdLine("producer") )
	{
		setInfo( "Launcher for Module-Producer");
	}

	Component* MetaModuleProducer::create() const
	{
		return new MetaModuleProducer( getId() );
	}
}
