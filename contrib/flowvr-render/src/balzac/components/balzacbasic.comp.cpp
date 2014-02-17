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
* File: balzacbasic.comp.cpp                                      *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include <flowvr/render/balzac/components/balzacbasic.comp.h>
#include <flowvr/portutils/components/portmodulerun.comp.h>
#include <flowvr/render/balzac/components/filtermultiplexmerge.comp.h>

#include <flowvr/portutils/components/portinterfacecache.h>
#include <flowvr/app/components/connection.comp.h>
#include <flowvr/app/components/filterpresignal.comp.h>

#include <algorithm>

using namespace flowvr::app;
using namespace flowvr::portutils;

namespace
{
	void _printPortName( const Port *p )
	{
		std::cout << "[" << p->getId() << "]" << std::endl;
	}
}

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			BalzacBasic::BalzacBasic( const std::string &id,
					                  const std::string &strInterfaceKey,
					                  flowvr::app::Composite *parent,
					                  flowvr::app::CmdLine *run )
			: Composite( id )
			, m_interfaceKey(strInterfaceKey)
			, m_run( run ? (CmdLine*)run->clone() : NULL )
			, m_sceneportname("scene")
			{
				addParameter( "sceneportname", m_sceneportname );

				if(m_run == NULL)
				{
					FlowvrRunSSH *run = new FlowvrRunSSH(this);
					//run->openDisplay(":0");
					m_run = run;
				}
				PortInterfaceCache *cache = PortInterfaceCache::getSingleton();
				PortInterfaceCache::Interface interface = cache->loadInterface(id, m_interfaceKey);

				// add incoming proxy for the scene
				addPort("scene", INPUT, FULL ); // we mirror the 'scene' from the mmp

				// we route the scene 'out' again, maybe someone needs it as such
				// this saves one render pass from balzac (if we would have placed the out port there)
				addPort("scene_out", OUTPUT, FULL );

				// route the current cam state to the outside world, interaction shells will
				// probably have use for this
				addPort("camstate", OUTPUT, FULL );
				// port to forward relative camera transforms to
				addPort("relativecam", INPUT, FULL );
                //addPort("relativecam", INPUT, FULL );
                
                

				// this is a composite, so it does not have beginIt and endIt
				// but we might want to control Balzac's rendering loop from the outside
				// that's why we are going to mirror them to our interface
				addPort("beginIt", INPUT, STAMPS );
				addPort("endIt", OUTPUT, STAMPS );

				// this is a special stamps message port that can be used for controlling
				// the render loop
				addPort("render.trigger", INPUT, STAMPS );


				// now we 'export' any port from the balzac interface as was given by the user
				if(interface.getIsValid())
				{
					// we assert here that the user 'really' has given a port named 'scene'
					PortInterfaceCache::Interface::DESCMAP::const_iterator it = interface.m_mpDescriptors.find(m_sceneportname);
					if( it == interface.m_mpDescriptors.end() )
						throw CustomException("balzac interface does not export a port named '"+m_sceneportname+"'", __FUNCTION_NAME__);

					setInfo( "composite wrapper for balzac module given in ["+m_interfaceKey+"]" );

					for(PortInterfaceCache::Interface::DESCMAP::const_iterator cit = interface.m_mpDescriptors.begin();
						cit != interface.m_mpDescriptors.end(); ++cit )
					{
						(*cit).second.createPorts( *this ); // note that we pass false by default
						                                    // so we will not create ports that are not marked as 'export'
					}

				}
				// user gave a parent object, so pass this to insert ourselves (convenience)
				if(parent)
					parent->insertObject(this);
			}


			BalzacBasic::~BalzacBasic()
			{
				delete m_run;
			}

			void BalzacBasic::execute()
			{
				////////////////////////////////////////////////////////////////
				// first off: the renderer
				////////////////////////////////////////////////////////////////
				PortModuleRun *balzac = new PortModuleRun( "BalzacRenderer",
											 m_interfaceKey,
											 *m_run,
											 this,
											 "flowvr-balzac", false );

				///////////////////////////////////////////////////////////////
				// CREATE LINKS TO ALL PORTS MARKED TO BE EXPORTED
				///////////////////////////////////////////////////////////////
				PortInterfaceCache *cache = PortInterfaceCache::getSingleton();
				PortInterfaceCache::Interface interface = cache->loadInterface(getId(), m_interfaceKey);
				if(interface.getIsValid())
				{
					// we now have to create all the links that have to be exposed
					for(PortInterfaceCache::Interface::DESCMAP::const_iterator cit = interface.m_mpDescriptors.begin();
						cit != interface.m_mpDescriptors.end(); ++cit )
					{
						const PortInterfaceCache::PortDescriptor &desc = (*cit).second;

						// if we hit a port that is not to be exported, we skip it here
						// (so there will no connection be possible with that port from the outside)
						if( desc.isExportPort() == false )
							continue;

						// respect multiplicity
						size_t multiplicity = desc.getMultiplicity();

						// see whether we have to map the 'real' scene port name that was given by
						// parameter to our logical scene port.
						std::string localName = (desc.getName() == m_sceneportname ? "scene" : desc.getName());

						if( multiplicity == 1 )
							link( getPort(localName), balzac->getPort(desc.getName()) );
						else
						{
							// we have a multiplicity > 1
							for( size_t n=0 ; n < multiplicity; ++n )
							{
								// @todo have to ask Bruno about that:
								//  link a multiple incoming to a multiple port
								std::string strName = desc.getName() + ftl::toString<size_t>( n );

								// this will compile, but probably not work
								link( getPort(localName), balzac->getPort( strName ) );
							}
						}
					}
				}

				////////////////////////////////////////////////////////////////
				// now the multiplexing
				////////////////////////////////////////////////////////////////
				std::list<const flowvr::app::Port*> siblings;
				getPort("relativecam")->getPrimitiveSiblingList(siblings);
				if( !siblings.empty() )
				{
					FilterMultiplexMerge *mmp = createMMP( balzac );
					////////////////////////////////////////////////////////////////
					// add the viewcontrol component
					////////////////////////////////////////////////////////////////
					PortModuleRun *vc = new PortModuleRun( "viewcontrol", "viewcontrol.xml", *m_run, this );
					link( vc->getPort("camera"), mmp->getPort("in") );
					link( vc->getPort("current"), getPort("camstate") );
					link( getPort("relativecam"), vc->getPort("reltransform") );
					////////////////////////////////////////////////////////////////
					// viewcontrol END
					////////////////////////////////////////////////////////////////
				}
				else
				{
					getPort("scene")->getPrimitiveSiblingList(siblings);
					size_t sz = siblings.size();
					if( sz > 1 )
					{
						createMMP( balzac ); // create the multiplex-merge and render.trigger stuff
					}
					else if( sz == 1 )
					{
						// no need to merge the scene, avoid mmp and care about scene_out
						std::list<const flowvr::app::Port*> outsibs;
						getPort("scene_out")->getPrimitiveSiblingList(outsibs);
						if( !outsibs.empty() )
						{
							// there is somebody waiting for the scene_out:
							// we use a pre-signal for this case to 'route' the scene
							// 1) to balzac
							// 2) to this composite's custom scene_out port
							// as we can not route the same information directly without filter (fan-out)
							FilterPreSignal *out = addObject<FilterPreSignal>("scene2out");

							// in case the scene port of balzac is blocking, we insert a token for the presignal
							out->setParameter<int>("nb", (balzac->getPort(m_sceneportname)->getIsBlocking() == Port::ST_BLOCKING ? 1 : 0) );

							link( getPort("scene"), out->getPort("in") ); // link our scene to filter
							link( out->getPort("out"), balzac->getPort(m_sceneportname) ); // link filter out to balzac
							link( out->getPort("out"), getPort("scene_out") ); // link filter out to scene_out for others
						}
						else
						{
							// no one cares about the output in scene_out
							// directly link scene to balzac scene and forget about scene_out
							link( getPort("scene"), balzac->getPort(m_sceneportname) );
						}
					}
				}


				/////////////////////////////////////////////////////////////
				// always forward beginIt / endIt to outward modules
				/////////////////////////////////////////////////////////////
				link( getPort("beginIt"), balzac->getPort("beginIt") );
				link( balzac->getPort("endIt"), getPort("endIt") );
			}

			FilterMultiplexMerge *BalzacBasic::createMMP( Composite *balzac )
			{
				// ok somebody connected to the relative-cam, let's create a merge
				// filter for merging the cam primitives with the other primitives.
				FilterMultiplexMerge *mmp = addObject<FilterMultiplexMerge>("mmp");
				link( getPort("scene"), mmp->getPort("in") ); // directly link scene to in of mmp
				link( mmp->getPort("out"), getPort("scene_out") ); // mirror scene to output again
				link( mmp->getPort("out"), balzac->getPort(m_sceneportname) );

				// user connected the 'render.trigger'?
				std::list<const flowvr::app::Port*> siblings;
				getPort("render.trigger")->getPrimitiveSiblingList(siblings);
				if( siblings.size() == 0 )
				{
					// nope.
					// see if we link to a blocking scene port
					if( balzac->getPort(m_sceneportname)->getIsBlocking() == Port::ST_BLOCKING )
					{
						// balzac has a blocking scene, so we will probably deadlock
						// avoid that by adding a presignal
						FilterPreSignal *ps = addObject<FilterPreSignal>("bLRender");
						link( balzac->getPort("endIt"), ps->getPort("in") );
						link( ps->getPort("out"), mmp->getPort("trigger") );
						ps->setParameter<int>("nb",1); // give a token to the ring ;)

						// do not throw away old orders in the mmp
						// this ensures that the pre-signal token is not lost due to a sample in
						// the mmp; in case a scene arrives late, balzac still gets its turn to render.
						mmp->setParameter<bool>("sampleOrder", false);

					}
					else
					{
						// port is either non-blocking OR we do not know, in any case
						// link the endIt of balzac to the trigger
						link( balzac->getPort("endIt"), mmp->getPort("trigger") );

						// in either case, throw away old orders
						// only use the last current order (do not wait for incoming scenes,
						// let balzac turn as quick as possible)
						mmp->setParameter<bool>("sampleOrder", true);

						if( !balzac->getIsBoundByInports() ) // balzac may turn without limits if there is no one out there
							mmp->setParameter<bool>("poolOrder", false);  // do not keep old orders in the mmp
																		  // (long queue would be the result)
					}
				}
				else
					link( getPort("render.trigger"), mmp->getPort("trigger") ); // directly link, the user thought about this

				return mmp;
			}

			Component *BalzacBasic::create() const
			{
				return new BalzacBasic( getId(), m_interfaceKey );
			}
		}
	}
}



