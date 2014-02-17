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
* File: getboundsPlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/render/balzac/services/extractboundsservice.h>


using namespace flowvr::render;
using namespace flowvr::render::balzac;

namespace
{

	class getboundsPlugHandler : public flowvr::portutils::SourcePortHandler
	{
	public:
		getboundsPlugHandler( extractboundsservice *pService, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::SourcePortHandler()
		, m_service(pService)
		, m_nLastRound(0)
		{
		}

		virtual eState handleMessage( flowvr::MessageWrite &out, 
                                      flowvr::StampList *stampsOut,
                                      flowvr::Allocator &allocator )
		{
			if( m_service->getRoundId() != m_nLastRound )
			{
				BBox bounds = m_service->getSceneBounds();

				if(!bounds.isEmpty())
				{
					out.data = allocator.alloc( sizeof(BBox) );
					BBox *bbout = out.data.getWrite<BBox>();
					*bbout = bounds;
				}
				m_nLastRound = m_service->getRoundId();
			}
			return E_OK;
		}

		long m_nLastRound;
		extractboundsservice *m_service;
	};
}


DEFIMP_PORTPLUG_NOPRM( getboundsPlugHandler, extractboundsservice, getboundsPlug )

