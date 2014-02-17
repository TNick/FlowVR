/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                   Balzac FlowVR render frontend                 *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA.  ALL RIGHTS RESERVED.                                    *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING file for further information.                           *
*                                                                 *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: filter.plugin.FilterMultiplexMerge.cpp                    *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include "flowvr/daemon.h"
#include "flowvr/plugd/genclass.h"
#include "flowvr/plugins/filter.h"
#include <iostream>
#include <sstream>
#include <unistd.h>


using namespace flowvr::plugd;
using namespace flowvr::plugins;
using namespace flowvr::xml;

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			class MultiplexMerge : public Filter
			{
			public:
				MultiplexMerge( const std::string &strId );
				virtual Class* getClass() const;

				virtual Result init(DOMElement* xmlRoot,
									Dispatcher* dispatcher);

				virtual void newMessageNotification(int mqid,
												  int msgnum,
												  const Message& msg,
												  Dispatcher* dispatcher);
				virtual void newStampListSpecification(int mqid,
													 const Message& msg,
													 Dispatcher* dispatcher);

				enum
				{
					IDPORT_ORDER=0, /**< the STAMP message queue to specify the number of messages to forward */
					IDPORT_IN0       /**< the FULL message queue to store incoming messages, first index ( IN->0 ! )  */
				};

			private:
				virtual void handleNewMessage(int mqid,
											  int msgnum,
											  const Message& msg,
											  Dispatcher* dispatcher);

				virtual void handleNewTrigger(int mqid,
											  int msgnum,
											  Dispatcher* dispatcher);

				void purgeQueue( int qid );

				/////////////////////////////////////////////////////////////////////
				// ATTRIBUTES
				/////////////////////////////////////////////////////////////////////
				unsigned int m_nNbInports, m_nNum;
				bool         m_bSampleTrigger, /**< purge trigger queue after each successful delivering of messages */
				             m_bStampsForwarded, /**< state note: stamps processed? */
				             m_bPoolTrigger, /**< keep only ONE trigger in the queue if there are no new messages */
				             m_bUseSegmentedOutput; /**< instead of merge by copy, use segmented output on the buffers */

			};


			///////////////////////////////////////////////////////////////////
			// IMPLEMENTATION BLOCK
			///////////////////////////////////////////////////////////////////

			MultiplexMerge::MultiplexMerge( const std::string &strId )
			: Filter( strId )
			, m_nNbInports(0)
			, m_bSampleTrigger(false)
			, m_nNum(0)
			, m_bStampsForwarded(false)
			, m_bPoolTrigger(true)
			, m_bUseSegmentedOutput(false)
			{

			}

			Result MultiplexMerge::init(DOMElement* xmlRoot,
								Dispatcher* dispatcher)
			{
				Result result = Filter::init(xmlRoot, dispatcher);
				if (result.error())
					return result; // something already failed for the basic init, so do not continue

				xml::DOMNodeList* node;
				std::string nb;
				node = xmlRoot->getElementsByTagName("nb"); // find attribute set by user
				if (node->getLength() > 0)
				{
					nb = node->item(0)->getTextContent();
					m_nNbInports = atoi(nb.c_str()); // convert to number

					if(m_nNbInports <= 1) // we check the queue length directly here
					{
						delete node; // free memory
						return Result(Result::ERROR, "m_nNbInports must be >= 2");
					}
				}
				delete node;

				node = xmlRoot->getElementsByTagName("sampleOrder"); // find attribute set by user
				if (node->getLength() > 0)
				{
					nb = node->item(0)->getTextContent();
					if( nb == "true" )
						m_bSampleTrigger = true;
					else
						m_bSampleTrigger = false;
				}
				delete node;

				node = xmlRoot->getElementsByTagName("poolOrder");
				if (node->getLength() > 0)
				{
					nb = node->item(0)->getTextContent();
					if( nb == "true" )
						m_bPoolTrigger = true;
					else
						m_bPoolTrigger = false;
				}

				delete node; // get rid of the memory


				node = xmlRoot->getElementsByTagName("useSegments");
				if( node->getLength() > 0 )
				{
					nb = node->item(0)->getTextContent();
					if( nb == "true" )
						m_bUseSegmentedOutput = true;
					else
						m_bUseSegmentedOutput = false;

//					std::cout << "segmented merge: " << m_bUseSegmentedOutput << std::endl;
				}

				initInputs(m_nNbInports+1); // +1 for trigger at [0]
				char buf[16];
				for( int n=0; n < m_nNbInports; ++n)
				{
					sprintf(buf, "%d", n);
					inputs[IDPORT_IN0+n]->setName("in"+std::string(buf));
				}

				inputs[IDPORT_ORDER]->setName("trigger");

				initOutputs(1);
				outputs[0]->setName("out");
				outputs[0]->msgtype = Message::FULL; // we output full messages,
				                                     // as this is more general than
				                                     // just stamp messages (we can route to full message
													 // ports as well at to stamp ports)
													 // the 'real' value is re-set upon the receive of the first stamps

				return result; // re-use the result object here
			}

			void MultiplexMerge::newMessageNotification(int mqid,
											  int msgnum,
											  const Message& msg,
											  Dispatcher* dispatcher)
			{
				switch(mqid)
				{
					case IDPORT_ORDER:
					{
						// new request for old messages
						handleNewTrigger(mqid,msgnum,dispatcher);
						break;
					}
					default:
					{
						handleNewMessage(mqid,msgnum,msg,dispatcher);
						break;
					}
				}
			}

			void MultiplexMerge::newStampListSpecification(int mqid,
												 const Message& msg,
												 Dispatcher* dispatcher)
			{
				if (!m_bStampsForwarded && mqid >= IDPORT_IN0) // take the first you get
				{
					//forwarding stamplist specification to the OMQ
					outputs[0]->stamps = inputs[mqid]->getStampList();
				    outputs[0]->msgtype = (msg.data.valid() ? Message::FULL : Message::STAMPS);
					outputs[0]->newStampSpecification(dispatcher);

					m_bStampsForwarded = true;
				}
			}

			////////////////////////////////////////////////////////////////////
			// WORKLOAD METHODS
			////////////////////////////////////////////////////////////////////

			void MultiplexMerge::purgeQueue( int qid )
			{
				// pop off the front message until the queue is empty
				// we expect the queue to be locked all the time here
			    while( !inputs[qid]->empty() )
			    	inputs[qid]->eraseFront();
			}

			void MultiplexMerge::handleNewMessage(int mqid,
										  int msgnum,
										  const Message& msg,
										  Dispatcher* dispatcher)
			{
				// send only on request
				if( !inputs[IDPORT_ORDER]->empty() ) // check simply by seeing if the queue is not empty
				{
					// ok, there is at least one request
					handleNewTrigger( IDPORT_ORDER, inputs[IDPORT_ORDER]->frontNum(), dispatcher );
				}
				// else: we leave it in its queue, no action!
			}

			void MultiplexMerge::handleNewTrigger(int mqid,
										  int msgnum,
										  Dispatcher* dispatcher)
			{
				// iterate over all inputs and collect all messages which are thereafter merged
				// @remark I use a side-effect here that the incoming queue is locked while
				//         we are working on the trigger queue, this does not have to stay that
				//         way, so take care at the day you are going to change that.
				std::vector<Message> msgs;
				size_t size = 0;
				int nPipeCnt = 0;
				for( int n=IDPORT_IN0; n < m_nNbInports+1; ++n )
				{
					int nCount   = inputs[n]->size();
					int num0     = inputs[n]->frontNum();

					if( nCount )
						++nPipeCnt;

					for( int k = 0; k < nCount; ++k )
					{
						// get messages in correct order (oldest first)
						int idx = num0 + k;
						Message m = inputs[n]->get( idx );
						if( m.valid() && m.data.getSize() > 0 )
						{
							size += m.data.getSize();
							msgs.push_back( m );
						}
					}
					purgeQueue( n );
				}

				if( msgs.empty() )
				{
					// no messages to deliver, yet
					if(m_bSampleTrigger)
					{
						// anyhow: use demanded to 'sample' all triggers, which means to purge all triggers in the queue
						// note that this method may be called on behalf of a trigger OR on behalf of a new message,
						// so to go safe, we purge the whole queue here.
						purgeQueue(IDPORT_ORDER);
					}
					else if(m_bPoolTrigger) // ok, we were not told to purge 'em, but should we keep just 1?
					{
						// yes...
						if( inputs[IDPORT_ORDER]->size() > 1 ) // test for > 1 because we want to keep the 1 trigger here
						{
							// there is more than one message in the trigger port...
							// skim any old ones
							while( inputs[IDPORT_ORDER]->size() > 1 )
								inputs[IDPORT_ORDER]->eraseFront();
						} // else: 1 trigger, keep it...
					}
					return; // note: no new message, so there is no need to do the below, simply return here
				}

				if( msgs.size() > 1 )
				{
                                        BufferWrite newdata;
					if( !m_bUseSegmentedOutput )
					{
                                                // we need to do a merge here
						newdata = alloc(size);
						if( newdata.valid() )
						{
							size_t pos = 0;

							// concatenate messages using memcpy
							for( std::vector<Message>::size_type i=0; i < msgs.size(); ++i )
							{
								memcpy(newdata.getWrite<char>(pos),msgs[i].data.getRead<char>(0),msgs[i].data.getSize());
								pos += msgs[i].data.getSize();
							}
						}
					}
					else
					{
						// concatenate messages by using segments
						for( std::vector<Message>::size_type i=0; i < msgs.size(); ++i )
                                                    newdata.Buffer::operator+= ( msgs[i].data );
					}

					MessagePut msgput;

					///@todo check the handling of stamps here
					msgput.stamps.clone(msgs[0].stamps,&inputs[IDPORT_IN0]->getStampList());
					msgput.data = newdata;

					// put and increase the number of output messages for this filter by 1
					outputs[0]->put(msgput, dispatcher, m_nNum++);
				}
				else
				{
					// this is a simple forward of one message, so there is no need to do the merge
					// here. Simply forward the one message.
					MessagePut msgput;
					msgput.stamps.clone(msgs[0].stamps, &inputs[IDPORT_IN0]->getStampList());
					msgput.data   = msgs[0].data;
					outputs[0]->put(msgput, dispatcher, m_nNum++);
				}

				// purge trigger queue on request
				if(m_bSampleTrigger)
				{
					purgeQueue(IDPORT_ORDER); // kill 'em all
				}
				else
					inputs[IDPORT_ORDER]->eraseFront(); // kill just this one trigger we have processed...


			}
			///////////////////////////////////////////////////////////////////
			// FLOWVR-PLUGD-STUFF
			///////////////////////////////////////////////////////////////////

			GenClass<MultiplexMerge>
			MultiplexMergeClass("flowvr.plugins.FilterMultiplexMerge", // name
								"Implements an OR-condition on the incoming ports, merges all messages on any input upon receiving an explicit trigger."); // description

			Class* MultiplexMerge::getClass() const
			{
			  return &MultiplexMergeClass;
			}
		} // namespace balsac
	} // namespace render
} // namespace flowvr

