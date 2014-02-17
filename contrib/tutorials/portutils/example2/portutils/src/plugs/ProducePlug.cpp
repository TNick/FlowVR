#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <iostream>

using namespace flowvr::portutils;
using namespace flowvr;


extern "C" void getParameters( ARGS &args )
{
	args["num"] = Parameter("1",
			        "Number of data entities to produce per iteration",
			        Parameter::P_NUMBER );
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
		, m_nNum(1)
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
				Data data = produce();
				size_t sz;
				memcpy( pdate, serialize(data,sz), 256 );
				pdate += 256;
			}
			return E_OK;
		}

		int m_nNum;
	};
}


DEFIMP_PORTPLUG_NOSERVICE( ProducePlugHandler, ProducePlug )
