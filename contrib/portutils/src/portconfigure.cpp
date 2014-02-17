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

#include <flowvr/portutils/portconfigure.h>
#include <flowvr/portutils/porthandlerregistry.h>
#include <flowvr/portutils/porthandler.h>


#include <dlfcn.h>              // POSIX dl stuff
#include <flowvr/xml.h>
#include <flowvr/moduleapi.h>

#include <list>
#include <set>
#include <string>
#include <ctype.h>
#include <algorithm>

#include <ftl/convert.h>


using namespace ftl;

namespace
{
	std::string resolve( const std::string &name, const flowvr::xml::TiXmlElement *node, const flowvr::portutils::ARGS &container, int nDepth = 0 )
	{
		std::string value;
		if( node )
		{
			const char *val = node->Attribute(name.c_str());
			if( !val )
				return std::string();

			value = std::string( val );
		}
		else
		{
			std::cout << "looking up [" << name << "]" << std::endl;
			flowvr::portutils::Parameter p = container(name);
			if( p.getType() == flowvr::portutils::Parameter::P_NONE )
				return value;
			value = p.getValueString();
			std::cout << "FOUND [" << value << "]" << std::endl;
		}

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
				break;
			}
			case '@':
			{
				// resolve from parameter
//				std::cout << "resolve from parameter! [" << name << "/" << vsub << "]" << std::endl;
				if( nDepth == 256 ) // arbitrary depth / break endless cycle
					throw flowvr::portutils::ParameterException( name, "possible infinite recursion" );

				return resolve( vsub, NULL, container, ++nDepth ); // recurse
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
						throw flowvr::portutils::ParameterException( name, "possible infinite recursion" );

					return resolve( vsub, NULL, container, ++nDepth ); // recurse
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
	/*
	 * creation function for base types based on tags. Note that we do only allow single dimension arrays
	 * to be constructed here (array[int], but not array[array[int]] for example.
	 */
	flowvr::BaseType *createBaseType( const std::string &sType, const std::string &subTypeName, int nSize )
	{
		if(sType == "INT")
		{
			return new flowvr::TypeInt;
		}
		else if(sType == "FLOAT")
		{
			return new flowvr::TypeFloat;
		}
		else if(sType == "STRING")
		{
			return new flowvr::TypeString;
		}
		else if(sType == "BINARY")
		{
			return flowvr::TypeBinary::create(nSize);
		}
		else if(sType == "ARRAY")
		{
			// first approach: only allow array of subtypes (not array of arrays)
			flowvr::BaseType *tp = createBaseType( subTypeName, "", nSize);
			return flowvr::TypeArray::create(nSize, tp);
		}

		return NULL;
	}

	/*
	 * read stamps from xml description.
	 */
	flowvr::StampList *configureStampList( const flowvr::xml::TiXmlHandle &stampsConf )
	{
		flowvr::StampList *list = new flowvr::StampList;
		// skim all children of this list to determine stamps, name and type
		flowvr::xml::TiXmlHandle stmp = stampsConf.FirstChild( "stamp" );
		for( flowvr::xml::TiXmlElement *stamp = stmp.Element();
					 stamp;
					 stamp = stamp->NextSiblingElement("stamp") )
		{
			const char *pcStampName = stamp->Attribute("name");
			const char *pcStampType = stamp->Attribute("type");
			const char *pcStampSize = stamp->Attribute("size");
			const char *pcSubType   = stamp->Attribute("subtype");

			if( pcStampName && pcStampType )
			{
				std::string sType(pcStampType);
				std::transform(sType.begin(), sType.end(), sType.begin(), ::toupper);

				std::string sSubType( (pcSubType ? pcSubType : "") );
				std::transform(sSubType.begin(), sSubType.end(), sSubType.begin(), ::toupper );


				flowvr::BaseType *tp = createBaseType( sType, sSubType, (pcStampSize?atoi(pcStampSize):0) );
				if(tp)
				{
					list->add( new flowvr::StampInfo(pcStampName, tp) );
//					std::cout << " [portconfigure]: adding StampInfo(" << pcStampName << ", " << pcStampType << ")" << std::endl;
				}
				else
				{
					std::cerr << " [portconfigure]: error while adding StampInfo(" << pcStampName << ", " << pcStampType << ")" << std::endl;
				}
			}
		}
		return list;
	}

	/*
	 * query stamp lists using a symbolic name from a stamp map (yes... this is a macro...)
	 */
	flowvr::StampList *getStamps( const flowvr::portutils::STAMPMAP &map, const std::string &strStampListName )
	{
		flowvr::portutils::STAMPMAP::const_iterator cit = map.find(strStampListName);
		if(cit == map.end())
			return NULL;
		else
			return (*cit).second;
	}

} // namespace
namespace flowvr
{
	namespace portutils
	{

		void *openDynamicLib( const std::string &infix, int flags, std::string &resolvedName )
		{
	#if defined(__APPLE__)
			std::string systemSuffix(".dylib");
	#else
			std::string systemSuffix(".so");
	#endif

			resolvedName = infix;

			if( infix.empty() )
				return NULL;

			if( infix.size() > systemSuffix.size() )
			{
	//			if( infix.substr(0,3) != "lib" )
	//				resolvedName = std::string("lib") + resolvedName;

				if( infix.substr(infix.size()-3, 3) !=  systemSuffix )
					resolvedName += systemSuffix;
			}

			void *so = dlopen( resolvedName.c_str(), flags );
			if(!so)
			{
				std::cerr << "Attempt to open file [" << resolvedName << "] failed. dlerror as follows." << std::endl;
				std::cerr << dlerror() << std::endl;
				return NULL;
			}
			return so;
		}

		void configureStampMap( const flowvr::xml::TiXmlHandle &stampConf, STAMPMAP &map,
								const std::string &sSelectName,
								const std::string &strPrefix )
		{
			flowvr::xml::TiXmlHandle pgs = stampConf.FirstChild( "stamplist" );

			for( flowvr::xml::TiXmlElement *child = pgs.Element();
				 child;
				 child = child->NextSiblingElement("stamplist") )
			{
				const char *pcName = child->Attribute("name");
				const char *pcFromFile = child->Attribute("from");

				if(!pcName)
				{
					std::cerr << "no name given for stamplist! -- skipping" << std::endl;
					continue;
				}

				if(!sSelectName.empty())
				{
					if( std::string(pcName) != sSelectName )
						continue; // omit every entry not in equaling the select name
				}

				flowvr::StampList *list = NULL;
				flowvr::xml::TiXmlHandle layout(NULL);

				if( pcFromFile )
				{
					// load stamp layout from file given by name
					std::string resourceName = strPrefix + std::string(pcFromFile);

					flowvr::xml::TiXmlDocument config( resourceName );
					if( config.LoadFile() )
					{
						flowvr::xml::TiXmlHandle docHandle( &config );
						flowvr::xml::TiXmlHandle configNode = docHandle.FirstChild("config");
						flowvr::xml::TiXmlHandle stampsConf = configNode.FirstChild("stamplists");
						configureStampMap( stampsConf, map, std::string(pcName) );
					}
					else
					{
						std::cerr << "could not open stamp resource at [" << resourceName << "]" << std::endl;
					}
				}
				else
				{
					layout = flowvr::xml::TiXmlHandle(child);
					flowvr::StampList *list = configureStampList( layout );
					map[pcName] = list;
				}
			}
		}

		int configurePorts( const flowvr::xml::TiXmlHandle &portConf,
							const STAMPMAP &stampMap,
				std::map<std::string,flowvr::Port*> &vePortMap,
				std::vector<flowvr::Port*> &vePorts,
				const ARGS &args )
		{
			flowvr::portutils::PortHandlerRegistry *pReg = flowvr::portutils::PortHandlerRegistry::getSingleton();
			flowvr::xml::TiXmlHandle pgs = portConf.FirstChild( "port" );

			for( flowvr::xml::TiXmlElement *child = pgs.Element();
				 child;
				 child = child->NextSiblingElement("port") )
			{
				std::string name(resolve("name", child, args ));
				std::string direction(resolve("direction", child, args ));
				std::string flags(resolve("flags", child, args ));
				std::string stamps(resolve("stamps", child, args ));
				std::string multiplicity( resolve("multiplicity", child, args ) );

				if(multiplicity.empty())
					multiplicity = std::string("1");


				if(!name.empty() && !direction.empty())
				{
					size_t mlt = strtol( multiplicity.c_str(), NULL, 10 );

					for( size_t n = 0; n < mlt ; ++n )
					{
						std::string portname(name);
						if( mlt > 1 )
							portname = portname + ftl::toString<size_t>(n);

						if( vePortMap.find(portname) != vePortMap.end() )
						{
							std::cerr << "error: port with name ["
									  << portname
									  << "] already given. -- SKIP CONFIGURATION"
									  << std::endl;
							goto _loop_end;
						}

						flowvr::Port      *pPort;
						flowvr::StampList *Stamps = NULL;

						if(!stamps.empty())
						{
							STAMPMAP::const_iterator cit = stampMap.find(stamps);
							if(cit != stampMap.end())
								Stamps = (*cit).second;
							else
							{
								std::cerr << "configurePorts() -- could not find stamplist named ["
										  << stamps
										  << "] for port ["
										  << portname
										  << "]"
										  << std::endl;
							}
						}

						if( direction == "in" )
						{
							bool bEventSource = false;

							if( !flags.empty() && ( flags.find("eventsource") != std::string::npos ) )
								bEventSource = true;

							pPort = new flowvr::InputPort( portname, Stamps, false, bEventSource );
						}
						else // make it an outport
							pPort = new flowvr::OutputPort( portname, Stamps, false );

						vePorts.push_back( pPort );
						vePortMap[portname] = pPort;
					}
				}
				else
				{
					std::cerr << "either name or direction not given for port declaration" << std::endl;
				}

				_loop_end: {}
			} // loop over ports

			return vePortMap.size();
		}

		bool disposePlugins( const PortPlugin::PLGS &plugs )
		{
			for(PortPlugin::PLGS::const_iterator it = plugs.begin(); it != plugs.end(); ++it)
			{
				// dispose plugin
				(*it).second.m_dlMthd( (*it).second.m_ifc );
				dlclose( (void*)(*it).second.m_soHandle );
			}

			return true;
		}


		bool configurePlugins( const flowvr::xml::TiXmlHandle &pluginConf,
							   const SERV &services,
							   PortPlugin::PLGS &vecOut,
							   const ARGS &args )
		{
			// plugin-conf is root node to "plugins"
			flowvr::xml::TiXmlHandle pgs = pluginConf.FirstChild( "plugin" );
			flowvr::xml::TiXmlElement *child;
			for(child = pgs.Element(); child; child = child->NextSiblingElement("plugin") )
			{
				const char *name  = child->Attribute("name");
				const char *so    = child->Attribute("so");
				const char *flags = child->Attribute("flags");
				const char *service = child->Attribute("service");


				if(name and so)
				{
					int nFlags = 0;
					if(flags)
						nFlags = strtol( flags, NULL, 10 );

					PortPlugin plug;
					std::string soName;
					/*
					 * here, we open the plugin-code, which should not pollute the namespace
					 * or linkage of other plugins as well, so we chose RTLD_LOCAL as visibility
					 * rule and linkage specification.
					 */
					plug.m_soHandle = openDynamicLib(so, RTLD_NOW|RTLD_LOCAL, soName); //dlopen(soName.c_str(),RTLD_NOW|RTLD_LOCAL);
					if(!plug.m_soHandle)
						continue;
					else
					{
						std::cout << "opened plug for port ["
								  << name
								  << "] by ["
								  << soName
								  << "] using flags="
								  << nFlags
								  << std::endl;
					}

					plug.m_crMthd = (PortPlugin::crMthd)dlsym(plug.m_soHandle, "createPlugin");
					plug.m_dlMthd = (PortPlugin::dlMthd)dlsym(plug.m_soHandle, "disposePlugin");

					std::string srv(service ? service : "");

					if(plug.m_crMthd)
					{
						// search the right service layer for this plugin
						IServiceLayer *pLayer = NULL;
						if( service == NULL ) // no one given
						{
							if( services.size() == 1 ) // ok we have just one... use this
							{
								pLayer = (*services.begin()).second.m_layer;
							}
							else if(services.size() != 0) // this is an error, service is ambiguous
							{
								// error, no service given and choice is ambiguous.
								std::cerr << "service for plugin [" << name
										  << "] not given and choice is ambigous (or none service defined)"
										  << std::endl;
								dlclose( plug.m_soHandle );
								continue;
							}
						}
						else
						{
							// service tag given, look up the service from the service map
							if( !srv.empty() )
							{
								SERV::const_iterator cit = services.find( srv );
								if( cit == services.end() )
								{
									// not found... that is not good
									std::cerr << "service for plugin [" << name
											  << "] given as ["
											  << service
											  << "] -- COULD NOT BE RESOLVED."
											  << std::endl;
									dlclose( plug.m_soHandle );
									continue;
								}
								else
									pLayer = (*cit).second.m_layer;
							}
							else
								pLayer = NULL; // simply pass NONE
						}

						if( pLayer == NULL and service and !srv.empty() ) // no layer, but service was given? error!
						{
							std::cerr << "service for plugin ["
									  << name
									  << "] not created? (service name given: [" << srv << "])"
									  << std::endl;
							dlclose( plug.m_soHandle );
							continue;
						}

						if( args.hasParameter( std::string(name) ) )
						{
							const Parameter &p = args(std::string(name));
							if( p.getType() == Parameter::P_PARAMETERCONTAINER )
							{
								const ARGS &prm = args(std::string(name)).getSubContainerConstRef();
								plug.m_ifc = plug.m_crMthd(pLayer, nFlags, prm);
							}
							else
								plug.m_ifc = plug.m_crMthd(pLayer, nFlags, ARGS()); // should be a little more verbose here...
						}
						else
							plug.m_ifc = plug.m_crMthd(pLayer, nFlags, ARGS());
					}
					else
					{
						std::cerr << "strange... no creation method found for [" << name << "]?"
								  << std::endl;
					}

					plug.m_strName = std::string(name);
					plug.m_soName  = std::string(soName);
					vecOut[name] = plug; // store

				}

			}
			return true;
		}

		namespace
		{


			int retrievePortsByName( const std::list<std::string> &liPortNames,
									  const std::map<std::string, flowvr::Port*> &mpPorts,
									  std::vector<flowvr::Port*> &vePorts )
			{
				int n=0;
				for(std::list<std::string>::const_iterator cit = liPortNames.begin(); cit != liPortNames.end(); ++cit )
				{
					std::map<std::string, flowvr::Port*>::const_iterator mit = mpPorts.find(*cit);
					if(mit != mpPorts.end())
					{
						vePorts.push_back( (*mit).second );
						++n;
					}
					else
					{
						vePorts.push_back(NULL); // attention: pushing NULL here to accomodate slot model!
					}
				}

				return n;
			}

			int stringToList( const std::string &sValue, std::list<std::string> &liStrings )
			{
				char cSep = ',';
				bool bTrim = true;
				if (sValue.empty())
					return 0;

				liStrings.clear();
				std::string strTemp;

				// split list according to commas (sp?) and remove spaces
				unsigned int iPos0 = 0, iPos1 = 0, iPos2;
				while (iPos0<sValue.size())
				{
					if (bTrim)
					{
						// find first non-space
						while (iPos0 < sValue.size()
							&& isspace(sValue[iPos0]))
							++iPos0;
					}

					// find the separator
					iPos1 = iPos0;
					while (iPos1 < sValue.size()
						&& sValue[iPos1] != cSep)
						++iPos1;

					iPos2 = iPos1;
					if (bTrim)
					{
						// remove trailing spaces
						while (iPos0 < iPos1
							&& isspace(sValue[iPos1-1]))
							--iPos1;
					}

					liStrings.push_back(sValue.substr(iPos0, iPos1-iPos0));
					iPos0 = iPos2+1;
				}
				// did we end with a separator??
				if (iPos2 < sValue.size() && sValue[iPos2] == cSep)
					liStrings.push_back(std::string());

				return liStrings.size();
			}

			std::list<std::string> resolvePortNames( const std::list<std::string> &liPorts )
			{
				return liPorts;
			}
		}

		bool configurePortComm( const flowvr::xml::TiXmlHandle &codeRoot,
								std::vector<flowvr::portutils::PortComm::PortMap> &veCodeVector,
								const std::map<std::string,flowvr::Port*> &mpPorts,
								const PortPlugin::PLGS &vecPlugs )
		{
			flowvr::xml::TiXmlHandle pgs = codeRoot.FirstChild( "path" );
			flowvr::xml::TiXmlElement *child;

			flowvr::portutils::PortHandlerRegistry *pRegistry = PortHandlerRegistry::getSingleton();

			for(child = pgs.Element(); child; child = child->NextSiblingElement("path") )
			{
				const char *pcHandler = child->Attribute("plugin");
				const char *pcName    = child->Attribute("name");
				const char *pcIn      = child->Attribute("in");
				const char *pcOut     = child->Attribute("out");


				if( (pcHandler == NULL) )
				{
	//				if(pcHandler == NULL)
						std::cerr << "code instruction given, but no handling plugin defined -- SKIP ["
								  << (pcName ? pcName : "NO-NAME?")
								  << "]" << std::endl;
	//				else
	//					std::cerr << "code instruction given, but in and out ports are NULL -- SKIP ["
	//					          << (pcName ? pcName : "NO-NAME?")
	//					          << "]" << std::endl;
					continue;
				}

				std::vector<flowvr::Port*> veInPorts, veOutPorts;
				PortHandlerBase *handler = NULL;
				if(pRegistry->getIsPortHandler(std::string(pcHandler)))
				{
					handler = pRegistry->getPortHandler( std::string(pcHandler) );
				}
				else
				{
					// no, does not seem to be the case, so try to see whether it is in
					// the plugs vector provided by the user
					PortPlugin::PLGS::const_iterator cit = vecPlugs.find( std::string(pcHandler) );
					if( cit != vecPlugs.end() )
					{
						// yes, there it is
						if((*cit).second.m_ifc) // test if we can get the handler
							handler = (*cit).second.m_ifc->getPortHandler(pcName);

						// if we created it, register it with the registry
						if(handler)
							pRegistry->registerPortHandler( std::string(pcHandler), handler );
						else
							std::cerr << "** could not create port handler named [" << pcHandler
									  << "] given for name [" << pcName << "] -- (but plugin created?)" << std::endl;
					}
					else
					{
						std::cerr << "could not create/retrieve handler named ["
								  << pcHandler
								  << "] -- SKIP CODEPATH" << std::endl;
						continue;
					}
				}

				bool bError = false;

				if(handler)
				{
					handler->setHandlerName( std::string(pcHandler ) );
				}
				else
				{
					std::cout << "no handler for [" << pcHandler << "] ??" << std::endl;
					bError = true;
				}

				std::list<std::string> liNames;


				if(pcIn)
				{
					int nExpect = stringToList( std::string(pcIn), liNames );
					liNames = resolvePortNames( liNames );

					if(handler)
					{
						std::vector<std::string> v( liNames.begin(), liNames.end() );
						handler->setInPortNames(v);
					}


					bError = bError | (nExpect != retrievePortsByName( liNames, mpPorts, veInPorts ) ) ;
					liNames.clear();
				}

				if(pcOut)
				{
					int nExpect = stringToList( std::string(pcOut), liNames );
					liNames = resolvePortNames( liNames );

					if(handler)
					{
						std::vector<std::string> v(liNames.begin(), liNames.end() );
						handler->setOutPortNames(v);
					}

					bError = bError | (nExpect != retrievePortsByName( liNames, mpPorts, veOutPorts ) );
				}


				veCodeVector.push_back( flowvr::portutils::PortComm::PortMap( veInPorts, veOutPorts, handler, !bError ) );
				if(bError)
				{
					std::cerr << "PortComm[" << pcName << "] in error state -- set to inactive" << std::endl;
				}
			}
			return true;
		}

		bool disposePortComm( std::vector<flowvr::portutils::PortComm::PortMap> &veCodeVector,
							  const PortPlugin::PLGS &vecPlugs )
		{
			PortHandlerRegistry *pRegistry = PortHandlerRegistry::getSingleton();

			std::set<PortHandlerBase*> handlers;

			// got to dispose the handler (loaded by plugins) and mark the code path inactive
			for( std::vector<flowvr::portutils::PortComm::PortMap>::iterator it = veCodeVector.begin();
					it != veCodeVector.end(); ++it )
			{
				(*it).setIsActive(false);
				PortHandlerBase *handler = (*it).getHandler();

				if( handler )
				{
					handlers.insert(handler);
				}
			}

			for( std::set<PortHandlerBase*>::iterator cit = handlers.begin(); cit != handlers.end(); ++cit )
			{
				// remove from registry
				pRegistry->unregisterPortHandler( (*cit)->getHandlerName() );

				PortPlugin::PLGS::const_iterator tt = vecPlugs.find( (*cit)->getHandlerName() );
				if( tt != vecPlugs.end() )
				{
					(*tt).second.m_ifc->disposePortHandler( (*cit) );
				}
			}

			return true;
		}

		// ########################################################################
		// SERVICE LAYER
		// ########################################################################

		bool configureServiceLayers( const flowvr::xml::TiXmlHandle &serviceRoot, SERV &services )
		{
			flowvr::xml::TiXmlHandle pgs = serviceRoot.FirstChild( "service" );
			flowvr::xml::TiXmlElement *child;

			for(child = pgs.Element(); child; child = child->NextSiblingElement("service") )
			{
				const char *pcServiceName = child->Attribute("name");
				const char *pcServiceSo   = child->Attribute("so");
				const char *pcPrefix      = child->Attribute("prefix");

				if(pcServiceName && pcServiceSo)
				{
					if( services.find( pcServiceName ) != services.end() )
					{
						std::cerr << "service with name [" << pcServiceName
								  << "] already created -- skipping."
								  << std::endl;
						continue;
					}

					ServicePlug plug = openServiceLayer( pcServiceSo, pcServiceName );
					if(plug.m_soHandle == NULL)
					{
						std::cerr << "could not create service plug." << std::endl;
						continue;
					}

					if(pcPrefix)
						plug.m_Prefix = std::string(pcPrefix);

					services[pcServiceName] = plug;
				}
				else
				{
					std::cerr << "service declaration needs 'name' and 'so' attribute set. " << std::endl
							 <<  "given: \n\tname:"
							 << (pcServiceName ? pcServiceName : "<NONE>")
							 << std::endl
							 << "\tso: "
							 << (pcServiceSo ? pcServiceSo : "<NONE>")
							 << std::endl;

				}

			}

			return true;
		}

		bool disposeServiceLayers( SERV &services )
		{
			for( SERV::iterator it = services.begin(); it != services.end(); ++it )
			{
				closeServiceLayer( (*it).second );
			}
			return true;
		}

		bool prepareServices( const SERV &services )
		{
			for(SERV::const_iterator cit = services.begin(); cit != services.end(); ++cit)
			{
				if( (*cit).second.m_dlArgs )
				{
					// call argument setup
	//				std::cout << "prefix = " << (*cit).second.m_Prefix << std::endl;
					(*cit).second.m_dlArgs( (*cit).second.m_Prefix );
				}
			}
			return true;
		}

		bool createServices( SERV &services, Module &parent, const ARGS &args )
		{
			for(SERV::iterator cit = services.begin(); cit != services.end(); ++cit)
			{
				if( (*cit).second.m_crMthd and !(*cit).second.m_layer )
				{
					// call argument setup
					(*cit).second.m_layer = (*cit).second.m_crMthd(parent);
					if((*cit).second.m_layer)
					{
						if( args.hasParameter( (*cit).second.m_strName ) )
						{
							ARGS::const_iterator i = args.find((*cit).second.m_strName);
							const Parameter &p = (*i).second;

							if(p.getType() == Parameter::P_PARAMETERCONTAINER )
							{
								(*cit).second.m_layer->setParameters(p.getSubContainerConstRef());
							}
						}
						else
							std::cerr << "no params for [" << (*cit).second.m_strName << "]" << std::endl;
					}
				}
			}
			return true;
		}

		ServicePlug openServiceLayer( const std::string &so, const std::string &strObjectName )
		{
			ServicePlug plug;

			std::string soName;
			/*
			 * we open the dll using RTLD_GLOBAL to assure that any symbol incoming from
			 * this service layer is available to the plugs as well.
			 */
			plug.m_soHandle = openDynamicLib(so, RTLD_NOW|RTLD_GLOBAL, soName);
			if(!plug.m_soHandle)
			{
				std::cerr << dlerror() << std::endl;
				return plug;
			}
	//		else
	//		{
	//			std::cout << "opened service ["
	//					  << strObjectName
	//					  << "] by ["
	//					  << so
	//					  << "]"
	//					  << std::endl;
	//		}

			plug.m_crMthd  = (ServicePlug::crMthd)dlsym(plug.m_soHandle, "createService");
			plug.m_dlMthd  = (ServicePlug::dlMthd)dlsym(plug.m_soHandle, "disposeService");
			plug.m_dlArgs  = (ServicePlug::argsMthd)dlsym(plug.m_soHandle, "createArgs");
			plug.m_dlParmsG = (ServicePlug::paramGMthd)dlsym(plug.m_soHandle, "getParameters");


			plug.m_strName = std::string(strObjectName);
			plug.m_soName  = std::string(soName);

			return plug;
		}

		void closeServiceLayer( ServicePlug &pLayer )
		{
			if( pLayer.m_dlMthd && pLayer.m_layer)
			{
				pLayer.m_dlMthd( pLayer.m_layer );
			}

			if(pLayer.m_soHandle)
				dlclose( (void*)pLayer.m_soHandle );

			pLayer.m_soHandle  = NULL;
			pLayer.m_layer     = NULL;
			pLayer.m_crMthd    = NULL;
			pLayer.m_dlMthd    = NULL;
			pLayer.m_dlParmsG  = NULL;
		}

	} // namespace portutils
} // namespace flowvr
