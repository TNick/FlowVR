#include "components/producerconsumerportutils.comp.h"
#include <flowvr/portutils/components/portmodulerun.comp.h>
#include <flowvr/app/components/filterpresignal.comp.h>
#include <flowvr/app/core/genclass.h>
#include <flowvr/app/core/run.h>

using namespace flowvr::app;
using namespace flowvr::portutils;

namespace example4
{
	GENCLASS( ProducerConsumerPortUtils )

	ProducerConsumerPortUtils::ProducerConsumerPortUtils( const std::string &id_ )
	: Composite(id_)
	{
		addParameter("chain-length", 1);
	}

	void ProducerConsumerPortUtils::execute()
	{
		FlowvrRunSSH ssh(this);

		PortModuleRun *producer = new PortModuleRun("producer", "producer.xml", ssh, this );
		PortModuleRun *consumer = new PortModuleRun("consumer", "consumer.xml", ssh, this );

		int chain_length = getParameter<int>("chain-length");
		if( chain_length < 0 )
			throw CustomException("Chain-length must be > 0", __FUNCTION_NAME__);

		PortModuleRun *pred = producer;
		for( size_t n = 0; n < chain_length; ++n )
		{
			PortModuleRun *c = new PortModuleRun("filter"+toString<int>(n), "consumerproducer.xml", ssh, this);
			link( pred->getPort("data"), c->getPort("eatdata") );
			pred = c;
		}

		link( pred->getPort("data"), consumer->getPort("eatdata") );

		FilterPreSignal *ps = addObject<FilterPreSignal>("pf");
		link( consumer->getPort("endIt"), ps->getPort("in") );
		link( ps->getPort("out"), producer->getPort("beginIt") );
	}

	Component *ProducerConsumerPortUtils::create() const
	{
		return new ProducerConsumerPortUtils( getId() );
	}

}
