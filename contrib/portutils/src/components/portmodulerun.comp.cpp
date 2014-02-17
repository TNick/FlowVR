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

#include <flowvr/portutils/components/portmodulerun.comp.h>
#include <flowvr/portutils/components/portmodule.comp.h>
#include <flowvr/portutils/components/portparallel.comp.h>
#include <flowvr/portutils/components/portinterfacecache.h>

#include <flowvr/app/core/exception.h>

#include <ftl/convert.h>
#include <flowvr/utils/filepath.h>


#include <algorithm>
#include <functional>

namespace
{

	flowvr::utils::FilePath portfilepath( "FLOWVR_PORTFILE_PATH" );

	/**
	 * @brief attempt to resolve parameter values with a given pre-fix indirectly.
	 *
	 * - $: resolve from environment ($FOO resolves from environment variable FOO)
	 * - @: resolve from another parameter by name ($FOO resolves from parameter FOO)
	 * - #: resolve from environment first, if that fails, resolve from parameter
	 *
	 * Can be nested, a recursion depth of 256 is used to stop searching.
	 */
	std::string resolve( const std::string &name, const flowvr::app::Composite &container, int nDepth = 0 )
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


	struct _hlpParam
	{
		_hlpParam( const std::string &strName, const flowvr::portutils::Parameter &parm )
		: m_strName(strName)
		, m_param(parm)
		{

		}

		std::string m_strName;
		flowvr::portutils::Parameter m_param;
	};

	void getConcatParamNames( const flowvr::portutils::Parameter &param,
			                         std::list<_hlpParam> &sList,
			                         const std::string &strPrefix = "" )
	{
		if( param.getType() == flowvr::portutils::Parameter::P_PARAMETERCONTAINER )
		{
			const flowvr::portutils::ParameterContainer &lst = param.getSubContainerConstRef();
			for( flowvr::portutils::ParameterContainer::const_iterator cit = lst.begin(); cit != lst.end(); ++cit )
			{
				if( !strPrefix.empty() )
					getConcatParamNames( (*cit).second, sList, strPrefix + std::string("-") + (*cit).first  );
				else
					getConcatParamNames( (*cit).second, sList, (*cit).first  );
			}
		}
		else
		{
			sList.push_back( _hlpParam( strPrefix, param )  );
		}
	}

	void _print( const _hlpParam &strOut )
	{
		std::cout << strOut.m_strName << ": ";
		strOut.m_param.print();
	}

	/**
	 * @brief helper functor to declare (addParameter) a flowvr::app::Parameter
	 *        for a given component.
	 *
	 * It distincts between optional and mandatory parameters.
	 */
	class _addParameter : public std::unary_function<void, const _hlpParam>
	{
	public:
		_addParameter( flowvr::portutils::PortModuleRun &module )
		: m_module(module)
		{

		}

		void operator()( const _hlpParam &param ) const
		{
			std::string strValue = param.m_param.getValueString();

			switch( param.m_param.getModeType() )
			{
				case flowvr::portutils::Parameter::MD_MANDATORY:
				{
					if( strValue.empty() )
						m_module.addParameter( param.m_strName ); // do not pass a default value
					else
						m_module.addParameter( param.m_strName, strValue );
					break;
				}
				case flowvr::portutils::Parameter::MD_OPTIONAL:
				default:
				{
					m_module.addParameter( param.m_strName, strValue );
				}
			}
		}

		flowvr::portutils::PortModuleRun &m_module;
	};

	/**
	 * @brief helper operator to set the parameters in the commandline, using the long version
	 *        and the associated value for the parameter.
	 */
	class _setParameter : public std::unary_function<void, const _hlpParam>
	{
	public:
		_setParameter( flowvr::portutils::PortModuleRun &module, flowvr::app::CmdLine &run )
		: m_module(module)
		, m_run(run)
		{

		}

		void operator()(const _hlpParam &param) const
		{
			try
			{
				std::string strParameter = resolve( param.m_strName, m_module ); // m_module.getParameter<std::string>( param.m_strName );
				// we have a no-count arg here, so strParameter has to have some value
				if( strParameter.empty() )
					return;

				m_run.addArg( std::string("--") + param.m_strName + std::string(" ") + strParameter );
			}
			catch( flowvr::app::ParameterException &p )
			{
				std::cout << p.what() << std::endl;
			}
		}

		flowvr::portutils::PortModuleRun &m_module;
		flowvr::app::CmdLine &m_run;
	};

	/**
	 * @brief helper operator to put parameters from flowvr::Parameter
	 *        to an ARGS tree representation that is passed in an encoded form to
	 *        the modules.
	 *
	 * This is an operator / functor that
	 * - tries to call resolve() on the parameter name (thus calling getParameter<string>(name)
	 * - will erase empty NO_VALUE / OPTIONAL parameters
	 * - add the set parameter (or the default value) to the ARGS tree given, knowing
	 *   the difference between module / plugins / services level.
	 * - it assumes a structure like
	 *   - <module>-<parametername> -> value (for module parameter)
	 *   - <plugins|services>-<plug|servicename>-<parametername> -> value (for plugin / service parameter)
	 */
	class _toArgTree : public std::unary_function<void, const _hlpParam>
	{
	public:
		_toArgTree( flowvr::portutils::PortModuleRun &module, flowvr::portutils::ARGS &args )
		: m_args(args)
		, m_module(module)
		{
		}

		void operator()( const _hlpParam &param ) const
		{
			try
			{
				std::string strParameter = resolve( param.m_strName, m_module );
				// we have a no-count arg here, so strParameter has to have some value
				if( strParameter.empty() )
				{
					// special case: no default value given, and it is marked as "novalue" + "optional" ->
					// there is a special code line testing for the existence in the arg-tree in the portmodule
					// during run-time (e.g.: if( args.hasParameter() ) -- or similar...
					// so for that case we remove the parameter from the final tree
					if( param.m_param.getSetType() == flowvr::portutils::Parameter::ST_NOVALUE and param.m_param.getModeType() == flowvr::portutils::Parameter::MD_OPTIONAL )
					{
						// erase parameter from map
						std::string::size_type n1 = param.m_strName.find_first_of('-', 0 );
						std::string::size_type n2 = param.m_strName.find_first_of('-', n1+1);

						if( n1 == std::string::npos )
							return; // not a single '-' found, this is a syntax error

						if( n2 == std::string::npos )
						{
							// module parameter...
							std::string type( param.m_strName.begin(), param.m_strName.begin()+n1 );
							std::string pname( param.m_strName.begin()+n1+1, param.m_strName.end());
							if( m_args.hasParameter(type) )
							{
								flowvr::portutils::ParameterContainer &t = m_args[type].getSubContainer();
								flowvr::portutils::ARGS::iterator it = t.find( pname );
								if( it != t.end() )
								{
									t.erase( it );
//									std::cout << "erasing [" << param.m_strName << "] from its map (module parameter)." << std::endl;
								}
								else
								{
									std::cerr << "could not find [" << param.m_strName << " ; " << pname << "] in this map?" << std::endl;
									t.print();
								}
							}
						}
						else
						{
							// this is either a service or a plugin parameter
							// structure: <[plugin|service]>-<name>-<parameter-name>
							std::string type( param.m_strName.begin(), param.m_strName.begin()+n1 );

							std::string name( param.m_strName.begin()+n1+1, param.m_strName.begin()+n2);
							std::string pname( param.m_strName.begin()+n2+1, param.m_strName.end());

							// get type tree
							if( m_args.hasParameter(type) ) // either plugin or service sub-tree
							{
								flowvr::portutils::ParameterContainer &t = m_args[type].getSubContainer(); // get param container by name
								flowvr::portutils::ARGS::iterator it = t.find( name );
								if( it != t.end() )
								{
									flowvr::portutils::ParameterContainer::iterator prm = ((*it).second.getSubContainer()).find(pname); // get param container by name
									((*it).second.getSubContainer()).erase( prm );
//									std::cout << "erasing [" << param.m_strName << "] from its map. (plugin / service parameter)" << std::endl;
								}
								else
								{
									std::cerr << "could not find [" << param.m_strName << " ; " << pname << "] in this map?" << std::endl;
									t.print();
								}
							}
						}
					}
					return;
				}

				// break-up parameter name <type>-<name>-<paramname>
				std::string::size_type n1 = param.m_strName.find_first_of('-', 0 );
				std::string::size_type n2 = param.m_strName.find_first_of('-', n1+1);

				if( n1 == std::string::npos )
					return; // not a single '-' found, this is a syntax error

				if( n2 == std::string::npos )
				{
					// this is a module parameter
					std::string type( param.m_strName.begin(), param.m_strName.begin()+n1 );
					std::string pname( param.m_strName.begin()+n1+1, param.m_strName.end());
					if( m_args.hasParameter(type) )
					{
						flowvr::portutils::ParameterContainer &t = m_args[type].getSubContainer();
						if( t.hasParameter(pname) )
						{
							flowvr::portutils::Parameter &p = t(pname);
							p.setValue(strParameter, p.getType(), p.getSubType(), p.getSetType(), p.getModeType(), p.getHelpString() );

							// change type: we got the parameter by file
							p.setSetType( flowvr::portutils::Parameter::ST_FILE );
						}
						else
						{
							std::cerr << " -- parameter resolve: could not find by type: [" << type << "]" << std::endl
									  << " t is: "  << std::endl;
							t.print();
						}
					}
				}
				else
				{
					// this is either a service or a plugin parameter
					// structure: <[plugin|service]>-<name>-<parameter-name>
					std::string type( param.m_strName.begin(), param.m_strName.begin()+n1 );

					std::string name( param.m_strName.begin()+n1+1, param.m_strName.begin()+n2);
					std::string pname( param.m_strName.begin()+n2+1, param.m_strName.end());

					// get type tree
					if( m_args.hasParameter(type) ) // either plugin or service sub-tree
					{
						flowvr::portutils::ParameterContainer &t = m_args[type].getSubContainer(); // get param container by name
						if( t.hasParameter(name) ) // see whether we have a component called 'name'
						{
							// yes
							flowvr::portutils::ParameterContainer &c = t[name].getSubContainer(); // get the container for that sub-component
							flowvr::portutils::Parameter &p = c(pname); // search a parameter named $pname (can be NIL ; is not quite clean!
																// we can modify that parameter here!)

							// copy these values
							p.setValue(strParameter, p.getType(), p.getSubType(), p.getSetType(), p.getModeType(), p.getHelpString() );

							// change type: we got the parameter by file
							p.setSetType( flowvr::portutils::Parameter::ST_FILE );
						}
						else
						{
							std::cerr << " -- parameter resolve: could not find by name: [" << name << "]" << std::endl
									  << " t is: "  << std::endl;
							t.print();
						}
					}
					else
					{
						std::cerr << " -- parameter resolve: could not find by type [" << type<< "]" << std::endl
								  << " margs is: " << std::endl;
						m_args.print();
					}
				}
			}
			catch( flowvr::app::ParameterException &p )
			{
				std::cout << p.what() << std::endl;
			}
		}

		flowvr::portutils::ARGS &m_args;
		flowvr::portutils::PortModuleRun &m_module;
	};

}

namespace flowvr
{
	namespace portutils
	{
		PortModuleRun::PortModuleRun()
		: flowvr::app::MetaModule("")
		, run(NULL)
		, m_prototype(NULL)
		{}

		PortModuleRun::PortModuleRun(const std::string& id_,
									 const std::string &interfacefile,
									 const flowvr::app::CmdLine &pRun,
									 flowvr::app::Composite *parent,
									 const std::string &strModuleName,
									 bool bAllowOverride)
		: MetaModule(id_)
		, run( dynamic_cast<flowvr::app::CmdLine*>( pRun.clone()) ) // copy
		, m_prototype( NULL ) // clone
		, strPortfile( interfacefile )
		, strModuleBinary( strModuleName )
		, m_bAllowOverride(bAllowOverride)
		{
			run->changeHostComp(this);

			setInfo("PortModuleRun is a launcher for a component.");

			addParameter("dbgWait", false);
			addParameter("initWait", false);
			addParameter("modulebinary", strModuleName);

			m_prototype = new PortModule("body", strPortfile);

			for(pPortIterator it = (*m_prototype).getPortBegin(); it != (*m_prototype).getPortEnd(); ++it)
				addPort((*it)->getId(), (*it)->getType(), (*it)->getMsgType(),
						((*it)->getMsgType() == flowvr::app::SAMEAS ? (*it)->getSameAs() : ""),
						(*it)->getBlockingState() );


			PortInterfaceCache *cache = PortInterfaceCache::getSingleton();
			PortInterfaceCache::Interface interface = cache->getInterface(strPortfile);
			if(interface.getIsValid())
			{
				std::list<_hlpParam> lModuleArgs;
				getConcatParamNames( interface.getServiceParameters(), lModuleArgs, "services" );
				getConcatParamNames( interface.getPluginParameters(),  lModuleArgs, "plugins");
				getConcatParamNames( interface.getModuleParameters(),  lModuleArgs, "module");
				std::for_each( lModuleArgs.begin(), lModuleArgs.end(), _addParameter(*this) );
			}

			if(parent)
				parent->insertObject(this);
		}

		PortModuleRun::PortModuleRun(const PortModuleRun& m)
		: MetaModule(m) // will clone children and ports from m
		, run( dynamic_cast<flowvr::app::CmdLine*>( m.getRun()->clone() ) ) // in case m is not assigned to a run, this will throw
		, strPortfile(m.strPortfile)
		, m_prototype( NULL )
		{
			run->changeHostComp(this);

			addParameter("dbgWait", false);
			addParameter("initWait", false);
			addParameter("modulebinary", m.strModuleBinary );

			if(m.m_prototype)
				m_prototype = m.m_prototype->create(); // clone from other proto
			else
				m_prototype = new PortModule("body", strPortfile ); // probably not finished?

			// parse the port according to the file given
			// clone ports from the portmodule
			for(pPortIterator it = (*m_prototype).getPortBegin(); it != (*m_prototype).getPortEnd(); ++it)
				addPort((*it)->getId(), (*it)->getType(), (*it)->getMsgType(),
						((*it)->getMsgType() == flowvr::app::SAMEAS ? (*it)->getSameAs() : ""),
						(*it)->getBlockingState() );

			PortInterfaceCache *cache = PortInterfaceCache::getSingleton();
			PortInterfaceCache::Interface interface = cache->getInterface(strPortfile);
			if(interface.getIsValid())
			{
				std::list<_hlpParam> lModuleArgs;
				getConcatParamNames( interface.getServiceParameters(), lModuleArgs, "services" );
				getConcatParamNames( interface.getPluginParameters(),  lModuleArgs, "plugins");
				getConcatParamNames( interface.getModuleParameters(),  lModuleArgs, "module");
				std::for_each( lModuleArgs.begin(), lModuleArgs.end(), _addParameter(*this) );
			}

		}



		PortModuleRun::~PortModuleRun()
		{
			delete run;
			delete m_prototype;
		}

		flowvr::app::Component* PortModuleRun::create() const
		{
			return new PortModuleRun(this->getId(), strPortfile, *run );
		}

		flowvr::app::CmdLine*  PortModuleRun::getRun() const
		{
			if (run == NULL)
				throw flowvr::app::NullPointerException(this->getType(), this->getFullId(), __FUNCTION_NAME__);
			return run;
		}


		const flowvr::app::Component &PortModuleRun::getPrototype() const
		{
			return *m_prototype;
		}

		flowvr::xml::DOMElement* PortModuleRun::XMLRunBuild() const
		{
			flowvr::xml::DOMElement* result = new flowvr::xml::DOMElement("run");

			std::string metamoduleId = getFullId();
			if ( !getMetaModuleIdSuffix().empty() )
			{
				metamoduleId += (std::string("/") + getMetaModuleIdSuffix());
			}

			result->SetAttribute("metamoduleid", metamoduleId);

			std::string runstring;

			try
			{
				runstring = getRun()->getString(); // call getString() on run to build the run  script line.
			}
			catch (flowvr::app::CustomException& e)
			{
				delete result;
				throw(e);
			}

			if (runstring.empty())
			{
				delete result;
				throw flowvr::app::CustomException(getType() + " " + getFullId() + ": empty run string", __FUNCTION_NAME__);
			}


			flowvr::xml::DOMText* cmd = new flowvr::xml::DOMText(runstring);
			result->LinkEndChild(cmd);
			return result;
		}

		void PortModuleRun::configure()
		{
			if(run)
			{
				std::string strModuleName = (m_bAllowOverride ? getParameter<std::string>("modulebinary") : strModuleBinary);
				std::string strDgbWait    = getParameter<std::string>("dbgWait");
				std::string strInitWait    = getParameter<std::string>("initWait");


				// give the binary name to launch first
				(*run).addArg(strModuleName);

				// append other arguments here..
				(*run).addArg( "--portfile " + strPortfile );

				if(!strDgbWait.empty() && convertTo<bool>(strDgbWait) )
					(*run).addArg( "--dbgWait");

				if(!strInitWait.empty() && convertTo<bool>(strInitWait) )
					(*run).addArg( "--initWait");


				(*run).addArg( "--idstring " + getId() );

				PortInterfaceCache *cache = PortInterfaceCache::getSingleton();
				PortInterfaceCache::Interface interface = cache->getInterface(strPortfile);
				if(interface.getIsValid())
				{
					std::list<_hlpParam> lModuleArgs;
					getConcatParamNames( interface.getServiceParameters(), lModuleArgs, "services" );
					getConcatParamNames( interface.getPluginParameters(),  lModuleArgs, "plugins" );
					getConcatParamNames( interface.getModuleParameters(),  lModuleArgs, "module" );

					flowvr::portutils::ARGS args = interface.m_mpARGS; // make a copy

					std::for_each( lModuleArgs.begin(), lModuleArgs.end(), _toArgTree( *this, args ) );

					std::string sb64;
					size_t length = flowvr::portutils::ParameterContainer::toBase64( args, sb64 );
					sb64 = toString<size_t>(length) + std::string(":") + sb64;

					(*run).addArg( "--argtree "+ sb64 );

				}
			}
		}

		void PortModuleRun::execute()
		{
			if( hostList.empty() )
				throw flowvr::app::CustomException("PortModuleRun " + getFullId() + ": empty host list (at least one expected)", __FUNCTION_NAME__);

			Component *body;

			if (hostList.size() > 1)
			{
				if( !getRun()->isParallel() )
					throw flowvr::app::CustomException("PortModuleRun " + getFullId() + ": singleton metamodule but "+ toString<int>(hostList.size())+ " hosts given for mapping (one expected)", __FUNCTION_NAME__);
				body = new PortParallel( "body", *m_prototype );
				body->setParameter("LOOKUP_NB_INSTANCES_METHOD", "FROM_HOSTS");
				getRun()->changeHostComp(body);
			}
			else
			{
				if( getRun()->isParallel() )
				{
					throw flowvr::app::CustomException("PortModuleRun " + getFullId() + ": singleton metamodule but parallel launcher", __FUNCTION_NAME__);
				}

				body = m_prototype->create();
				body->setId("singleton");
				body->hostList.clear();
				body->hostList.push_back(hostList.front());
				body->hostList.front().appendtoOrigin("->"+getId());
			}

			try
			{
				insertObject(body);
			}
			catch(flowvr::app::CustomException &e)
			{
				delete body;
				throw e;
			}

			// mirror interface
			for(pPortIterator it = getPortBegin(); it != getPortEnd(); ++it)
				link(*it, body->getPort((*it)->getId()));

			setMetaModuleIdSuffix(body->getId());
			configure();
		}

	} // namespace portutils
} // namespace flowvr
