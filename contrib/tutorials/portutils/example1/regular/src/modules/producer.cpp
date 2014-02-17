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
	OutputPort p("data");
	vector<Port*> ports;
	ports.push_back(&p);

	ModuleAPI* flowvr = initModule(ports);
	if (flowvr == NULL)
		return 1;

	while (flowvr->wait())
	{
		Data date = produce();

		size_t n;
		char *pdate = serialize(date,n);

		MessageWrite mw;

		mw.data = flowvr->alloc(n);
		memcpy(mw.data.writeAccess(),pdate,n);

		// Send message
		flowvr->put(&p,mw);

		utils::microsleep(100);
	}

	flowvr->close();

	return 0;
}
