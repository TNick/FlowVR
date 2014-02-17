#if !defined(_PRODUCERCONSUMER_COMP_H_)
#define _PRODUCERCONSUMER_COMP_H_

#include <flowvr/app/core/component.h>

namespace example2
{
	class ProducerConsumerPortUtils : public flowvr::app::Composite
	{
	public:
		ProducerConsumerPortUtils( const std::string &id_ );

		virtual void execute();
		virtual Component *create() const;
	};
}


#endif // _PRODUCERCONSUMER_COMP_H_

