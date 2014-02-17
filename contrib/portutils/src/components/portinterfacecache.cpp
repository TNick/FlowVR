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

#include <flowvr/portutils/components/portinterfacecache.h>
#include <memory>

#include <flowvr/xml.h>
#include <dlfcn.h>              // POSIX dl stuff
#include <exception>

#include <algorithm>
#include <functional>

#include <flowvr/app/core/component.h>
#include <flowvr/utils/filepath.h>

#include <ftl/convert.h>

#include <flowvr/portutils/portconfigure.h>

using namespace ftl;

namespace
{
	flowvr::utils::FilePath portfiles( "FLOWVR_PORTFILE_PATH" );

	std::string resolve( const std::string &name, const flowvr::app::Component &container, int nDepth = 0 )
	{
		std::string value = container.getParameter<std::string>(name);
		if(value.empty())
			return value;

		std::string v(value);

		v = trim(v);

		if( v.empty() )
			return std::string();

		if( v.size() == 1 )
			return v;

		std::string vsub( v.begin()+1, v.end() );

		switch( v[0] )
		{
			case '$':
			{
				// resolve from ENV
				const char *env = getenv( vsub.c_str() );
				if( env )
					return std::string(env);
				else
					return v;
			}
			case '@':
			{
				// resolve from parameter
				if( nDepth == 256 ) // arbitrary depth / break endless cycle
					throw flowvr::app::ParameterException( container.getId(), value, __FUNCTION_NAME__ );

				try
				{
					return resolve( vsub, container, ++nDepth ); // recurse
				}
				catch( flowvr::app::ParameterException &e )
				{
					return v;
				}
				break;
			}
			case '#':
			{
				// resolve from ENV, if that is not given, resolve from parameter
				const char *env = getenv( vsub.c_str() );
				if( env )
					return std::string(env);
				else
				{
					// resolve from parameter
					if( nDepth == 256 ) // arbitrary depth / break endless cycle
						throw flowvr::app::ParameterException( container.getId(), value, __FUNCTION_NAME__ );

					try
					{
						return resolve( vsub, container, ++nDepth ); // recurse
					}
					catch( flowvr::app::ParameterException &e )
					{
						return v;
					}
					break;
				}
				break;
			}
			case '\\':
			{
				if( v[1] == '$' or v[1] == '@' or v[1] == '#' )
					return vsub; // skip first char '\'
				// no break here on purpose
			}
			default:
				return v;
		}

		return std::string();
	}

	std::auto_ptr<flowvr::portutils::PortInterfaceCache> SsSingleton( new flowvr::portutils::PortInterfaceCache );

	typedef void (*paramGMthd)( flowvr::portutils::ParameterContainer & );

	int getParametersFromSo( const std::string &strSoName,
			                 const std::string &strFctName,
			                 const std::string &strModuleName,
			                 flowvr::portutils::ParameterContainer &args )
	{
		/*
		 * we open the dll using RTLD_GLOBAL to assure that any symbol incoming from
		 * this service layer is available to the plugs as well.
		 */
		std::string soName;
		void *soHandle = flowvr::portutils::openDynamicLib(strSoName,RTLD_LAZY|RTLD_LOCAL, soName);
		if(!soHandle)
		{
			std::cerr << dlerror() << std::endl;
			return -1;
		}

		paramGMthd dlParmsG = (paramGMthd)dlsym(soHandle, strFctName.c_str() );
		if( dlParmsG )
		{
			flowvr::portutils::ParameterContainer c;
			dlParmsG( c );
			args[strModuleName] = flowvr::portutils::Parameter( c );
			dlclose(soHandle);

			return c.size();
		}


		dlclose( soHandle );

		return 0;
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

}

namespace flowvr
{
	namespace portutils
	{

		PortInterfaceCache::PortDescriptor::PortDescriptor()
		{

		}


		PortInterfaceCache::PortDescriptor::PortDescriptor( const std::string &strName,
						const std::string &portType,
						const std::string &portMessageType,
						const std::string &strSameAsName,
						const std::string &strExport,
						const std::string &strFlags,
						const std::string &strMultiplicity,
						const std::string &strFileName)
		: m_Name( strName )
		, m_PortType( portType )
		, m_MessageType( portMessageType )
		, m_SameAsName( strSameAsName )
		, m_Export( strExport )
		, m_Flags( strFlags )
		, m_Multiplicity( strMultiplicity )
		, m_strFileName( strFileName )
		{}

		bool PortInterfaceCache::PortDescriptor::isResolved() const
		{
			return true;
		}

		PortInterfaceCache::PortDescriptor PortInterfaceCache::PortDescriptor::resolved( const flowvr::app::Component &context ) const
		{
			return PortDescriptor( resolve( m_Name, context ),
								   resolve( m_PortType, context ),
								   resolve( m_MessageType, context),
								   resolve( m_SameAsName, context),
								   resolve( m_Export, context ),
								   resolve( m_Flags, context ),
								   resolve( m_Multiplicity, context),
								   m_strFileName );
		}

		size_t PortInterfaceCache::PortDescriptor::createPorts( flowvr::app::Component &parent, bool bForceExport ) const
		{
			if(!isResolved())
				return resolved(parent).createPorts( parent );


			bool bExport = convertTo<bool>( m_Export );
			if( !bForceExport and !bExport )
				return 0;


			flowvr::app::PortMessageType mTp = ( !m_SameAsName.empty() ? flowvr::app::SAMEAS : flowvr::app::FULL);
			if( mTp == flowvr::app::FULL && (m_Flags.find("stampsonly") != std::string::npos ) )
				mTp = flowvr::app::STAMPS;

			flowvr::app::PortType Tp;

			if( m_PortType == "in" )
			{
				Tp = flowvr::app::INPUT;
			}
			else // make it an outport
			{
				Tp = flowvr::app::OUTPUT;
			}


			bool bBlocking = true;

			bBlocking = (m_Flags.find( "eventsource" ) == std::string::npos); // it is blocking when the flags is absent

			// multiplicty
			size_t mlt = strtol( m_Multiplicity.c_str(), NULL, 10 );

			for( size_t n = 0 ; n < mlt; ++n )
			{
				std::string name;
				if( mlt > 1 )
				{
					name = m_Name + ftl::toString<size_t>(n);
				}
				else
					name = m_Name;

				parent.addPort( name, Tp, mTp, m_SameAsName,  ( Tp == flowvr::app::OUTPUT ? flowvr::app::Port::ST_UNKNOWN
																   : ( bBlocking ? flowvr::app::Port::ST_BLOCKING : flowvr::app::Port::ST_NONBLOCKING ) ) );
			}
			return mlt;
		}

		 // #######################################################################


		PortInterfaceCache::Interface::Interface()
		: m_valid(false)
		{
			m_mpARGS["services"] = Parameter( ParameterContainer() );
			m_mpARGS["plugins"]  = Parameter( ParameterContainer() );
			m_mpARGS["module"]   = Parameter( ParameterContainer() );
		}

		bool PortInterfaceCache::Interface::getIsValid() const
		{
			return m_valid;
		}

		const ParameterContainer &PortInterfaceCache::Interface::getServiceParameters() const
		{
			ARGS::const_iterator cit = m_mpARGS.find("services");
			if(cit == m_mpARGS.end())
				throw std::exception();

			return (*cit).second.getSubContainerConstRef();
		}

		const ParameterContainer &PortInterfaceCache::Interface::getPluginParameters() const
		{
			ARGS::const_iterator cit = m_mpARGS.find("plugins");
			if(cit == m_mpARGS.end())
				throw std::exception();

			return (*cit).second.getSubContainerConstRef();
		}

		const ParameterContainer &PortInterfaceCache::Interface::getModuleParameters() const
		{
			ARGS::const_iterator cit = m_mpARGS.find("module");
			if(cit == m_mpARGS.end())
				throw std::exception();

			return (*cit).second.getSubContainerConstRef();
		}



		// ########################################################################


		PortInterfaceCache *PortInterfaceCache::getSingleton()
		{
			return SsSingleton.get();
		}

		PortInterfaceCache::PortInterfaceCache()
		{

		}

		PortInterfaceCache::~PortInterfaceCache()
		{

		}


		PortInterfaceCache::Interface PortInterfaceCache::getInterface( const std::string &strName ) const
		{
			INTMAP::const_iterator it = m_mpInterfaces.find(strName);
			if(it == m_mpInterfaces.end())
				return Interface();
			else
				return (*it).second;
		}

		bool  PortInterfaceCache::hasInterface( const std::string &strName ) const
		{
			INTMAP::const_iterator it = m_mpInterfaces.find(strName);
			return !(it == m_mpInterfaces.end());
		}


		PortInterfaceCache::Interface  PortInterfaceCache::loadInterface( const std::string &compId,
																		  const std::string &strXmlDescName,
																		  bool bForceReplace )
		{
			if( hasInterface(strXmlDescName) && !bForceReplace )
				return getInterface(strXmlDescName); // load only once for this key

			std::string strXmlDescFileName(strXmlDescName);
			if( !portfiles.exists(strXmlDescFileName) )
			{
				if( !portfiles.findFile( strXmlDescFileName ) )
					throw flowvr::app::ParameterException(compId,
							 "portfile: requested file was ["+strXmlDescName
							 +"], but this pointing to a non-existent file (using FilePath(FLOWVR_PORTFILE_PATH)).", __FILE__);
			}

			flowvr::xml::TiXmlDocument config( strXmlDescFileName.c_str() );
			if( !config.LoadFile() )
				throw flowvr::app::ParameterException(compId,
						 "portfile: requested file was ["+strXmlDescFileName
						 +"], but this pointing to a file without proper XML syntax.", __FILE__);

			// configure plugins first
			flowvr::xml::TiXmlHandle docHandle( &config );
			flowvr::xml::TiXmlHandle configNode = docHandle.FirstChild("config");
			flowvr::xml::TiXmlHandle infoNode = configNode.FirstChild("info");
			flowvr::xml::TiXmlHandle services = configNode.FirstChild("services");
			flowvr::xml::TiXmlHandle plugins = configNode.FirstChild("plugins");


			flowvr::xml::TiXmlHandle portsNode = configNode.FirstChild("ports");
			flowvr::xml::TiXmlHandle pgs = portsNode.FirstChild( "port" );

			Interface interface;

			if( infoNode.Element() )
			{
				// get the text node attached to it
				flowvr::xml::TiXmlText *text = infoNode.FirstChild().Text();
				if(text && text->Value() )
					interface.m_Info = std::string("[")+strXmlDescFileName+std::string("]: ") + text->Value();
			}
			else
			{
				interface.m_Info = std::string("[")+strXmlDescFileName+std::string("]: -- no info given --");
			}


			bool bAllExport = false;
			if( portsNode.Element() )
			{
				const char *pcAllExport = portsNode.Element()->Attribute("exportdefault");
				if(pcAllExport)
					bAllExport = convertTo<bool>( pcAllExport );
			}

			for( flowvr::xml::TiXmlElement *child = pgs.Element();
				 child;
				 child = child->NextSiblingElement("port") )
			{
				const char *pcName         = child->Attribute("name");
				const char *pcDirection    = child->Attribute("direction");
				const char *pcSameAs       = child->Attribute("sameas");
				const char *pcExport       = child->Attribute("export");
				const char *pcFlags        = child->Attribute("flags");
				const char *pcMultiplicity = child->Attribute("multiplicity");

				if(!pcMultiplicity)
					pcMultiplicity = "1";

				if(pcName && pcDirection)
				{
					if( interface.m_mpDescriptors.find( pcName ) != interface.m_mpDescriptors.end() )
					{
						std::cerr << "error: port with name ["
								  << pcName
								  << "] already given. -- SKIP CONFIGURATION"
								  << std::endl;
						continue;
					}

					interface.m_mpDescriptors[pcName] = PortDescriptor( pcName, pcDirection,
																	   (pcSameAs ? pcSameAs : ""),
																	   (pcSameAs ? pcSameAs : ""),
																	   (pcExport ? pcExport : ftl::toString<bool>( bAllExport ) ),
																	   (pcFlags ? pcFlags : "" ),
																	   pcMultiplicity, strXmlDescFileName );
				}
				else
				{
					std::cerr << "either name or direction not given for port declaration" << std::endl;
				}
			} // loop



			// skim all parameters from all services.

			// loop over services
			ParameterContainer &serviceArgs = interface.m_mpARGS["services"].getSubContainer();
			ParameterContainer &pluginArgs  = interface.m_mpARGS["plugins"].getSubContainer();
			ParameterContainer &moduleArgs  = interface.m_mpARGS["module"].getSubContainer();


			flowvr::xml::TiXmlHandle service = services.FirstChild("service");
			for(flowvr::xml::TiXmlElement *o = service.Element(); o; o = o->NextSiblingElement("service") )
			{

				const char *pcPrefix = o->Attribute("prefix");
				const char *pcSo     = o->Attribute("so");
				const char *pcName   = o->Attribute("name");

				if(pcSo and pcName)
					getParametersFromSo( std::string(pcSo), "getParameters", std::string(pcName), serviceArgs );

			} // loop services

			flowvr::xml::TiXmlHandle plugin = plugins.FirstChild("plugin");
			for(flowvr::xml::TiXmlElement *o = plugin.Element(); o; o = o->NextSiblingElement("plugin") )
			{
				const char *pcSo   = o->Attribute("so");
				const char *pcName = o->Attribute("name");

				if(pcSo and pcName)
					getParametersFromSo( std::string(pcSo), "getParameters", std::string(pcName), pluginArgs );
			}


			// claim all unresolved parameter variables from all attributes
			claimUnresolvedAttributes( docHandle.FirstChild().Element(), moduleArgs );

			interface.m_valid = true; // validate entry

			m_mpInterfaces[strXmlDescName] = interface; // copy in
			return interface;
		}
	} // namespace portutils
} // namespace flowvr


/* make function easily accessible from outside with POD's */

extern "C" {
          
int port_interface_get_params_from_so(const char *strSoName, const char *strFctName, const char *strModuleName, 
                                      char *buf, int sz) {
  flowvr::portutils::ParameterContainer pc;

  getParametersFromSo(strSoName, strFctName, strModuleName, pc);
  std::string str = pc.stringify();
  strncpy(buf, str.c_str(), sz);

  return strlen(buf);
}

};

