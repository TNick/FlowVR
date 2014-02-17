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
#include <fstream>
#include <algorithm>
#include <functional>

using namespace flowvr::portutils;

std::string &ltrim( std::string &s )
{
	s.erase( s.begin(), std::find_if( s.begin(), s.end(), std::not1(std::ptr_fun<int,int>(std::isspace))));
	return s;
}

std::string &rtrim( std::string &s )
{
	s.erase( std::find_if( s.rbegin(), s.rend(), std::not1( std::ptr_fun<int,int>(std::isspace))).base(), s.end() );
	return s;
}

std::string &trim( std::string &s )
{
	return ltrim( rtrim(s) );
}

void getParameter( const std::string &soName, const std::string &sName, ARGS &storage )
{
	std::string so;
	void *soHandle = flowvr::portutils::openDynamicLib(soName,RTLD_LAZY|RTLD_LOCAL, so);
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

void claimUnresolvedAttributes( flowvr::xml::TiXmlElement *node, flowvr::portutils::ParameterContainer &storage )
{
	if( !node )
		return;

	// iterate and store attributes


	for( flowvr::xml::TiXmlAttribute *attrib = node->FirstAttribute() ; attrib ; attrib = attrib->Next() )
	{
		const char *value = attrib->Value();
		if( value )
		{
			std::string v(value);
			v = trim(v);
			if( v.size()>1 and v[0] == '@' )
			{
				// mark attribute as possible parameter
				std::string vsub(v.begin()+1, v.end());
				storage[vsub] = flowvr::portutils::Parameter( flowvr::portutils::Parameter::P_STRING );
			}
		}
	}

	for( flowvr::xml::TiXmlElement *child = node->FirstChildElement() ;
		  child ;
		  child = child->NextSiblingElement() )
	{
		claimUnresolvedAttributes( child, storage );
	}
}

void outputparameter( const flowvr::portutils::ARGS &treeRoot,
		              const std::string &strInfix, /** <infix>-<infix>-paramname */
		              const std::string &strPrefix, /** <prefix>/<infix>... */
		              std::ostream &out, bool bOutputBounds, bool bShowEmptySubs )
{
	if(bOutputBounds)
		out << "# BEGIN -- " << (strInfix.empty() ? "<module-root>\n" : strInfix+std::string("\n") );

	for( flowvr::portutils::ARGS::const_iterator cit = treeRoot.begin(); cit != treeRoot.end(); ++cit )
	{
		const flowvr::portutils::Parameter &p = (*cit).second;
		if( p.getType() == flowvr::portutils::Parameter::P_PARAMETERCONTAINER )
		{
			if( p.getSubContainerConstRef().empty() )
			{
				if(bShowEmptySubs)
					out << "# " << strPrefix << strInfix << "-" << (*cit).first << " -- has no parameters." << std::endl << std::endl;
				//out << strPrefix << strInfix << "-" << (*cit).first << " = <DEFINE>" << std::endl;
				continue;
			}
			else
			{
				std::string infix = (strInfix.empty() ? (*cit).first : strInfix+std::string("-")+(*cit).first);
				outputparameter( p.getSubContainerConstRef(), infix, strPrefix, out, bOutputBounds, bShowEmptySubs );
			}
		}
		else
		{
			if(bOutputBounds)
				out << std::endl;

			out << "# " << strInfix << " -- " << (*cit).first << ": " << std::endl;

			std::string strHelpText = p.getHelpString();
			if(!strHelpText.empty())
				out <<  "# " << strHelpText << std::endl;

			out << 	"# TYPE: "
				<< p.getTypeString(p.getType());

			if (p.getSubType() != flowvr::portutils::Parameter::PS_NONE)
				out << " ; SUBTYPE: "
				    << p.getSubTypeString( p.getSubType() );

			out	<< " ; MODE = "
				<< p.getModeString(p.getModeType())
				<< std::endl;

			out << strPrefix << strInfix << "-" << (*cit).first << "=" << p.getValueString() << std::endl;
		}
	}
	if(bOutputBounds)
		out <<  "## END " << (strInfix.empty() ? "<module-root>\n" : strInfix+std::string("\n") ) << std::endl;
	else if(!strInfix.empty())
			out << std::endl; // avoid endl for root-node
}

int main(int argc, char **argv)
{
	flowvr::utils::Option<std::string> *portFile =
			new flowvr::utils::Option<std::string>("portfile", 'p', "path to the module parameter file", false);
	flowvr::utils::Option<std::string> *outfile =
			new flowvr::utils::Option<std::string>("outfile", 'o', "file-name for output file (def: stdout)", "", false);
	flowvr::utils::Option<std::string> *compname =
			new flowvr::utils::Option<std::string>("compname", 'c', "component prefix for parameterfile (as is)", "", false);
	flowvr::utils::FlagOption *boundsOut = new flowvr::utils::FlagOption("bounds-show", 'b', "show different plugins in output", false, true);
	flowvr::utils::FlagOption *emptySubs = new flowvr::utils::FlagOption("empty-show", 'e', "show plugins that do not need paramters", false, true);

	flowvr::utils::CmdLine line(std::string(argv[0]) + std::string(": create parameter file from portutils-binary") );

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

	std::string preF = compname->value();
	bool bShowEmptySubs = emptySubs->value();
	bool bShowPlugBounds = boundsOut->value();


	if( preF[ preF.size()-1] != ':' )
		preF += ":";

	flowvr::xml::TiXmlDocument config( (*portFile).value().c_str() );
	if( config.LoadFile() )
	{
		// configure plugins first
		flowvr::xml::TiXmlHandle docHandle( &config );
		flowvr::xml::TiXmlHandle configNode  = docHandle.FirstChild("config");

		flowvr::xml::TiXmlHandle xmlServices = configNode.FirstChild("services");
		flowvr::xml::TiXmlHandle xmlPlugs    = configNode.FirstChild("plugins");

		flowvr::portutils::ARGS argmap;

		flowvr::portutils::ARGS args;
		args["services"] = Parameter( ParameterContainer() );
		args["plugins"]  = Parameter( ParameterContainer() );
		args["module"]   = Parameter( ParameterContainer() );

		collectParams(xmlServices, "service", args["services"].getSubContainer() );
		collectParams(xmlPlugs, "plugin", args["plugins"].getSubContainer() );
		claimUnresolvedAttributes( docHandle.FirstChild().Element(), args["module"].getSubContainer() );

//		args.print();


		if( outfile->value().empty() or outfile->value() == "-" )
		{
			outputparameter( args, "", preF, std::cout, bShowPlugBounds, bShowEmptySubs );
		}
		else
		{
			std::ofstream out( outfile->value().c_str() );
			outputparameter( args, "", preF, out, bShowPlugBounds, bShowEmptySubs );
		}
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
