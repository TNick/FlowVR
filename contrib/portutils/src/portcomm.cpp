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

#include <flowvr/portutils/portcomm.h>
#include <flowvr/portutils/porthandler.h>
#include <flowvr/portutils/portservicelayer.h>

#include <algorithm>
#include <set>

#include <flowvr/moduleapi.h>
#include <flowvr/utils/timing.h>

#include <iostream>
#include <unistd.h>

namespace
{
	class _processMsg : public std::unary_function<flowvr::portutils::PortComm::MsgMap&,bool>
	{
	public:
		_processMsg()
		: m_dirty(false)
		{

		}

		bool operator()( flowvr::portutils::PortComm::MsgMap &mp )
		{
			if( mp.m_parent.getHandler() == NULL )
			{
				std::cout << "handler on NULL?" << std::endl;
				mp.m_parent.setIsActive(false);
				return false;
			}
			flowvr::portutils::PortHandlerBase::eState dirt = mp.m_parent.getHandler()->handleMessages( mp );
			m_dirty = m_dirty | (dirt==flowvr::portutils::PortHandlerBase::E_OK?true:false);
			if( (dirt == flowvr::portutils::PortHandlerBase::E_REMOVE) or (dirt == flowvr::portutils::PortHandlerBase::E_ERROR) )
			{
				mp.m_parent.setIsActive(false);
			}

			return true;
		}


		bool m_dirty,
		     m_bActive;

	};


	class _deletePort : public std::unary_function<flowvr::Port*,void>
	{
	public:
		void operator()( flowvr::Port *port ) const
		{
			delete port;
		}
	};

	/**
	 * one functor to collect and delete ports at the same time in a set.
	 */
	class _collectPorts : public std::unary_function<flowvr::portutils::PortComm::PortMap&,void>
	{
	public:
		_collectPorts( std::set<flowvr::Port*> &setPort )
		: m_setPorts(setPort) {}


		/**
		 * collect port-pointers from a port-map (clear port-map pointer list afterwards)
		 */
		void operator()( flowvr::portutils::PortComm::PortMap &mp )
		{
			// pass *this (!selects operator(port)!)
			std::for_each( mp.m_InPorts.begin(), mp.m_InPorts.end(), *this );
			mp.m_InPorts.clear();

			// pass *this (!selects operator(port)!)
			std::for_each( mp.m_OutPorts.begin(), mp.m_OutPorts.end(), *this );
			mp.m_OutPorts.clear();
		}

		/**
		 * collect operation.
		 */
		void operator() ( flowvr::Port *port )
		{
			m_setPorts.insert(port);
		}

		std::set<flowvr::Port*> &m_setPorts;


	};

	class _preLoop : public std::unary_function<flowvr::portutils::IServiceLayer*, bool>
	{
	public:
		bool operator()(flowvr::portutils::IServiceLayer *layer)
		{
			return layer->PreLoop();
		}
	};

	class _postLoop : public std::unary_function<flowvr::portutils::IServiceLayer*, bool>
	{
	public:
		bool operator()(flowvr::portutils::IServiceLayer *layer)
		{
			return layer->PostLoop();
		}
	};


}


namespace flowvr
{
	namespace portutils
	{
		bool PortComm::PortMap::getIsActive() const
		{
			return m_bActive;
		}

		void PortComm::PortMap::setIsActive( bool bActive )
		{
			m_bActive = bActive;
	//		std::cout << "PortMap activation change: setting " << getHandler()->getHandlerName()
	//				  << " to " << (bActive?"ACTIVE":"DISABLED") << std::endl;
		}

		bool PortComm::PortMap::once(flowvr::ModuleAPI  *m)
		{
			if(m_handler)
				return m_handler->once( *this, *m->getAllocator() );
			return false;
		}

		flowvr::portutils::PortHandlerBase *PortComm::PortMap::getHandler() const
		{
			return m_handler;
		}

		PortComm::PortComm( flowvr::ModuleAPI *module )
		: m_Module(module)
		, m_enabled(true)
		, m_defRet(RET_CLEAN)
		, m_inifiteWait(false)
		{

		}

		PortComm::~PortComm()
		{
			// ports can be assigned to any number of code-paths
			std::set<flowvr::Port*> setPorts;
			// collect each pointer (but only once)
			std::for_each( m_vCodeRange.begin(), m_vCodeRange.end(), _collectPorts( setPorts ) );

			// delete every pointer just once.
			std::for_each( setPorts.begin(), setPorts.end(), _deletePort() );
		}


		flowvr::ModuleAPI *PortComm::getModuleAPI() const
		{
			return m_Module;
		}

		void PortComm::setModuleAPI( flowvr::ModuleAPI *api )
		{
			m_Module = api;
		}



		void PortComm::setInfiniteWaitFlag()
		{
			std::cerr << "PortComm::setInifiteWaitFlag() on process [" << getpid() << "]-- ATTENTION" << std::endl;
			m_inifiteWait = true;
		}

		PortComm::eState PortComm::process()
		{
			// expert-debug api
			// used to have the module hang around here to catch it with a debugger
			// then set the m_inifiteWait back to false to leave this loop
			if(m_inifiteWait)
			{
				bool b = true;
				while(b)
				{
					flowvr::utils::microsleep(1000);
				}
			}


			if(!m_enabled)
				return m_defRet;

			if(m_Module == NULL)
				return RET_FAIL;


			// ####################################################################
			// update services PreLoop()
			// ####################################################################
			std::for_each( m_services.begin(), m_services.end(), _preLoop() );

			if( m_Module->wait() == false )
			{
				// error or quit!
				return RET_FAIL;
			}

			// this is the code-algo to be called for each code path
			_processMsg c2;

			int n = 0; // count the number of code path units that were executed for this module.
					   // in case of error, we can end up in a module that is not executing anymore,
					   // which results in idle-polling and wasting cpu time.


			// for each run, we prepare a cache map of messages queried from the input ports
			// in this run. We map the port-name to the message that was at the port at the
			// time of query. In case some ports are re-used over different code-paths, this
			// is the only way of ensuring that a plugin gets the message for this iteration.
			std::map<std::string, flowvr::Message> msgmap;

			// iterate over all code-paths for this module, one by one...
			for( std::vector<PortMap>::iterator it = m_vCodeRange.begin();
				 it != m_vCodeRange.end(); ++it)
			{
				// skip over code paths that are not active (i.e., in case of error)
				if((*it).getIsActive() == true)
				{
					// we pass a message map with all the information needed to the
					// code path, we assign the code path as parent and give the flowvr-module
					// as allocator (e.g., allocate memory as setup on a global level)
					PortComm::MsgMap mp( *it, *(*m_Module).getAllocator() );

					// scan all input ports for messages, according to port name determined by the ports
					// actively used in the code path (*it).m_InPorts
					for( std::vector<flowvr::Port*>::iterator cit = (*it).m_InPorts.begin(); cit != (*it).m_InPorts.end(); ++cit )
					{
						// messages are cached by using the port name,
						// we need it several times below, so we cache it here...
						if( (*cit) == NULL ) // can happen... misspelling of port
						{
							mp.inmsg.push_back( flowvr::Message() );
							mp.instamps.push_back( NULL ); // no stamps here!
							mp.instate.push_back(0); // equals 'not-connected'
							continue;
						}

						std::string portname = (*cit)->name;

						// look if we already got that message in this iteration
						std::map<std::string, flowvr::Message>::const_iterator tt = msgmap.find( portname );

						if( tt == msgmap.end() )
						{
							// no, cache the message
							flowvr::Message m; // empty
							flowvr::InputPort *inp = static_cast<flowvr::InputPort*>( (*cit) ); // get input port type
							(*cit)->getModule()->get( inp, m ); // get message from flowvr
							msgmap[ portname ] = m; // cache in map (using the portname)

							mp.inmsg.push_back(m); // give it to the code path
							mp.instamps.push_back( (*cit)->stamps ); // assign stamplist for this message as well
						}
						else
						{
							// ok, we have it, copy it over to the local msg vector (appending)
							mp.inmsg.push_back((*tt).second); // give it to the code path
							mp.instamps.push_back( (*cit)->stamps ); // assign stamplist for this message as well
						}

						// in either case, re-mark the connection state here
						// (is only valid after the first wait, so we query it here each iteration)
						mp.instate.push_back( (*cit)->isConnected() ? 1 : 0 );
					} // iterator over inports


					// ok, so far we have only setup the input message structure, we have to create some (empty)
					// buffers for these code paths that produce output as well..
					// in case the output did not declare to use any ports, this loop will not be executed ;)
					for( std::vector<flowvr::Port*>::iterator cit = (*it).m_OutPorts.begin(); cit != (*it).m_OutPorts.end(); ++cit )
					{
						mp.outmsg.push_back(flowvr::MessageWrite()); // give (empty) message write..
						if( (*cit) != NULL ) // can happen... misspelling of port...
							mp.outstamps.push_back( (*cit)->stamps ); // assign stamplist of the output port, in case the
																  // output code wants to modify the stamps to be sent
						else
							mp.outstamps.push_back( NULL );

						mp.outstate.push_back( (*cit)->isConnected() ? 1 : 0 );
					}


					// this is the call on the code path to execute...

					c2(mp); // this is a call to process the portmap
					++n; // count code path


					// we presumably produced output, so we put that to the ports that are given by the code path
					for( PortComm::MsgMap::OSLOT::iterator ot = mp.outmsg.begin(); ot != mp.outmsg.end(); ++ot )
					{
						// is the message valid?
						if( (*ot).data.valid() )
						{
							// no... so we send it.
							// get the index of the output port in the vector we are traversing (we need it to
							// get the right message write buffer)
							std::vector<flowvr::Port*>::size_type idx = std::distance( mp.outmsg.begin(), ot );
							if(idx < (*it).m_OutPorts.size() ) // sanity check
							{
								flowvr::OutputPort *out = static_cast<flowvr::OutputPort*>( (*it).m_OutPorts[ idx ] );
								if(out) // port found
								{
									m_Module->put( out, (*ot) ); // write to output
								}
							}
							else
							{
								std::cerr << "output port not found (port vector too short)" << std::endl;
							}
						}
					}

					if( (*it).getHandler() )
						(*it).getHandler()->postPutActivation();
				}
			}

			// ########################################################################################
			// POST-LOOP UPDATE ON SERVICES NOW
			// ########################################################################################

			std::for_each( m_services.begin(), m_services.end(), _postLoop() );

			// test if we 'nicen' the module
			// note that a module will iterate really fast if
			// - there is no code path left to execute
			// - there are no connected inports that we will wait for
			// note that on the contrary, a module with a code path and
			// - no input connection
			// - only none-blocking inputs
			// should iterate as fast as possible, as we might have a no-message handler code-path
			// to update, as well as services and other stuff
			if(n == 0)
			{
				// hmm... practically, there is no code path left...
				// that means that this module will go idle polling...
				// we gracefully sleep for some time, so we can decide to exit
				// once the module->wait() will fail

				// first check if this module is bound by input
				if( !m_Module->isBoundByInports() and m_Module->getNumberOfInputPorts() > 1 ) // we have user-defined ports (more than
																							  // just beginIt but those are not bound)
					flowvr::utils::microsleep(1000); // no, this module will go nuts... so be nice here..
				// else: yes, we wait for incoming messages and take them off the bus
			}


			return (c2.m_dirty ? RET_DIRTY : RET_CLEAN); // determine dirty state
		}

		void PortComm::setCodeRange( const std::vector<PortMap> &vCodeRange )
		{
			m_vCodeRange = vCodeRange;
		}

		void PortComm::setServices( const std::vector<IServiceLayer*> &veServices )
		{
			m_services = veServices;
		}

		bool PortComm::getEnabled() const
		{
			return m_enabled;
		}

		void PortComm::setEnabled( bool bEnabled )
		{
			m_enabled = bEnabled;
		}

		PortComm::eState PortComm::getDefaultReturn() const
		{
			return m_defRet;
		}

		void PortComm::setDefaultReturn( PortComm::eState  est)
		{
			m_defRet = est;
		}
	} // namespace portutils
} // namespace flowvr
