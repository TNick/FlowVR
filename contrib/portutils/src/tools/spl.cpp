/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                        PortUtils library                        *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
*                                                                 *
*   The source code is  covered by different licences,            *
*   mainly  GNU LGPL and GNU GPL. Please refer to the             *
*   copyright.txt file in each subdirectory.                      *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*  Ingo Assenmacher,                                              *
*  Bruno Raffin                                                   *
*                                                                 *
*******************************************************************/

#include <flowvr/utils/cmdline.h>
#include <flowvr/portutils/portparameter.h>

using namespace ftl;
using namespace flowvr::portutils;

int main(int argc, char **argv)
{
	flowvr::utils::Option<std::string> argtree( "argtree", 'a', "base64 encoded / compressed argtree string", false);
	flowvr::utils::Option<int> showraw("showraw", 'r', "show raw string as well", false, false);

	flowvr::utils::CmdLine line( (std::string(argv[0])  + std::string(": show parameter container given by an argstring.") ) );
	bool bError = false;
	if( line.parse( argc, argv, &bError ) == false )
	{
		if( !bError )
			std::cout << line.help() << std::endl;
		else
			std::cerr << "Error parsing command line." << std::endl
			          << line.help() << std::endl;
		return 0;
	}

	if(argtree.count == 0)
	{
		std::cerr << "no argtree string given!" << std::endl;
		return -1;
	}

	ParameterContainer c;
	std::string fullString = argtree.value();
	std::string::size_type n = fullString.find_first_of(':', 0);
	std::string size(fullString.begin(), fullString.begin()+n);
	std::string tree( fullString.begin()+n+1, fullString.end());

	std::string sRaw = ParameterContainer::fromBase64(tree, c, atol( size.c_str() ) );

	std::cout << "###############################" << std::endl
			  << "TREE" << std::endl
			  << "###############################" << std::endl;

	c.print();

	if(showraw.value())
	{
		std::cout << "###############################" << std::endl
				  << "RAW" << std::endl
				  << "###############################" << std::endl;
		std::cout << sRaw << std::endl;
	}
	return 0;
}

