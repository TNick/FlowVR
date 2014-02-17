#include <flowvr/module.h>
#include <flowvr/message.h>
#include <flowvr/utils/timing.h>
#include <flowvr/utils/cmdline.h>

#include <string.h>

#include <vector>


using namespace flowvr;
using namespace std;
using namespace flowvr::utils;

class Data
{
public:
	Data() { memset( data, 0, 256); }
	char data[256];
};

Data produce()
{
	return Data();
}

char *serialize( Data &data, size_t &n )
{
	n = 256;
	return &data.data[0];
}


int main( int argc, char **argv )
{
	// Parameter passing from the command-line
	CmdLine line("producer");
	bool error=false;

	int nnum = 1;
	Option<int> num("num", 'n', "Number of Data instances to put in one message", &nnum, true );

	if(!line.parse(argc,argv, &error))
	{
		if( error )
			std::cerr << line.help() << std::endl;
		else
			std::cout << line.help() << std::endl;
		return 1;
	}

	OutputPort p("data");
	vector<Port*> ports;
	ports.push_back(&p);

	ModuleAPI* flowvr = initModule(ports);
	if (flowvr == NULL)
		return 1;

	while (flowvr->wait())
	{
		vector<Data> vProduce;
		for( size_t n=0; n<nnum; ++n)
			vProduce.push_back(produce());

		MessageWrite mw;
		mw.data = flowvr->alloc( nnum * 256 );
		char *dt = mw.data.getWrite<char>();
		for( vector< Data >::iterator cit = vProduce.begin();
			 cit != vProduce.end();
			 ++cit )
		{
			size_t n;
			memcpy(dt, serialize((*cit),n), 256 ); // assume static size for this example
			dt += 256; // advance in vector
		}

		// Send message
		flowvr->put(&p,mw);

		utils::microsleep(100);
	}

	flowvr->close();

	return 0;
}
