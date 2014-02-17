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

#ifndef PORTUTILSPORTHANDLER_H_
#define PORTUTILSPORTHANDLER_H_

#include <vector>
#include <string>
#include <flowvr/portutils/portcomm.h>
#include <flowvr/portutils/portparameter.h>

namespace flowvr
{
	class Port;
	class InputPort;
	class OutputPort;
	class ModuleAPI;
}

namespace flowvr
{
	namespace portutils
	{
		/**
		 * @brief Baseclass for main interfaces that do the processing of messages.
		 */
		class PortHandlerBase
		{
		public:
			/**
			 * @enum PortHandlerBase::eState
			 * @brief return values for handleMessages()
			 *
			 * Depending on the context, the implementation of a handler can decide to return the following
			 * values. The PortComm decides to halt or remove a handler depending on the return values.
			 */
			enum eState
			{
				E_OK = 0, /**< Everything went fine... */
				E_REMOVE, /**< Everything went fine, or not... please remove the handler from the update */
				E_ERROR   /**< Something went definitely wrong, remove the module */
			};

			virtual ~PortHandlerBase();

			/**
			 * @brief Main entry point for update dispatching.
			 * The handleMessages() method is the sole and abstract entry point for message handling
			 * in the PortHandlerBase. Normally, there is no need to re-define it on user lever,
			 * other than having very special requirements on the message processing, like access to
			 * the flowvr module, ports or the like.
			 *
			 * Subclasses of this PortHandlerBase define the method and offer alternatives for simple usage.
			 *
			 * @return eState see description of eState
			 * @param mp the PortComm::MsgMap to process.
			 */
			virtual eState handleMessages( PortComm::MsgMap &mp ) = 0;

			/**
			 * Return the user defined name for this handler. Should be unique during the configuration
			 * of a module.
			 * @return the name of this handler, possibly empty
			 */
			std::string getHandlerName() const;

			/**
			 * Set the name for this handler. The name is just a symbol for lookup and user-defined.
			 * The PortHandlerBase does not assume any other semantics with the name.
			 * @param name the name that can be accessed using getHandlerName()
			 */
			void setHandlerName( const std::string &name );

			virtual void resetArgs( const ARGS & )  {}

			/**
			 * @brief is called <i>once</i> before the first loop of the application
			 *        but <i>after</i> everything is initialized.
			 *
			 * This method does not pass environmental information, so it can be used for
			 * any state-related settings.
			 */
			virtual void preLoopActivation() {}

			virtual void postPutActivation() {}

			/**
			 * @brief is called <i>once</i> during the call to module::init(),
			 *        is called before preLoopActivation. The routine passes
			 *        environmental information (portmap and allocator) so it
			 *        can be used for example to send 'once' messages
			 *
			 *  @param portMap the portmap of this module. Can be used to search
			 *         ports, stamps and the like.
			 *  @param alloc the allocator to use to reserve memory for messages.
			 */
			virtual bool once(PortComm::PortMap &portMap, flowvr::Allocator &alloc) { return true; }

			const std::vector<std::string> &getInPortNames() const;
			void setInPortNames( const std::vector<std::string> & );

			const std::vector<std::string> &getOutPortNames() const;
			void setOutPortNames( const std::vector<std::string> & );
		protected:
			void fillMsgMapWrite( PortComm::PortMap &, PortComm::MsgMap & ) const;

		private:
			std::string m_strHandlerName;

			std::vector<std::string> m_vInPortNames,
									 m_vOutPortNames;
		};


		/**
		 * @brief A base class for handlers that serve as data source.
		 *
		 * A source handler has only output, e.g. <i>only</i> write something to an out port, or to be
		 * more precise, only work on <i>one</i> flowvr::MessageWrite.
		 */
		class SourcePortHandler : public PortHandlerBase
		{
		public:
			/**
			 * @brief selection method to retrieve a MessageWrite from the MsgMap.
			 *
			 * selects the <i>first</i> MessageWrite from the PortComm::MsgMap and
			 * passes that to handleMessage(), along with the allocator given in the PortComm::MsgMap.
			 *
			 * @return the return value of handleMessage()
			 * @param mp the PortComm::MsgMap to process
			 */
			virtual eState handleMessages( PortComm::MsgMap &mp );

			/**
			 * @brief actual handling method.
			 *
			 * This method has to be defined by the subclass.
			 *
			 * @param mw the MessageWrite object to fill with data
			 * @param sl the stamplist that can be used to query stamps from the Output port to send the message to
			 * @param alloc the allocator to use to obtain memory for the message
			 * @return see PortHandlerBass::eState for a description, return what is appropriate for you.
			 */
			virtual eState handleMessage( flowvr::MessageWrite &mw, flowvr::StampList *sl, flowvr::Allocator &alloc ) = 0;

			virtual bool once(PortComm::PortMap &, flowvr::Allocator &alloc);
			virtual bool once(flowvr::ModuleAPI &, flowvr::MessageWrite &mw, flowvr::StampList *sl, flowvr::Allocator &alloc )  { return true; }
		};

		/**
		 * @brief A NoPortHandler works just on its own, no input and no output ports.
		 *
		 * Can be used to execute functions on the context without messages to work on.
		 * Subclasses have to implement the handleNoMessage() method.
		 */
		class NoPortHandler : public PortHandlerBase
		{
		public:
			/**
			 * @brief Macro method to call handleNoMessage() by masking out all arguments.
			 *
			 * @return the value of handleNoMessage()
			 * @param mp the PortComm::MsgMap to work on
			 */
			virtual eState handleMessages( PortComm::MsgMap &mp );
			virtual eState handleNoMessage() = 0;
		};

		/**
		 * @brief A base class for handlers that consume input on one port
		 *
		 * A sink for data is a port that accepts input, but does not produce output.
		 * Subclasses have to implement the handleMessage() method.
		 */
		class SinkPortHandler : public PortHandlerBase
		{
		public:
			/**
			 * @brief Macro function to call handlMessage() on the <i>first</i> message in the PortComm::MsgMap
			 *        input message vector.
			 *
			 * @return the value of handleMessage()
			 */
			virtual eState handleMessages( PortComm::MsgMap &mp );

			/**
			 * @brief the actual method that has to be implemented by subclasses
			 *
			 * Handle a message coming in from the flowvr network. Do not produce any output.
			 *
			 * @param m the message to consume (read-only)
			 * @param sl the StampList assigned to the message port where m comes from
			 *
			 * @return see eState documentation.
			 */
			virtual eState handleMessage( const flowvr::Message &m, const flowvr::StampList *sl ) = 0;
		};


		/**
		 * @brief Simplification for one-input-one-output processing.
		 *
		 * The PipeHandler is activate by the arrival of one message, it calculates
		 * and then produces one output on the given MessageWrite.
		 * Subclasses define the handleMessagePipe() method.
		 */
		class PipeHandler : public PortHandlerBase
		{
		public:
			/**
			 * @brief macro method to mask the <i>first</i> message on the input ports and the <i>first</i> MessageWrite
			 *        on the output ports of the MsgMap given.
			 *
			 * Calls handleMessagePipe() with the first messages given on the input and output vectors.
			 *
			 * @param mp the PortComm::MsgMap to process.
			 */
			virtual eState handleMessages( PortComm::MsgMap &mp );

			/**
			 * @brief the actual workload method to implement by subclassing.
			 *
			 * The pipe model processes one message on the input side to produce a new message on the output
			 * side. The input and output side can have different stamp lists.
			 *
			 * @param m_in the input to process (read-only)
			 * @param sl_in the StampList for the input message
			 * @param m_out the output message to fill with content
			 * @param sl_out the StampList to use for the output message
			 * @param alloc the allocator to use when filling m_out with content
			 *
			 * @return see documentation of sState
			 */
			virtual eState handleMessagePipe( const flowvr::Message &m_in,
											  const flowvr::StampList *sl_in,
											  flowvr::MessageWrite &m_out,
											  flowvr::StampList *sl_out,
											  flowvr::Allocator &alloc) = 0;

			virtual bool once( PortComm::PortMap &mp, flowvr::Allocator &alloc );
			virtual bool once( flowvr::ModuleAPI &, flowvr::MessageWrite &m_out, flowvr::StampList *sl_out, flowvr::Allocator &alloc ) { return true; }
		};

		/**
		 * @brief the most general case type input and output handler
		 *
		 * The MultiplexHandler takes n input messages and produced m output messages.
		 * Subclasses define handleMultiplexing().
		 *
		 */
		class MultiplexHandler : public PortHandlerBase
		{
		public:
			/**
			 * @brief macro like forwarder for the information given in the PortComm::MsgMap.
			 *
			 * @return the output of handleMultiplexing.
			 */
			virtual eState handleMessages( PortComm::MsgMap &mp );

			/**
			 * @brief the multiplexing function
			 *
			 * @param m_in the slot vector for input processing. Note that there can be entries in that
			 *        vector which are assigned to NULL (it's the handler that defines the number of slots
			 *        that are expected)
			 * @param sl_in the StampLists to be used for the input messages. There is a bijection between the
			 *        index in the sl_in and m_in (index-wise match)
			 * @param m_out the outslots that have to be filled by this handler. All entries in the vector
			 *        are assigned with empty message writes. It is this handler that defines the semantics
			 *        of each slot.
			 * @param sl_out the output slots used for the output messages. The stamps are mapped index-wise
			 *        to the entries in the m_out vector.
			 * @param alloc the allocator to use when calling for memory on the output messages.
			 */
			virtual eState handleMultiplexing( const PortComm::MsgMap::SLOTS &m_in,
											   const PortComm::MsgMap::SLOTSTAMPS  &sl_in,
											   PortComm::MsgMap::OSLOT  &m_out,
											   PortComm::MsgMap::SLOTSTAMPS &sl_out,
											   flowvr::Allocator & alloc) = 0;

			virtual bool once( PortComm::PortMap &, flowvr::Allocator &alloc );
			virtual bool once( PortComm::MsgMap::OSLOT  &m_out, PortComm::MsgMap::SLOTSTAMPS &sl_out, flowvr::Allocator & alloc) { return true; }
		};
	} // namespace portutils
} // namespace flowvr

#endif /* PORTUTILSPORTHANDLER_H_ */
