#ifndef _PRODUCER_COMP_H_
#define _PRODUCER_COMP_H_

#include <flowvr/app/components/module.comp.h>

namespace example1
{
	class ModuleProducer : public flowvr::app::Module
	{
	public:
		ModuleProducer( const std::string &id_ );
		virtual Component *create() const;
	};
}


#endif // _PRODUCER_COMP_H_
