#ifndef _CONSUMER_COMP_H_
#define _CONSUMER_COMP_H_

#include <flowvr/app/components/module.comp.h>

namespace example1
{
	class ModuleConsumer : public flowvr::app::Module
	{
	public:
		ModuleConsumer( const std::string &id_ );
		virtual Component *create() const;
	};
}


#endif // _CONSUMER_COMP_H_
