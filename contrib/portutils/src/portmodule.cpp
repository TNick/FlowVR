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

#include <flowvr/portutils/portmodule.h>
#include <flowvr/portutils/portconfigure.h>
#include <flowvr/portutils/porthandler.h>


#include <flowvr/xml.h>
#include <flowvr/moduleapi.h>
#include <flowvr/utils/cmdline.h>
#include <flowvr/mem/sharedmemorymanager.h>

#include <flowvr/utils/timing.h>
#include <flowvr/utils/filepath.h>


namespace
{
	flowvr::utils::FilePath descFiles( "FLOWVR_PORTFILE_PATH" );

	class _callPreLoop : public std::unary_function<void, flowvr::portutils::PortComm::PortMap>
	{
	public:
		void operator()( flowvr::portutils::PortComm::PortMap &mp )
		{
			flowvr::portutils::PortHandlerBase *handler = mp.getHandler();
			if( handler )
				handler->preLoopActivation();
		}
	};


	class _compSoName : public std::unary_function<bool, const std::string>
	{
	public:
		_compSoName( const std::string &strSoName )
		: m_strSoName( strSoName )
		{

		}

		bool operator()(const std::pair<std::string, flowvr::portutils::ServicePlug> &el) const
		{
			return (m_strSoName == el.second.m_soName);
		}

		std::string m_strSoName;
	};

	class _compName : public std::unary_function<bool, const std::string>
	{
	public:
		_compName( const std::string &strSoName )
		: m_strSoName( strSoName )
		{

		}

		bool operator()(const std::pair<std::string, flowvr::portutils::ServicePlug> &el) const
		{
			return (m_strSoName == el.second.m_strName );
		}

		std::string m_strSoName;
	};

}


/////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////////////
namespace flowvr
{
	namespace portutils
	{
		Module::Module( )
		: loop(NULL) // pass NULL (= no module, yet)
		, flowvr(NULL)
		, isClosing(false)
		, m_argc(0)
		, m_argv(NULL)
		{
		}

		Module::~Module()
		{
			// delete stamplists contained in stampmap
			for( STAMPMAP::iterator it = stampMap.begin(); it != stampMap.end(); ++it )
			{
				delete (*it).second;
			}

			flowvr::portutils::disposePortComm(mpPortComms, plugs);
			flowvr::portutils::disposePlugins(plugs);
			flowvr::portutils::disposeServiceLayers(services);

			delete loop;
			delete flowvr;
		}


		bool Module::getIsClosing() const
		{
			return isClosing;
		}

		bool Module::init(const std::string &strModuleText, int argc, char **argv, bool bStrictParse)
		{
			m_argc = argc;
			m_argv = argv;

			loop = new PortComm(NULL);

			std::string strConfigFile;
			// allocate option on heap (no delete on purpose)
			flowvr::utils::Option<std::string> *portFile =
					new flowvr::utils::Option<std::string>("portfile", 'p', "path to the module parameter file", false);
			flowvr::utils::Option<bool> *dgbWait =
					new flowvr::utils::Option<bool>("dbgWait", 'd', "wait infinite to have a debugger attached", false, true);

			flowvr::utils::Option<bool> *initWait =
					new flowvr::utils::Option<bool>("initWait", 'd', "wait infinite to have a debugger attached (init routine)", false, true);

			flowvr::utils::Option<std::string> *idstring =
					new flowvr::utils::Option<std::string>("idstring", 'i', "id string passed from flowvr-app", true);

			flowvr::utils::Option<std::string> *argtree =
					new flowvr::utils::Option<std::string>("argtree", 'a', "base64 encoded / bzip2 compressed arguments", false);

			flowvr::utils::CmdLine line(std::string(argv[0]) + strModuleText );

			bool bError=false;

			if(line.parse( argc, argv, &bError ) == false and bError == false )
			{
				// help option
				std::cout << line.help() << std::endl;
				return false;
			}

			if((*portFile).count == 0)
			{
				// ok, we can stop here... we NEED a portfile declaration
				if( idstring->count == 0 )
					std::cerr << "no portfile specified. STOP." << std::endl;
				else
					std::cerr << "[" << idstring->value() << "] -- no portfile specified. STOP." << std::endl;
				return false;
			}

			if( (*initWait).value() == true )
			{
				std::cerr << "PortComm::setInifiteWaitFlag() during init for [" << getpid() << "]-- ATTENTION" << std::endl;
				bool b = true;
				while(b)
				{
					flowvr::utils::microsleep(1000);
				}
			}

			// cache value, as the upcoming parsing might reset that it was already written,
			// so we lose the value on the second parsing
			std::string strIdTag = idstring->value();
			std::string argT     = argtree->value();

			std::string pFile( (*portFile).value() );

			if( !descFiles.exists( pFile ) )
			{
				if(!descFiles.findFile( pFile ))
				{
					std::cerr << " ** could not resolve portfile [" << pFile << "]" << std::endl;
					return false;
				}
			}

			flowvr::xml::TiXmlDocument config( pFile.c_str() );
			if( config.LoadFile() )
			{
				// configure plugins first
				flowvr::xml::TiXmlHandle docHandle( &config );

				flowvr::xml::TiXmlHandle configNode  = docHandle.FirstChild("config");
				flowvr::xml::TiXmlHandle xmlServices = configNode.FirstChild("services");

				flowvr::xml::TiXmlHandle ports      = configNode.FirstChild("ports");
				flowvr::xml::TiXmlHandle xmlPlugs   = configNode.FirstChild("plugins");
				flowvr::xml::TiXmlHandle code       = configNode.FirstChild("code");
				flowvr::xml::TiXmlHandle stampsConf = configNode.FirstChild("stamplists");


				std::map<std::string, flowvr::Port*> mpPorts;
				std::vector<flowvr::Port*>           vePorts;

				flowvr::portutils::ARGS argmap;

				std::string::size_type n = argT.find_first_of(':', 0);
				if( !n or n == std::string::npos )
				{
					std::cerr << argtree->count << ": argtree given, but no size prefix!" << std::endl
							  << "argtree = " << argtree->value() << std::endl;
					argmap["services"] = flowvr::portutils::Parameter( flowvr::portutils::ParameterContainer() );
					argmap["plugins"]  = flowvr::portutils::Parameter( flowvr::portutils::ParameterContainer() );
					argmap["module"]   = flowvr::portutils::Parameter( flowvr::portutils::ParameterContainer() );
				}
				else
				{
					std::string sb64( argT.begin()+n+1, argT.end() );
					std::string sSize( argT.begin(), argT.begin()+n);

					flowvr::portutils::ARGS::fromBase64(sb64, argmap, atol(sSize.c_str()) );
				}

				flowvr::portutils::configureServiceLayers( xmlServices, services );

				// create argument listings
				flowvr::portutils::prepareServices( services );
				flowvr::portutils::createServices(services, *this, argmap["services"].getSubContainerConstRef());
				flowvr::portutils::configurePlugins(xmlPlugs, services, plugs, argmap["plugins"].getSubContainerConstRef() );

				// load the stamp list for this configuration.
				// the stamp list is expected to reside in the very same directory
				// as the portfile itself, so we look for the prefix in the path given
				// for the portfile.
				std::string strPrefix;
				std::string::size_type np = pFile.find_last_of("/"); // is it like: /<path>/<path>/
				if( np != std::string::npos )
				{
					// yes... prefixed with a relative or absolute path
					strPrefix = pFile.substr(0,np+1); // include slash
				} // else: no, its probably in the same directory, so prefix will be empty.

				flowvr::portutils::configureStampMap( stampsConf, stampMap, "", strPrefix );

				flowvr::portutils::configurePorts( ports, stampMap, mpPorts, vePorts, argmap["module"].getSubContainerConstRef() );
				flowvr::portutils::configurePortComm( code, mpPortComms, mpPorts, plugs );

				flowvr =  flowvr::initModule( vePorts );
			}
			else
			{
				std::cerr << "could not load port-config file given as [" << pFile << "]" << std::endl;
				const char *pcErr = config.ErrorDesc();
				if(pcErr)
					std::cerr << "row: " << config.ErrorRow() << std::endl
							  << "col: " << config.ErrorCol() << std::endl
							  << "err: " << pcErr << std::endl;
				return false;
			}

			if((*dgbWait).value())
				(*loop).setInfiniteWaitFlag();

			std::vector<IServiceLayer*> v;

			for(SERV::const_iterator cit = services.begin(); cit != services.end(); ++cit )
			{
				if( (*cit).second.m_layer )
					v.push_back( (*cit).second.m_layer );
			}

			(*loop).setServices(v);

			(*loop).setCodeRange( mpPortComms );

			(*loop).setModuleAPI(flowvr);
			(*loop).setDefaultReturn(flowvr::portutils::PortComm::RET_FAIL);

			if( flowvr )
			{
				std::for_each( v.begin(), v.end(), std::mem_fun( &flowvr::portutils::IServiceLayer::Once ) );

				for( std::vector<PortComm::PortMap>::iterator it = mpPortComms.begin(); it != mpPortComms.end(); ++it )
					(*it).once(flowvr);
			}

			return true;
		}


		void Module::getArgs( int &argc, char **&argv ) const
		{
			argc = m_argc;
			argv = m_argv;
		}

		void Module::close()
		{
			isClosing = true;
			if(flowvr)
				flowvr->close();
			isClosing = false;
		}

		void Module::callPreLoopActivation()
		{
			// iterate over all handlers one time, and call the pre-loopactivation API
			std::for_each( mpPortComms.begin(), mpPortComms.end(), _callPreLoop() );
		}

		int Module::process()
		{
			while( (*loop).process() != flowvr::portutils::PortComm::RET_FAIL )
			{
				// empty on purpose
			}
			return 0;
		}

		PortComm &Module::getPortComm() const
		{
			return *loop;
		}

		flowvr::Allocator &Module::getAllocator() const
		{
			if(!flowvr)
				throw std::exception(); // no way to coninue here...

			return *(*flowvr).getAllocator();
		}

		flowvr::ModuleAPI *Module::getModuleAPI() const
		{
			return flowvr;
		}

		void Module::registerServicePlug( const ServicePlug &plug )
		{
			services[ plug.m_strName ] = plug;
		}


		ServicePlug Module::getServiceBySoName( const std::string &strSoName ) const
		{
			SERV::const_iterator cit = std::find_if( services.begin(),
													 services.end(),
													 _compSoName( strSoName ) );
			if( cit == services.end() )
				return ServicePlug();

			return (*cit).second;
		}

		ServicePlug Module::getServiceByName( const std::string &strName ) const
		{
			SERV::const_iterator cit = std::find_if( services.begin(),
													 services.end(),
													 _compName( strName ) );
			if( cit == services.end() )
				return ServicePlug();

			return (*cit).second;
		}
	} // namespace portutils
} // namespace flowvr
