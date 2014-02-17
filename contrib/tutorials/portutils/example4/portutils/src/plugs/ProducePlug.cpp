#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <iostream>
#include "services/DataService.h"

using namespace flowvr::portutils;
using namespace flowvr;
using namespace example3;

extern "C" void getParameters( ARGS &args )
{
	args["num"] = Parameter("1",
			        "Number of data entities to produce per iteration",
			        Parameter::P_NUMBER );
}



namespace
{
	class ProducePlugHandler : public SourcePortHandler
	{
	public:
		ProducePlugHandler( DataService *service, const flowvr::portutils::ARGS &args )
		: SourcePortHandler()
		, m_nNum(1)
		, m_service(service)
		{
			try
			{
				m_nNum = args("num").getValue<int>();
			}
			catch( std::exception & )
			{}
		}

		virtual eState handleMessage( flowvr::MessageWrite &out,
                                      flowvr::StampList *stampsout,
                                      Allocator &allocate )
		{

			out.data = allocate.alloc( m_nNum * 256 );
			char *pdate = out.data.getWrite<char>();

			for( size_t n=0; n<m_nNum;++n)
			{
				DataService::Data data = (*m_service).produce();
				size_t sz;
				memcpy( pdate, (*m_service).serialize(data,sz), 256 );
				pdate += 256;
			}
			return E_OK;
		}

		int m_nNum;
		DataService *m_service;
	};
}


DEFIMP_PORTPLUG( ProducePlugHandler, example3::DataService, ProducePlug )
