#include "components/producerconsumerportutils.comp.h"
#include <flowvr/portutils/components/portmodulerun.comp.h>
#include <flowvr/app/components/filterpresignal.comp.h>
#include <flowvr/app/core/genclass.h>
#include <flowvr/app/core/run.h>

using namespace flowvr::app;
using namespace flowvr::portutils;

namespace example1
{
	GENCLASS( ProducerConsumerPortUtils )

	ProducerConsumerPortUtils::ProducerConsumerPortUtils( const std::string &id_ )
	: Composite(id_)
	{

	}

	void ProducerConsumerPortUtils::execute()
	{
		FlowvrRunSSH ssh(this);

		PortModuleRun *producer = new PortModuleRun("producer", "producer.xml", ssh, this );
		PortModuleRun *consumer = new PortModuleRun("consumer", "consumer.xml", ssh, this );

		link( producer->getPort("data"), consumer->getPort("eatdata") );

		FilterPreSignal *ps = addObject<FilterPreSignal>("pf");
		link( consumer->getPort("endIt"), ps->getPort("in") );
		link( ps->getPort("out"), producer->getPort("beginIt") );
	}

	Component *ProducerConsumerPortUtils::create() const
	{
		return new ProducerConsumerPortUtils( getId() );
	}

}
