#ifndef _METAMODULECONSUMER_H_
#define _METAMODULECONSUMER_H_

#include <flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h>
#include "components/moduleconsumer.comp.h"

namespace example1
{
	class MetaModuleConsumer : public flowvr::app::MetaModuleFlowvrRunSSHSingleton<ModuleConsumer>
	{
	public :
		MetaModuleConsumer(const std::string& id_ );
		virtual Component* create() const;
	};
} // namespace


#endif // _METAMODULECONSUMER_H_
