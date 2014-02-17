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


#include <flowvr/portutils/porthandler.h>

#include <flowvr/moduleapi.h>
#include <iostream>

namespace flowvr
{
	namespace portutils
	{
		PortHandlerBase::~PortHandlerBase()
		{
	//		std::cout << "PortHandlerBase::~PortHandlerBase(" << getHandlerName() << ")" << std::endl;
		}

		std::string PortHandlerBase::getHandlerName() const
		{
			return m_strHandlerName;
		}

		void PortHandlerBase::setHandlerName( const std::string &name )
		{
			m_strHandlerName = name;
		}


		const std::vector<std::string> &PortHandlerBase::getInPortNames() const
		{
			return m_vInPortNames;
		}

		void PortHandlerBase::setInPortNames( const std::vector<std::string> &inNames )
		{
			m_vInPortNames = inNames;
		}

		const std::vector<std::string> &PortHandlerBase::getOutPortNames() const
		{
			return m_vOutPortNames;
		}

		void PortHandlerBase::setOutPortNames( const std::vector<std::string> &outNames )
		{
			m_vOutPortNames = outNames;
		}


		void PortHandlerBase::fillMsgMapWrite( PortComm::PortMap &mp, PortComm::MsgMap &mm ) const
		{
			for( std::vector<flowvr::Port*>::iterator cit = mp.m_OutPorts.begin(); cit != mp.m_OutPorts.end(); ++cit )
			{
				mm.outmsg.push_back(flowvr::MessageWrite()); // give (empty) message write..
				if( (*cit) != NULL ) // can happen... misspelling of port...
					mm.outstamps.push_back( (*cit)->stamps ); // assign stamplist of the output port, in case the
														  // output code wants to modify the stamps to be sent
				else
					mm.outstamps.push_back( NULL );
			}
		}
		// ##################################################################################
		// SourcePortHandler
		// ##################################################################################


		PortHandlerBase::eState SourcePortHandler::handleMessages( PortComm::MsgMap &mp )
		{
			if( mp.outmsg.empty() )
				return E_ERROR;

			return handleMessage( mp.outmsg[0], mp.outstamps[0], mp.m_allocator );
		}


		bool SourcePortHandler::once(PortComm::PortMap &mp, flowvr::Allocator &alloc)
		{
			PortComm::MsgMap mm(mp,alloc);
			fillMsgMapWrite(mp,mm);
			if(once( *mp.m_OutPorts[0]->getModule(), mm.outmsg[0], mm.outstamps[0], mm.m_allocator ))
			{
				if( mm.outmsg[0].data.valid() )
					mp.m_OutPorts[0]->getModule()->put(dynamic_cast<flowvr::OutputPort*>(mp.m_OutPorts[0]), mm.outmsg[0] );
				return true;
			}
			return false;
		}

		// ##################################################################################
		// NoPortHandler
		// ##################################################################################
		PortHandlerBase::eState NoPortHandler::handleMessages( PortComm::MsgMap &mp )
		{
			return handleNoMessage();
		}
		// ##################################################################################
		// SinkPortHandler
		// ##################################################################################

		PortHandlerBase::eState SinkPortHandler::handleMessages( PortComm::MsgMap &mp )
		{
			if( mp.inmsg.empty() )
				return E_ERROR;

			flowvr::Message &m = mp.inmsg[0];
			if(m.valid())
				return handleMessage( mp.inmsg[0], mp.instamps[0] );
			else
				return E_OK; /// @todo is return E_OK on invalid msg ok? (for non-blocking... well... yes...)
		}

		// ##################################################################################
		// PipeHandler
		// ##################################################################################


		PortHandlerBase::eState PipeHandler::handleMessages( PortComm::MsgMap &mp )
		{
			if( mp.inmsg.empty() )
				return E_ERROR;

			flowvr::Message &m = mp.inmsg[0];
			if(m.valid())
				return handleMessagePipe( mp.inmsg[0], mp.instamps[0], mp.outmsg[0], mp.outstamps[0], mp.m_allocator );
			else
			{
	//			std::cerr << "PipeHandler::handleMessages() -- invalid message received." << std::endl;
				return E_OK; /// @todo is return E_OK on invalid msg ok? (for non-blocking... well... yes... and for unconnected blockings as well...)
			}
		}

		bool PipeHandler::once(PortComm::PortMap &mp, flowvr::Allocator &alloc)
		{
			PortComm::MsgMap mm(mp,alloc);
			fillMsgMapWrite(mp,mm);
			if( once( *mp.m_OutPorts[0]->getModule(), mm.outmsg[0], mm.outstamps[0], mm.m_allocator ) )
			{
				if( mm.outmsg[0].data.valid() ) // we watch for full messages here (after all its a module)
					mp.m_OutPorts[0]->getModule()->put(dynamic_cast<flowvr::OutputPort*>(mp.m_OutPorts[0]), mm.outmsg[0] );
				return true;
			}
			return false;
		}

		// ##################################################################################
		// MULTIPLEX HANDLER
		// ##################################################################################


		PortHandlerBase::eState MultiplexHandler::handleMessages( PortComm::MsgMap &mp )
		{
			// prepare messages and pass to multiple handle
			return handleMultiplexing( mp.inmsg, mp.instamps, mp.outmsg, mp.outstamps, mp.m_allocator );
		}

		bool MultiplexHandler::once( PortComm::PortMap &mp, flowvr::Allocator &alloc )
		{
			PortComm::MsgMap mm(mp,alloc);
			fillMsgMapWrite(mp,mm);
			if(once( mm.outmsg, mm.outstamps, mm.m_allocator ))
			{
				for( PortComm::MsgMap::OSLOT::iterator it = mm.outmsg.begin(); it != mm.outmsg.end(); ++it )
				{
					if( (*it).data.valid() )
					{
						flowvr::OutputPort *out = dynamic_cast<flowvr::OutputPort*>(mp.m_OutPorts[ it - mm.outmsg.begin() ]);
						out->getModule()->put( out, *it );
					}
				}
			}
		}
	} // namespace portutils
} // namespace flowvr
