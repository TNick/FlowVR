#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>
#include "services/DataService.h"

using namespace flowvr::portutils;
using namespace flowvr;
using namespace example3;


extern "C" void getParameters( ARGS &args )
{
}



namespace
{
	class ConsumePlugHandler : public SinkPortHandler
	{
	public:
		ConsumePlugHandler( DataService *service, const flowvr::portutils::ARGS &args )
		: SinkPortHandler()
		, m_service(service)
		{
		}

		virtual eState handleMessage( const flowvr::Message &m,
                                      const flowvr::StampList *sl )
		{
			const char *pdate = m.data.getRead<char>();

			for( size_t n=0; n < m.data.getSize()/256; ++n )
			{
				DataService::Data date = (*m_service).deserialize( pdate, 256 );
				(*m_service).consume( date );
				pdate += 256;
			}
			return E_OK;
		}

		DataService *m_service;
	};
}


DEFIMP_PORTPLUG( ConsumePlugHandler, DataService, ConsumePlug )
