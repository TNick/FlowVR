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
* File: extractsceneidsPlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/render/balzac/services/extractboundsservice.h>
#include <flowvr/render/balzac/data/msgtypes.h>

#include <vector>

using namespace std;
using namespace flowvr::render::balzac;
using namespace flowvr;

namespace
{

	class extractsceneidsPlugHandler : public flowvr::portutils::SourcePortHandler
	{
	public:
		extractsceneidsPlugHandler( extractboundsservice *pService, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::SourcePortHandler()
		, m_service(pService)
		, m_nLastRound(-1)
		{
		}

		virtual eState handleMessage( flowvr::MessageWrite &mw,
				                      flowvr::StampList *sl,
				                      flowvr::Allocator &allocator )
		{
			if( m_nLastRound != m_service->getPrimitiveStateChange() )
			{
				const extractboundsservice::RMAP &rsrc  = m_service->getRessources();
				const extractboundsservice::PMAP &prims = m_service->getPrimitives();

				if( prims.empty() )
					return E_OK;

				mw.data = allocator.alloc( prims.size() * sizeof( ObjectBounds ) );


				size_t n=0;
				for( extractboundsservice::RMAP::const_iterator it = rsrc.begin(); it != rsrc.end(); ++it, ++n )
				{
					if( !m_service->getCapCam() and (*it).first == flowvr::render::ID_CAMERA )
						continue;

					extractboundsservice::PMAP::const_iterator p = prims.find( (*it).first );
					if( p == prims.end() )
						continue;

					ObjectBounds *bounds = mw.data.getWrite<ObjectBounds>( n*sizeof(ObjectBounds) );

					(*bounds).id      = (*it).first;
					(*bounds).m_a     = (*it).second.a;
					(*bounds).m_b     = (*it).second.b;
					(*bounds).toWorld = (*p).second.accum;
				}

				m_nLastRound = m_service->getPrimitiveStateChange();
			}
			return E_OK;
		}

		long m_nLastRound;

		extractboundsservice *m_service;

	};
}


DEFIMP_PORTPLUG_NOPRM( extractsceneidsPlugHandler, extractboundsservice, extractsceneidsPlug )

