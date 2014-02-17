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

#include <flowvr/portutils/components/portparallel.comp.h>

#include <flowvr/app/core/exception.h>
#include <flowvr/app/components/connection.comp.h>
#include <sstream>


namespace flowvr
{
	namespace portutils
	{

		// Clone interface of component
		PortParallel::PortParallel(const std::string& id_, const Component &md)
		: Composite(id_, "Communication")
		, module( md.create() )
		{
			setInfo("N times [" + md.getId() +" in parallel (N is determined at run-time)");

			addParameter("LOOKUP_NB_INSTANCES_METHOD", "FROM_HOSTS");
			addParameter("INSTANCES_FROM_PORT");
			addParameter("NB_INSTANCES");

			// setup ports by cloning
			for (ConstpPortIterator it = module->getPortBegin(); it != module->getPortEnd(); ++it)
			{
				addPort((*it)->getId(), (*it)->getType(), (*it)->getMsgType(),
						((*it)->getMsgType() == flowvr::app::SAMEAS ? (*it)->getSameAs() : ""),
						(*it)->getBlockingState());
			}
		}

		PortParallel::~PortParallel()
		{
			delete module;
		}

		flowvr::app::Component* PortParallel::create() const
		{
			return new PortParallel(getId(), *module);
		}

			void PortParallel::execute()
			{
			const std::string type = getParameter<std::string> ("LOOKUP_NB_INSTANCES_METHOD");
			if (type == "STATIC")
			{
				const unsigned int nbComponents = getParameter<unsigned int> ("NB_INSTANCES");
				if (nbComponents == 0)
				{
					throw flowvr::app::CustomException(
							"In PortParallel " + getFullId()
									+ ", LOOKUP_NB_INSTANCES_METHOD = STATIC and NB_INSTANCES = 0. NB_INSTANCE should be > 0", __FUNCTION_NAME__);
				}
				for (unsigned int i = 0; i != nbComponents; ++i)
				{
					flowvr::app::Component *rnNode = NULL;
					try
					{
						module->create();
						rnNode->setId( toString<unsigned int>(i) );
						insertObject(rnNode);
					}
					catch(flowvr::app::CustomException & e )
					{
						delete rnNode;
						throw e;
					}


					for (pPortIterator itLink = rnNode->getPortBegin();itLink != rnNode->getPortEnd(); ++itLink)
					{
						flowvr::app::Port* parentPort = getPort((*itLink)->getId());
						link(*itLink, parentPort);
					}
				}
			}
			else
			{
				if (type == "FROM_PORT")
				{
					const std::string portName = getParameter<std::string> ("PORT_NAME_INSTANCES");
					flowvr::app::Port* pPort = this->getPort(portName);
					unsigned int i = 0;
					std::list<const flowvr::app::Port*> primlist;
					pPort->getPrimitiveSiblingList(primlist);

					if (primlist.size() == 0)
					{
						throw flowvr::app::CustomException(
								"In PortParallel " + getFullId()
										+ ", LOOKUP_NB_INSTANCES_METHOD = FROM_PORT and PORT_NAME_INSTANCES is "
										+ portName
										+ " but NB_INSTANCES will be 0 (No Primitive linked to this port). NB_INSTANCE should be > 0", __FUNCTION_NAME__);
					}
					this->setParameter<unsigned int> ("NB_INSTANCES", primlist.size());
					for(std::list<const flowvr::app::Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
					{
						flowvr::app::Component *rnNode = module->create();
						rnNode->setId( toString<unsigned int>(i) );
						try
						{
							insertObject(rnNode);
						}
						catch(flowvr::app::CustomException &e)
						{
							delete rnNode;
							throw e;
						}

						for (pPortIterator itLink = rnNode->getPortBegin(); itLink
								!= rnNode->getPortEnd(); ++itLink)
						{
							if ((*itLink)->getId() == portName)
							{
								link(pPort,*itLink);
								(*itLink)->addRestrictionToPrimitiveSibling(*it);
							}
							else
							{
								flowvr::app::Port* p =
									this->getPort((*itLink)->getId());
								try
								{
									std::list<const flowvr::app::Port*> pprimlist;
									p->getPrimitiveSiblingList(pprimlist);
									if (pprimlist.size() != primlist.size())
										link(*itLink, p);
									else
									{
										std::list<const flowvr::app::Port*>::const_iterator
												itP = pprimlist.begin();
										for (unsigned int ip = 0; ip != i; ++ip)
										{
											++itP;
										}
										link(p, *itLink);
										(*itLink)->addRestrictionToPrimitiveSibling(
												*itP);
									}
								}
								catch (flowvr::app::CustomException e)
								{
									link(*itLink, p);
								}
							}
						}
						++i;
					}

				}
				else
				{
					if (hostList.empty())
						throw flowvr::app::CustomException(
								"In PortParallel " + getFullId()
							  + ", LOOKUP_NB_INSTANCES_METHOD = FROM_HOST but NB_INSTANCES will be 0 (Hosts List is empty). NB_INSTANCE should be > 0", __FUNCTION_NAME__);

					unsigned int i = 0;
					for (flowvr::app::HostList::const_iterator itHost = hostList.begin(); itHost != hostList.end(); ++itHost, ++i)
					{
						flowvr::app::Component *rnNode = module->create();
						rnNode->setId(toString<unsigned int> (i));

						try
						{
							insertObject(rnNode);
						}
						catch(flowvr::app::CustomException &e)
						{
							delete rnNode;
							throw e;
						}

						for (pPortIterator itLink = rnNode->getPortBegin(); itLink != rnNode->getPortEnd(); ++itLink)
						{
							flowvr::app::Port* parentPort = this->getPort((*itLink)->getId());
							link(*itLink, parentPort);
						}
					}

					this->setParameter<unsigned int> ("NB_INSTANCES", hostList.size());
				}
			}
		}

			void PortParallel::setHosts()
			{
				const std::string type = getParameter<std::string> ("LOOKUP_NB_INSTANCES_METHOD");
				if ( type == "FROM_PORT")
				{
					//SAME AS SIBLING
					const std::string portName = getParameter<std::string> ("PORT_NAME_INSTANCES");
					flowvr::app::Port* pPort = this->getPort(portName);
					unsigned int i = 0;
					std::list<const flowvr::app::Port*> primlist;
					pPort->getPrimitiveSiblingList(primlist);

					if (primlist.size() == 0)
						throw flowvr::app::NoPrimitiveConnectedException(portName, getFullId(), __FUNCTION_NAME__);

					for(std::list<const flowvr::app::Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
					{
						flowvr::app::Component* rnNode = getComponent(toString<unsigned int> (i));

						// CASE OF SIBLING IS A CONNECTION
						flowvr::app::Component* siblingComp = (*it)->getOwner();
						while ( dynamic_cast<flowvr::app::ConnectionBase*>(siblingComp) )
						{
							std::string oppositePort = ((*it)->getId() == "in" ? "out" : "in");

							std::list<const flowvr::app::Port*> oppositeprimlist;
							siblingComp->getPort(oppositePort)->getPrimitiveSiblingList(oppositeprimlist);
							siblingComp = (*oppositeprimlist.begin())->getOwner();
						}


						if (siblingComp )
						{
							// siblingComp is a primitive (but not a connection)  so one host expected
							if(siblingComp->hostList.size() > 1 )
								throw flowvr::app::CustomException("Primitive" + getFullId() + ": "+ toString<int>(hostList.size())+ " hosts (one expected)", __FUNCTION_NAME__);
							else if( siblingComp->hostList.empty() )
							{
								throw flowvr::app::CustomException("Primitive" + getFullId() + ": no host (one expected)", __FUNCTION_NAME__);
							}

							rnNode->hostList.push_back(siblingComp->hostList.front());
							rnNode->hostList.back().appendtoOrigin("=>"+siblingComp->getFullId());
						}
						++i;

					}
				}
				else
				{
					if (type == "FROM_HOSTS" )
					{
						if ( hostList.empty() )
							throw flowvr::app::CustomException("PortParallel " + getFullId() + ": empty host list (at least one host expected)", __FUNCTION_NAME__);

						unsigned int i = 0;
						for (flowvr::app::HostList::const_iterator itHost = hostList.begin(); itHost != hostList.end(); ++itHost, ++i)
						{
							Component* rnNode = getComponent(toString<unsigned int>(i));
							if (dynamic_cast<flowvr::app::ConnectionBase*>(rnNode)  == NULL)
							{
								rnNode->hostList.push_back(*itHost);
								rnNode->hostList.back().appendtoOrigin("->"+getId());
							}
						}

					} // end if
				} // end else
			} // end function

	} // namespace portutils
} // namespace flowvr
