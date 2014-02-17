#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>


using namespace flowvr::portutils;
using namespace flowvr;


extern "C" void getParameters( ARGS &args )
{
}



namespace
{
	class Data
	{
	public:
		Data() { memset( data, 0, 256 ); }
		char data[256];
	};

	Data deserialize( const void *in, size_t n )
	{
		Data date;
		memcpy( date.data, in, 256 );
		return date;
	}

	void consume( const Data &date )
	{
		// hummy yummy yummy
	}

	class ConsumePlugHandler : public SinkPortHandler
	{
	public:
		ConsumePlugHandler( const flowvr::portutils::ARGS &args )
		: SinkPortHandler()
		{
		}

		virtual eState handleMessage( const flowvr::Message &m,
                                      const flowvr::StampList *sl )
		{
			const char *pdate = m.data.getRead<char>();

			for( size_t n=0; n < m.data.getSize()/256; ++n )
			{
				Data date = deserialize( pdate, 256 );
				consume( date );
				pdate += 256;
			}
			return E_OK;
		}
	};
}


DEFIMP_PORTPLUG_NOSERVICE( ConsumePlugHandler, ConsumePlug )
