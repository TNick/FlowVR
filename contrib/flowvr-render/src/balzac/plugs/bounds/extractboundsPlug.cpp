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
* File: extractbounds.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/render/chunk.h>
#include <flowvr/render/bbox.h>

#include <map>
#include <vector>
#include <set>

#include <flowvr/render/balzac/services/extractboundsservice.h>

using namespace flowvr::render::balzac;


namespace
{
	class extractboundsHandler : public flowvr::portutils::SinkPortHandler
	{
	public:
		extractboundsHandler(extractboundsservice *service, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::SinkPortHandler()
		, m_service(service)
		{

		}


		virtual eState handleMessage( const flowvr::Message &in, const flowvr::StampList *sl )
		{
			if( in.data.empty() )
				return E_OK;

			if(m_service->extractBounds(in))
				return E_OK;
			else
				return E_ERROR;
		}

		extractboundsservice *m_service;
	};
}


DEFIMP_PORTPLUG_NOPRM( extractboundsHandler, extractboundsservice, extractbounds )

