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
* File: relativeTransformPlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>
#include <flowvr/render/balzac/services/transformstateservice.h>


using namespace flowvr::render::balzac;

extern "C" void getParameters( flowvr::portutils::ARGS &args)
{
	args["MODE"] = flowvr::portutils::Parameter("0", flowvr::portutils::Parameter::P_NUMBER);
	args["VERBOSE"] = flowvr::portutils::Parameter("0", flowvr::portutils::Parameter::P_NUMBER);
}


namespace
{

	class getTransformPlugHandler : public flowvr::portutils::SourcePortHandler
	{
	public:
		getTransformPlugHandler( TransformstateService *pService, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::SourcePortHandler()
		, m_service(pService)
		, m_dLastUpdateTs(-1)
		, m_nMode(0)
		, m_bVerbose(false)
		{
			try
			{
				m_nMode = args("MODE").getValue<int>();
				m_bVerbose = args("VERBOSE").getValue<int>() ? true : false;

				if( m_bVerbose )
				{
					std::cout << " ** gettransformPlug: setting " << std::endl
							  << "\tMODE = " << m_nMode << std::endl;
				}
			}
			catch( std::exception &e )
			{
				std::cerr << "getValue() exception: " << e.what() << std::endl;
			}
		}

		virtual eState handleMessage( flowvr::MessageWrite &out, 
                                      flowvr::StampList *stampsOut,
                                      flowvr::Allocator &allocate )
		{

			if( m_dLastUpdateTs < m_service->getLastUpdateTs() )
			{
				m_dLastUpdateTs = m_service->getLastUpdateTs();

				out.data = allocate.alloc( sizeof( ftl::Mat4x4f ) );
				ftl::Mat4x4f *m;
				if(out.data.valid())
				{
					m = out.data.getWrite<ftl::Mat4x4f>();
					switch(m_nMode)
					{
					case 1:
					{
						*m = m_service->getTransform();
						break;
					}
					case 0:
					default:
						*m = m_service->getRelativeTransform();
						break;
					}
				}
				else
					return E_ERROR;

				if(m_bVerbose)
					std::cout << getHandlerName() << ": " << (m ? *m : ftl::Mat4x4f() )<< std::endl;
			}
			return E_OK;
		}


		TransformstateService *m_service;
		flowvr::utils::microtime m_dLastUpdateTs;
		int m_nMode;
		bool m_bVerbose;
	};
}


DEFIMP_PORTPLUG( getTransformPlugHandler, TransformstateService, getTransformPlug )

