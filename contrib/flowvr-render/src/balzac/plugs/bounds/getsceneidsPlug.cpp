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
				const extractboundsservice::PMAP &prims = m_service->getPrimitives();
				vector<ID> vecIds;

				for( extractboundsservice::PMAP::const_iterator it = prims.begin(); it != prims.end(); ++it )
				{
					if( !m_service->getCapCam() and (*it).first == flowvr::render::ID_CAMERA )
						continue;

					vecIds.push_back( (*it).first );
				}

				std::sort( vecIds.begin(), vecIds.end() );

				mw.data = allocator.alloc( vecIds.size() * sizeof(ID) );
				if( !mw.data.valid() )
					return E_ERROR;

				memcpy( mw.data.getWrite<ID>(), &vecIds[0], vecIds.size() * sizeof(ID) );

				m_nLastRound = m_service->getPrimitiveStateChange();
			}
			return E_OK;
		}

		long m_nLastRound;

		extractboundsservice *m_service;

	};
}


DEFIMP_PORTPLUG_NOPRM( extractsceneidsPlugHandler, extractboundsservice, extractsceneidsPlug )

