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
* File: applyTransformPlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/render/balzac/services/transformstateservice.h>
#include <ftl/mat.h>


using namespace flowvr::portutils;
using namespace flowvr::render::balzac;
using namespace flowvr;
using namespace ftl;


extern "C" void getParameters( ARGS &args )
{
	// insert parameter here
	args["FROM"] = Parameter("LEFT", "apply incoming transform from left or right", Parameter::P_STRING );
}



namespace
{

	class applyTransformPlugHandler : public flowvr::portutils::SinkPortHandler
	{
	public:
		applyTransformPlugHandler( TransformstateService *pService, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::SinkPortHandler()
		, m_service(pService)
		{
			try
			{
				std::string from = args("FROM").getValueString();
				std::transform(from.begin(), from.end(), from.begin(), ::toupper);
				if(from == "LEFT")
				{
					m_applyMode = TransformstateService::MD_LEFT_MULT;
				}
				else
					m_applyMode = TransformstateService::MD_RIGHT_MULT;
			}
			catch( std::exception &e )
			{
			}
		}

		virtual eState handleMessage( const flowvr::Message &in, 
                                      const flowvr::StampList *stampsIn )
		{
			if( !in.data.valid() or in.data.getSize() < 16*sizeof(float) )
				return E_ERROR;

			const Mat4x4f *m =  in.data.getRead<Mat4x4f>();
			m_service->applyTransform( *m, m_applyMode, true );
			return E_OK;
		}


		TransformstateService::eMode m_applyMode;
		TransformstateService       *m_service;
	};
}


DEFIMP_PORTPLUG( applyTransformPlugHandler, TransformstateService, applyTransformPlug )

