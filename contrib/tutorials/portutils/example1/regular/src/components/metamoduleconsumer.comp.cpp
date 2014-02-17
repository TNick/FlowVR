#include "components/metamoduleconsumer.comp.h"

using namespace flowvr::app;

namespace example1
{
	MetaModuleConsumer::MetaModuleConsumer(const std::string& id_ )
	: MetaModuleFlowvrRunSSHSingleton<ModuleConsumer>(id_, CmdLine("consumer") )
	{
		setInfo( "Launcher for Module-Consumer");
	}

	Component* MetaModuleConsumer::create() const
	{
		return new MetaModuleConsumer( getId() );
	}
}
