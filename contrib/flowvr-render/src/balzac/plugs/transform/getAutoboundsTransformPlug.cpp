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
#include <flowvr/render/chunkrenderwriter.h>

#include <ftl/mat.h>

using namespace flowvr::render::balzac;
using namespace flowvr::portutils;
using namespace ftl;
using namespace flowvr::render;


extern "C" void getParameters( ARGS &args )
{
	args["UPDATETS"] = Parameter("1", Parameter::P_NUMBER);
	args["VERBOSE"] = Parameter("0", Parameter::P_NUMBER);
	args["WHICH"] = Parameter("camstate", Parameter::P_STRING, Parameter::PS_NONE, "give 'camstate' or 'projection' (anything else than 'camstate' is 'projection'");
	args["TYPE"] = Parameter("matrix", Parameter::P_STRING, Parameter::PS_NONE, "output as 'matrix' or 'chunk'");
	args["CAMID"] = Parameter("0", Parameter::P_NUMBER, Parameter::PS_NONE, "cam id to use when outputting a chunk");
}
namespace
{

	class getAutoboundsTransformPlugHandler : public SourcePortHandler
	{
	public:
		getAutoboundsTransformPlugHandler( autoboundsservice *pService, const ARGS &args )
		: SourcePortHandler()
		, m_service(pService)
		, m_bUpdateTs(true)
		, m_bVerbose(false)
		, m_nWhich( W_CAMSTATE )
		, m_nFrom( F_LEFT )
		, m_nCamId(0)
		, m_nOutType(TP_MATRIX)
		{
			try
			{
				m_bUpdateTs = (args("UPDATETS").getValue<int>() ? true : false);
				m_bVerbose  = (args("VERBOSE").getValue<int>() ? true : false );
				m_nWhich    = (args("WHICH").getValueString() == "camstate" ? W_CAMSTATE : W_PROJECTION);
				m_nCamId    =  args("CAMID").getValue<int>();
				m_nOutType  = (args("TYPE").getValueString() == "matrix" ? TP_MATRIX : TP_CHUNK);
			}
			catch( std::exception &e )
			{

			}
		}

		virtual eState handleMessage( flowvr::MessageWrite &mw, flowvr::StampList *sl, flowvr::Allocator &alloc )
		{
			TransformstateService *state = ( m_nWhich == W_CAMSTATE ? &m_service->m_camstate : &m_service->m_projection );
			if( m_nOutType == TP_MATRIX )
			{
				mw.data = alloc.alloc( sizeof( Mat4x4f ) );
				*mw.data.getWrite<Mat4x4f>() = (*state).getTransform();
			}
			else
			{
				ChunkRenderWriter scene;
				scene.addParam( m_nCamId, ChunkPrimParam::PROJECTION, "", (*state).getTransform() );
			}
			return E_OK;
		}

		autoboundsservice *m_service;
		bool m_bUpdateTs, m_bVerbose;

		int m_nCamId, m_nOutType;

		enum
		{
			TP_MATRIX=0,
			TP_CHUNK
		};

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


DEFIMP_PORTPLUG( getAutoboundsTransformPlugHandler, autoboundsservice, getAutoboundsTransformPlug )

