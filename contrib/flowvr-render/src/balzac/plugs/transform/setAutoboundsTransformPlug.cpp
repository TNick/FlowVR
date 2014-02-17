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

#include <flowvr/render/balzac/services/autoboundsservice.h>
#include <ftl/mat.h>

using namespace flowvr::render::balzac;
using namespace flowvr::portutils;


extern "C" void getParameters( ARGS &args )
{
	args["UPDATETS"] = Parameter("1", Parameter::P_NUMBER);
	args["VERBOSE"] = Parameter("0", Parameter::P_NUMBER);
	args["MULTFROM"] = Parameter("left", Parameter::P_STRING, Parameter::PS_NONE, "from 'left' or 'right' or 'set' (defaults to 'left' if string does not make sense)" );
	args["WHICH"] = Parameter("camstate", Parameter::P_STRING, Parameter::PS_NONE, "give 'camstate' or 'projection' (anything else than 'camstate' is 'projection'");
}
namespace
{

	class setAutoboundsTransformPlugHandler : public SinkPortHandler
	{
	public:
		setAutoboundsTransformPlugHandler( autoboundsservice *pService, const ARGS &args )
		: SinkPortHandler()
		, m_service(pService)
		, m_bUpdateTs(true)
		, m_bVerbose(false)
		, m_nWhich( W_CAMSTATE )
		, m_nFrom( F_LEFT )
		{
			try
			{
				m_bUpdateTs = (args("UPDATETS").getValue<int>() ? true : false);
				m_bVerbose  = (args("VERBOSE").getValue<int>() ? true : false );
				m_nWhich = (args("WHICH").getValueString() == "camstate" ? W_CAMSTATE : W_PROJECTION);

				std::string from = args("MULTFROM").getValueString();
				if( from == "left")
					m_nFrom = F_LEFT;
				else if( from == "right" )
					m_nFrom = F_RIGHT;
				else if( from == "replace" )
					m_nFrom = F_SET;
			}
			catch( std::exception &e )
			{

			}
		}

		virtual eState handleMessage( const flowvr::Message &in, 
                                      const flowvr::StampList *stampsIn )
		{
			if(in.data.getSize() == 0)
				return E_OK;

			const ftl::Mat4x4f m = *in.data.getRead<ftl::Mat4x4f>();


			TransformstateService *state = ( m_nWhich == W_CAMSTATE ? &m_service->m_camstate : &m_service->m_projection );

			if( m_nFrom == F_SET )
				(*state).setTransform( m, m_bUpdateTs );
			else
			{
				TransformstateService::eMode md = (m_nFrom == F_LEFT ? TransformstateService::MD_LEFT_MULT : TransformstateService::MD_RIGHT_MULT);
				(*state).applyTransform(m, md, m_bUpdateTs);
			}

			if(m_bVerbose)
			{
				std::cout << getHandlerName() << ": " << m << std::endl;
				std::cout << "from = " << m_nFrom << std::endl
						  << "which = " << m_nWhich << std::endl;
			}

			return E_OK;
		}

		autoboundsservice *m_service;
		bool m_bUpdateTs, m_bVerbose;

		enum
		{
			W_CAMSTATE,
			W_PROJECTION
		};

		enum
		{
			F_LEFT,
			F_RIGHT,
			F_SET
		};

		int m_nWhich, m_nFrom;
	};
}


DEFIMP_PORTPLUG( setAutoboundsTransformPlugHandler, autoboundsservice, setAutoboundsTransformPlug )

