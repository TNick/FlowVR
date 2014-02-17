#include "components/producerconsumerregular.comp.h"
#include "components/metamoduleproducer.comp.h"
#include "components/metamoduleconsumer.comp.h"

#include <flowvr/app/components/filterpresignal.comp.h>
#include <flowvr/app/core/genclass.h>

using namespace flowvr::app;

namespace example1
{
	ProducerConsumerRegular::ProducerConsumerRegular( const std::string &id_)
	: Composite(id_)
	{

	}

	void ProducerConsumerRegular::execute()
	{
		MetaModuleProducer *p = addObject<MetaModuleProducer>("producer");
		MetaModuleConsumer *c = addObject<MetaModuleConsumer>("consumer");

		link( p->getPort("data"), c->getPort("eatdata") );


		// technical detail here: break cycle using a PreSignal (nb=1)
		FilterPreSignal *ps = addObject<FilterPreSignal>("ps");
		link( c->getPort("endIt"), ps->getPort("in") );
		link( ps->getPort("out"), p->getPort("beginIt") );
	}

	Component *ProducerConsumerRegular::create() const
	{
		return new ProducerConsumerRegular( getId() );
	}

	GENCLASS( ProducerConsumerRegular )
}
