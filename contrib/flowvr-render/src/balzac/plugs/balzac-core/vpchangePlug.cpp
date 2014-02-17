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
* File: vpchange.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/utils/timing.h>
#include <flowvr/render/balzac/servicelayer/servicelayer.h>
#include <flowvr/render/balzac/servicelayer/display.h>

#include <flowvr/render/balzac/data/msgtypes.h>


namespace
{
	using namespace flowvr::render::balzac;

	class vpchangePlugHandler : public flowvr::portutils::SourcePortHandler
	{
	public:
		vpchangePlugHandler( ServiceLayer *pService, const flowvr::portutils::ARGS & )
		: flowvr::portutils::SourcePortHandler()
		, m_service(pService)
		, m_lastUpdate(0)
		{
		}

		virtual eState handleMessage( flowvr::MessageWrite &out,
									  flowvr::StampList *sl,
									  flowvr::Allocator &alloc )
		{
			Display *pDisp = m_service->getDisplay();

			if( pDisp == NULL )
				return E_ERROR;

			double dUpd = pDisp->getViewportUpdateTs();
			if( (dUpd != m_lastUpdate) or (m_lastUpdate == 0) )
			{
				m_lastUpdate = dUpd;
				writeViewport( pDisp, out, alloc );
			}

			return E_OK;
		}

		bool writeViewport( Display *pDisp, flowvr::MessageWrite &out, flowvr::Allocator &alloc ) const
		{
			flowvr::render::balzac::ViewportChgMsg chg;

			if( pDisp->getPosition(chg.m_x, chg.m_y, chg.m_w, chg.m_h) )
			{
				out.data = alloc.alloc( sizeof( chg ) );
				return chg.toBuffer( out.data );
			}

			return false;
		}

		ServiceLayer *m_service;
		flowvr::utils::microtime m_lastUpdate;
	};
}


DEFIMP_PORTPLUG_NOPRM( vpchangePlugHandler, ServiceLayer, vpchange )

