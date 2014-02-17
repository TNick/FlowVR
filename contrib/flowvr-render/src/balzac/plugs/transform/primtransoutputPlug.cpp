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
* File: primtransoutputPlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>


#include <flowvr/render/balzac/services/transformstateservice.h>

#include <flowvr/render/chunkrenderwriter.h>

using namespace flowvr::render;
using namespace flowvr::render::balzac;


extern "C" void getParameters( flowvr::portutils::ARGS &args )
{
	args["ID"] = flowvr::portutils::Parameter( "-1", "fvr-render ID for the primitive to transform (-1 == no object)", flowvr::portutils::Parameter::P_NUMBER );
	args["VERBOSE"] = flowvr::portutils::Parameter( "false", "be verbose during update (outputs transform and ID upon change).", flowvr::portutils::Parameter::P_BOOLEAN );
}

namespace
{

	class primtransoutputPlugHandler : public flowvr::portutils::SourcePortHandler
	{
	public:
		primtransoutputPlugHandler( TransformstateService *pService, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::SourcePortHandler()
		, m_service(pService)
		, m_scene()
		, m_nLastUpdate(0)
		, m_nID(-1)
		, m_bVerbose(false)
		{
			try
			{
				m_nID = args("ID").getValue<int>();
				m_bVerbose = args("VERBOSE").getValue<bool>();
			}
			catch( std::exception &e )
			{

			}
		}

		virtual eState handleMessage( flowvr::MessageWrite &out, 
                                      flowvr::StampList *stampsOut,
                                      flowvr::Allocator &allocate )
		{
			if( m_nLastUpdate < m_service->getLastUpdateTs() )
			{
				std::vector<flowvr::ID> v;
				m_service->getTargetIds(v);
				if( v.empty() )
				{
					// better to do nothing when id was not set,
					// as we might still wait for a big part of the scene to
					// come upon init!
					if( m_nID >= 0 )
					{
						m_scene.addParam( m_nID, ChunkPrimParam::TRANSFORM, "", m_service->getTransform() );
						if(m_bVerbose)
							std::cout << getHandlerName() << ": added transform for primitive [" << m_nID << "] (param-ID)" << std::endl;
					}
				}
				else
				{
					for( std::vector<flowvr::ID>::const_iterator cit = v.begin(); cit != v.end(); ++cit )
					{
						m_scene.addParam( *cit, ChunkPrimParam::TRANSFORM, "", m_service->getTransform() );
						if(m_bVerbose)
							std::cout << getHandlerName() << ": added transform for primitive [" << *cit << "]" << std::endl;
					}
				}

				if(m_bVerbose)
					std::cout << getHandlerName() << ": " << m_service->getTransform() << std::endl;

				m_nLastUpdate = m_service->getLastUpdateTs();
			}

			if( m_scene.isDirty() )
				out = m_scene.dump<flowvr::Allocator>( &allocate );

			return E_OK;
		}

		flowvr::utils::microtime m_nLastUpdate;
		TransformstateService   *m_service;
		ChunkRenderWriter        m_scene;
		int                      m_nID;
		bool                     m_bVerbose;
	};
}


DEFIMP_PORTPLUG( primtransoutputPlugHandler, TransformstateService, primtransoutputPlug )

