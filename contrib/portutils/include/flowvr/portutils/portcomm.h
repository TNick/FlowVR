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

#ifndef PORTCOMM_H_
#define PORTCOMM_H_

#include <vector>
#include <map>

#include <flowvr/message.h>

namespace flowvr
{
	class Port;
	class ModuleAPI;

	namespace portutils
	{
		class PortHandlerBase;
		class IServiceLayer;

		/**
		 * @brief the PortComm class defines the event handling state machine of the binaries using PortUtils.
		 *
		 * PortComm stands for <i>PortComm</i>unicator and it defined structures for passing arguments to
		 * handlers as well as the state machine that is executed during each flowvr iteration.
		 * Currently, there is one flowvr-module per PortComm.
		 *
		 * The PortComm can be disabled ( setEnabled() ). In case it is disabled, the call to process() will
		 * return immediately with the default return state ( setDefaultReturn() ).
		 *
		 */
		class PortComm
		{
		public:
			/**
			 * @brief A code path descriptor to execute for a PortComm instance.
			 *
			 * The PortMap descriptor takes a number of flowvr InputPorts and OutputPorts
			 * as well as a handler that will
			 * -# need messages from all the inports in the inport vector
			 * -# process on the data and then
			 * -# output a single message for every port in the outport vector.
			 *
			 * The PortMap structure is a mostly read-only structure. Handlers should not modify it
			 * during processing, but the vectors for in and output are given as public members for
			 * the sake of simplicity.
			 * A code path can be set to inactive ( setIsActive() ).
			 */
			class PortMap
			{
			public:
				/**
				 * Constructor.
				 * @param veInPorts copied to m_InPorts
				 * @param veOutPorts copied to m_OutPorts
				 * @param pHandler the pointer to the handler that executed this code path
				 * @param bActive the initial activity state.
				 *
				 * Note that there is currently no automatic way to switch an inactive PortMap
				 * instance to an active one, but only the other way around: the PortHandlerBase
				 * can run into an error, which causes the PortComm to inactivate the PortMap during
				 * Processing. The activation flag is typically set to false when the user misconfigured
				 * the PortMap (for example by passing non-existent port names).
				 */
				PortMap( const std::vector<flowvr::Port*> &veInPorts,
						 const std::vector<flowvr::Port*> &veOutPorts,
						 PortHandlerBase *pHandler,
						 bool bActive )
				: m_InPorts(veInPorts)
				, m_OutPorts(veOutPorts)
				, m_handler(pHandler)
				, m_bActive(bActive)
				{

				}

				/**
				 * @return the activation state of this PortMap
				 *        - true in case the handler will be updated during PortComm::process()
				 *        - false else
				 */
				bool getIsActive() const;

				/**
				 * @param bActive the activation state of this PortMap. Set it to
				 *        - true in case the PortMap is executed during PortComm::process()
				 *        - false else
				 */
				void setIsActive( bool bActive );

				/**
				 * Retrieve the PortHandlerBase that will process on the inports of this PortMap to
				 * create output on the outports attached to this PortMap.
				 * @return the PortHandlerBase or NULL in case non is attached (possibly due to misconfiguration)
				 */
				PortHandlerBase *getHandler() const;

				bool once(flowvr::ModuleAPI *);



				std::vector<flowvr::Port*> m_InPorts, /**< the inport range available for this PortMap.
														   Note that it can contain NULL as a pointer. */
										   m_OutPorts; /**< the outport range available for this PortMap,
															all Ports in this vector will be used for putting
															a message to. Can contain NULL as well */

			protected:
				PortMap() {}


			private:
				PortHandlerBase *m_handler; /** the handler for this PortMap */
				bool m_bActive; /** the activation state */
			};


			/**
			 * The simplified handling structure for PortHandlerBase instances.
			 * Workign on the PortMap forces the user to get message from and to Ports, which is normally
			 * always the same code and cumbersome to find the correct Ports in the vectors.
			 * The MsgMap is an abstraction, as it contains simply the messages given in SLOTS where each
			 * slot is defined by the index of the InPort or OutPort in the PortMap. In case a Port was set to
			 * NULL in the PortMap, the slot in the MsgMap contains an invalid (empty) message.
			 * For output, the MsgMap comprises slots of MessageWrite objects. The user has to call
			 * 'allocate' on the buffer to claim memory during the output processing (the MessageWrite objects are
			 * not initialized to claim memory).
			 */
			class MsgMap
			{
			public:
				typedef std::vector<flowvr::Message>      SLOTS;
				typedef std::vector<flowvr::StampList*>   SLOTSTAMPS;
				typedef std::vector<flowvr::MessageWrite> OSLOT;
				typedef std::vector<int> SLOTSTATE;

				SLOTS  inmsg;
				SLOTSTATE instate;
				OSLOT  outmsg;
				SLOTSTATE outstate;
				SLOTSTAMPS instamps,
						   outstamps;

				MsgMap(PortMap &parent, flowvr::Allocator &alloc)
				: inmsg()
				, instate()
				, outmsg()
				, outstate()
				, m_parent(parent)
				, m_allocator(alloc)
				{}

				/**
				 * link to parent PortMap, in case you need to access ports directly.
				 * Use with care.
				 */
				PortMap &m_parent;

				/**
				 * the allocator to use for memory allocation during the production of output.
				 */
				flowvr::Allocator &m_allocator;
			};

			/**
			 * The constructor sets this PortComm to
			 * - enabled
			 * - the default return is RET_CLEAN
			 * - the PortComm will not wait infinite
			 * - the pointer to the flowvr module is copied
			 *
			 * @param pModule a pointer to the flowvr module to bind this PortComm to. Can be NULL,
			 *        but in that case pass a module using setModuleAPI() prior to a call to process()
			 *
			 * @see setModuleAPI()
			 * @see process()
			 */
			PortComm( flowvr::ModuleAPI  *pModule );

			/**
			 * Currently does nothing.
			 * User code should not forget to clean up all the ports on the flowvr side that were allocated.
			 */
			~PortComm();


			/**
			 * Possible return states of process()
			 * @todo check the concept of dirty returns, as it is probably legacy
			 */
			enum eState
			{
				RET_FAIL=-1, /**< some severe error happened, you might try again to call process(),
								  but the chances are high, it will fail again. */
				RET_DIRTY,   /**< normal processing, there were messages dispatched */
				RET_CLEAN    /**< all handlers are 'clean' for this iteration (all message consumed) */
			};

			/**
			 * @brief Iterate on flowvr loop for this port comm
			 *
			 * A call to process() will
			 * -# call the IServiceLayer::PreLoop() for all services registered (and active)
			 * -# call moduleAPI::wait() to receive new messages from flowvr.
			 * -# iterate over ever code path registered (as Type PortMap) and
			 * -# call PortHandlerBase::handleMessages() by passing a message map that is fetched from the flowvr module
			 * -# in case a handler produces a message in return, all messages are put to their corresponding ports after the handler executed
			 * -# call the IServiceLayer::PostLoop() for all services registered (and active)
			 *
			 * In case there is no active handler after the first call to moduleAPI::wait(), the PortComm will
			 * - see if any ports are bound to receive messages
			 * - if this is not the case, it will wait for a second before entering module::wait() again.
			 *   This is a measure to avoid a module that eats all CPU on idle polling because there is nothing to do
			 *   and no message to consume.
			 * - if the module is bound, the PortComm will enter the moduleAPI::wait() immediately to eat incoming messages
			 *   and avoid a long queue and memory shortage.
			 *
			 *  The PortComm takes only those messages from the moduleAPI that have a handler attached, thus impairing
			 *  the overhead of taking a message just for those. However,all handlers working on the same input port
			 *  will get the same message acquired only once from this PortComm, so they work on coherent data.
			 *
			 * @see getEnabled()
			 * @see setEnabled()
			 * @see getDefaultReturn()
			 * @see setDefaultReturn()
			 *
			 * @return the state of this PortComm. In case this is RET_FAIL, then
			 *         - either moduleAPI::wait() failed (flowvr end signal)
			 *         - you set no module using the constructor or setModuleAPI()
			 *         - it is not enabled and you set the default return to be RET_FAIL
			 */
			eState process();

			/**
			 * @brief Retrieve the flowvr moduleAPI that is bound to this PortComm.
			 *
			 * @return the pointer to the module or NULL in case the PortComm is not bound.
			 */
			flowvr::ModuleAPI *getModuleAPI() const;

			/**
			 * Set the flowvr module to use during processing.
			 * Note that this API is not meant to be used frequently and it is not considered thread safe.
			 * Nothing will happen to the old module if it was set.
			 * @param api the pointer to the flowvr module to use, can be NULL.
			 *        (The next call to process() will return RET_FAIL then)
			 */
			void setModuleAPI( flowvr::ModuleAPI *api );


			/**
			 * Retrieve the enabled state of this PortComm.
			 * An enabled PortComm will process messages from flowvr during a call to process().
			 * In case it is disabled, the call to process() will return the default return value.
			 *
			 * @see setDefaultReturn()
			 * @see process()
			 */
			bool getEnabled() const;

			/**
			 * Set the enabled state of this PortComm.
			 * @param bEnabled
			 *        - true in case this PortComm is dispatching messages during a call to process(),
			 *        - false else.
			 *
			 * @see getEnabled()
			 * @see process()
			 */
			void setEnabled( bool bEnabled );


			/**
			 * Retrieve the default return state of this PortComm.
			 * The default return is given in case this PortComm is disabled and the user called process()
			 *
			 * @see setDefaultReturn()
			 * @see process()
			 * @see setEnabled()
			 * @see getEnabled()
			 */
			eState getDefaultReturn() const;

			/**
			 * Set the state that is returned when this PortComm is disabled.
			 * @see getDefaultReturn()
			 * @see process()
			 * @see setEnabled()
			 * @see getEnabled()
			 *
			 * @param est the state to return when this PortComm is disabled and the user calls process()
			 */
			void setDefaultReturn( eState est );


			// ####################################################################
			// EXPERT DEVELOPER API
			// ####################################################################

			/**
			 * Set the vector of code ranges to process during each call to process().
			 * The PortMaps will be traversed exactly in the order given.
			 * This API needs only be called once, though you could call it more. It will be
			 * re-evaluated upon every call to process().
			 * @param vCodeRange the vector of PortMap structures to traverse during process.
			 */
			void setCodeRange( const std::vector<PortMap> &vCodeRange );

			/**
			 * Set the range of services that are used for update before and after the processing of messages.
			 * The service layers will be updated exactly in the order as given in this vector.
			 * The service layers should outlive the PortComm. There is no need to call this method more
			 * than once, though you could, as the vector is re-evaluated every call to process()
			 */
			void setServices( const std::vector<IServiceLayer*> &vServices );

			/**
			 * Expert debug API, do not use it unless you know what you are doing.
			 * Ok... this is what it does: in case you call this API, the PortComm will enter an inifinite loop
			 * prior to the first call to moduleAPI::wait(). There is only one way to escape that loop: compile
			 * the PortUtils in debug mode, attach a debugger and set the while-loop condition to false and step on.
			 * The idea of this method is to have some time to attach a debugger before the call to wait.
			 * In case you set the InfiniteWait flag to true, the PortComm will utter a message to std out, identifying
			 * the PID of this module.
			 * @warning do not call it without a debugger at hand.
			 */
			void setInfiniteWaitFlag();

		private:
			flowvr::ModuleAPI           *m_Module; /**< the module to work on, pointer must outlive the PortComm */
			std::vector<PortMap>         m_vCodeRange; /**< the code range to work on */
			std::vector<IServiceLayer *> m_services; /**< services to update before and after the processing */

			bool m_enabled, /**< enabled state of the PortComm */
				 m_inifiteWait; /**< expert debug infinite wait flag */
			eState m_defRet; /**< the default return value in case this module is set to disabled */
		};
	} // namespace portutils
} // namespace flowvr

#endif /* PORTCOMM_H_ */
