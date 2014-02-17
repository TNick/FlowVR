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
* File: setRelativeTransformPlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/render/balzac/services/transformstateservice.h>
#include <ftl/mat.h>

using namespace flowvr::render::balzac;


extern "C" void getParameters( flowvr::portutils::ARGS &args )
{
	args["UPDATE-TS"] = flowvr::portutils::Parameter("1", flowvr::portutils::Parameter::P_NUMBER);
	args["VERBOSE"] = flowvr::portutils::Parameter("0", flowvr::portutils::Parameter::P_NUMBER);
}
namespace
{

	class setRelativeTransformPlugHandler : public flowvr::portutils::SinkPortHandler
	{
	public:
		setRelativeTransformPlugHandler( TransformstateService *pService, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::SinkPortHandler()
		, m_service(pService)
		, m_bUpdateTs(true)
		, m_bVerbose(false)
		{
			try
			{
				m_bUpdateTs = (args("UPDATE-TS").getValue<int>() ? true : false);
				m_bVerbose  = (args("VERBOSE").getValue<int>() ? true : false );
			}
			catch( std::exception &e )
			{

			}

			std::cerr << "update-TS set to :" << m_bUpdateTs << std::endl;
		}

		virtual eState handleMessage( const flowvr::Message &in, 
                                      const flowvr::StampList *stampsIn )
		{
			if(in.data.getSize() == 0)
				return E_OK;

			const ftl::Mat4x4f m = *in.data.getRead<ftl::Mat4x4f>();

			if(m_bVerbose)
				std::cout << getHandlerName() << ": " << m << std::endl;

			m_service->applyTransform(m, TransformstateService::MD_LEFT_MULT, m_bUpdateTs) ; // T*m*TI * m_service->getTransform() );
			return E_OK;
		}


		TransformstateService *m_service;
		bool m_bUpdateTs, m_bVerbose;
	};
}


DEFIMP_PORTPLUG( setRelativeTransformPlugHandler, TransformstateService, setRelativeTransformPlug )

