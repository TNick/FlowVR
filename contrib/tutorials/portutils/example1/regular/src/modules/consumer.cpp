#include <flowvr/module.h>
#include <flowvr/message.h>
#include <flowvr/utils/timing.h>

#include <string.h>


using namespace flowvr;
using namespace std;

class Data
{
public:
	Data() { memset( data, 0, 256); }
	char data[256];
};

void consume( Data &date )
{
	// hummy yummy yummy...
}

Data deserialize( char *data, size_t n )
{
	Data date;
	memcpy( date.data, data, n );

	return date;
}


int main( int argc, char **argv )
{
	InputPort p("eatdata");
	vector<Port*> ports;
	ports.push_back(&p);

	ModuleAPI* flowvr = initModule(ports);
	if (flowvr == NULL)
		return 1;

	while (flowvr->wait())
	{
		Message m;
		flowvr->get(&p, m);

		Data date = deserialize((char*)m.data.readAccess(), m.data.getSize() );
		consume(date);

		utils::microsleep(100);
	}

	flowvr->close();

	return 0;
}
