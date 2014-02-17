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
#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/portconfigure.h>

#include <flowvr/xml.h>

#include <set>
#include <string>

#include <dlfcn.h>


using namespace flowvr::portutils;

void getParameter( const std::string &soName, const std::string &sName, ARGS &storage )
{
	std::string so;
	void *soHandle = flowvr::portutils::openDynamicLib(soName,RTLD_LAZY|RTLD_LOCAL,so);
	if(!soHandle)
	{
		std::cerr << dlerror() << std::endl;
		return;
	}

	PortPlugin::pmMthd prm = (PortPlugin::pmMthd)dlsym(soHandle, "getParameters");

	if( prm != NULL )
	{
		ARGS lcArgs;
		prm(lcArgs);

		storage[ sName ] = Parameter( lcArgs );
	}

	dlclose( soHandle );
}


void collectParams( flowvr::xml::TiXmlHandle &root, const std::string &nodename, flowvr::portutils::ARGS &storage )
{
	flowvr::xml::TiXmlHandle pgs = root.FirstChild( nodename );
	flowvr::xml::TiXmlElement *child;

	for(child = pgs.Element(); child; child = child->NextSiblingElement(nodename) )
	{
		const char *pcName        = child->Attribute("name");
		const char *pcSo          = child->Attribute("so");
		if(pcName and pcSo)
		{
			getParameter( pcSo, pcName, storage );
		}
	}

}

int main(int argc, char **argv)
{
	flowvr::utils::Option<std::string> *portFile =
			new flowvr::utils::Option<std::string>("portfile", 'p', "path to the module parameter file", false);
	flowvr::utils::Option<std::string> *argtree =
			new flowvr::utils::Option<std::string>("argtree", 'a', "base64 encoded / bzip2 compressed arguments", false);

	flowvr::utils::CmdLine line(std::string(argv[0]) + std::string(": show parameter settings for portutils-binary") );

	bool bError=false;

	if(line.parse( argc, argv, &bError ) == false and bError == false )
	{
		// help option
		std::cout << line.help() << std::endl;
		return false;
	}

	if((*portFile).count == 0)
	{
		std::cerr << "no portfile specified. STOP." << std::endl;
		return false;
	}

	std::string argT = argtree->value();


	flowvr::xml::TiXmlDocument config( (*portFile).value().c_str() );
	if( config.LoadFile() )
	{
		// configure plugins first
		flowvr::xml::TiXmlHandle docHandle( &config );
		flowvr::xml::TiXmlHandle configNode  = docHandle.FirstChild("config");

		flowvr::xml::TiXmlHandle xmlServices = configNode.FirstChild("services");
		flowvr::xml::TiXmlHandle xmlPlugs    = configNode.FirstChild("plugins");

		flowvr::portutils::ARGS argmap;

		if( !argT.empty() )
		{
			std::string::size_type n = argT.find_first_of(':', 0);
			if( !n or n == std::string::npos )
			{
				std::cerr << argtree->count << ": argtree given, but no size prefix!" << std::endl
						  << "argtree = " << argtree->value() << std::endl;
				argmap["services"] = flowvr::portutils::Parameter( flowvr::portutils::ParameterContainer() );
				argmap["plugins" ] = flowvr::portutils::Parameter( flowvr::portutils::ParameterContainer() );
			}
			else
			{
				std::string sb64( argT.begin()+n+1, argT.end() );
				std::string sSize( argT.begin(), argT.begin()+n);

				flowvr::portutils::ARGS::fromBase64(sb64, argmap, atol(sSize.c_str()) );
			}
		}

		flowvr::portutils::ARGS args;
		args["services"] = Parameter( ParameterContainer() );
		args["plugins"]  = Parameter( ParameterContainer() );

		collectParams(xmlServices, "service", args["services"].getSubContainer() );
		collectParams(xmlPlugs, "plugin", args["plugins"].getSubContainer() );

		args.print();

	}
	else
	{
		std::cerr << "could not load port-config file given as [" << (*portFile).value() << "]" << std::endl;
		const char *pcErr = config.ErrorDesc();
		if(pcErr)
			std::cerr << "row: " << config.ErrorRow() << std::endl
			          << "col: " << config.ErrorCol() << std::endl
			          << "err: " << pcErr << std::endl;
		return false;
	}


	return 0;
}
