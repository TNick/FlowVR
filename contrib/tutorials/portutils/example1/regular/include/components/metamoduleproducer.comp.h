#ifndef _METAMODULEPRODUCER_H_
#define _METAMODULEPRODUCER_H_

#include <flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h>
#include "components/moduleproducer.comp.h"

namespace example1
{
	class MetaModuleProducer : public flowvr::app::MetaModuleFlowvrRunSSHSingleton<ModuleProducer>
	{
	public :
		MetaModuleProducer(const std::string& id_ );
		virtual Component* create() const;
	};
} // namespace


#endif // _METAMODULEPRODUCER_H_
