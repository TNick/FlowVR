#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <iostream>

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

	Data produce()
	{
		return Data();
	}

	char *serialize( Data &date, size_t &n )
	{
		n = 256;
		return &date.data[0];
	}

	class ProducePlugHandler : public SourcePortHandler
	{
	public:
		ProducePlugHandler( const flowvr::portutils::ARGS &args )
		: SourcePortHandler()
		{
		}

		virtual eState handleMessage( flowvr::MessageWrite &out,
                                      flowvr::StampList *stampsout,
                                      Allocator &allocate )
		{
			Data date = produce();
			size_t n;
			char *pdate = serialize( date, n );
			out.data = allocate.alloc(n);
			memcpy( out.data.writeAccess(), pdate, n );

			return E_OK;
		}
	};
}


DEFIMP_PORTPLUG_NOSERVICE( ProducePlugHandler, ProducePlug )
