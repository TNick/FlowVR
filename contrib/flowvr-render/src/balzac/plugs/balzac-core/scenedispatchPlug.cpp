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
* File: scenedispatch.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/render/balzac/servicelayer/servicelayer.h>
#include <flowvr/render/balzac/servicelayer/display.h>

#include <flowvr/render/cg/cgengine.h>

namespace
{

	using namespace flowvr::render::balzac;

	class scenedispatchPlugHandler : public flowvr::portutils::SinkPortHandler
	{
	public:
		scenedispatchPlugHandler( ServiceLayer *pService, const flowvr::portutils::ARGS &prm )
		: flowvr::portutils::SinkPortHandler()
		, m_service(pService)
		{
		}

		virtual eState handleMessage( const flowvr::Message &in,
                                      const flowvr::StampList *stampsIn )
		{
			Display *pDisp = m_service->getDisplay();
			if(!pDisp)
				return E_ERROR;

			flowvr::render::cg::CgEngine *engine = pDisp->getWorldCgEngine();

//			if( in.data.getNumberOfSegments() > 1 )
//				std::cout << "processing multiple segments (" << in.data.getNumberOfSegments() << ")" << std::endl;
			for( size_t n=0; n < in.data.getNumberOfSegments(); ++n )
			{
				// for the sake of simplicity, we wrap each segment in
				// a separate message and use the same stamps from the incoming message
				// as stamps
				flowvr::Message msg;
				msg.data = in.data[n];
				msg.stamps = in.stamps;

				if (!engine->process(msg))
				{
					std::string source;
					in.stamps.read(stampsIn->source, source);
					std::cerr << "Engine::process FAILED for handler "
							  << getHandlerName()
							  << " from " << source
							  << std::endl;
					return E_REMOVE;
				}
			}
			return E_OK;
		}
		ServiceLayer *m_service;
	};
}


DEFIMP_PORTPLUG_NOPRM( scenedispatchPlugHandler, ServiceLayer, scenedispatch )

