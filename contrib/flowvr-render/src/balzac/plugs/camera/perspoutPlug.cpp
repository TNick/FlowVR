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
* File: perspoutPlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>
#include <flowvr/render/balzac/data/msgtypes.h>

#include <flowvr/render/chunkrenderwriter.h>

using namespace flowvr::render;

using namespace flowvr::render::balzac;
using namespace flowvr::portutils;

#define PI 3.14159265358979323846264338327


extern "C" void getParameters( flowvr::portutils::ARGS &args )
{
	args["FOV"]   = Parameter("60.0", "Y-direction field of view in degree", Parameter::P_NUMBER);
	args["ZNEAR"] = Parameter("0.001", "near plane distance in camera space", Parameter::P_NUMBER);
	args["ZFAR"]  = Parameter("1000.0", "far plane distance in camera space", Parameter::P_NUMBER);
	args["VERBOSE"] = Parameter("false", "output intermediate results", Parameter::P_BOOLEAN);
	args["SCENEOUT"] = Parameter("true", "output as scene chunk (true) or matrix (false)", Parameter::P_BOOLEAN);
}

namespace
{

	class perspoutPlugHandler : public flowvr::portutils::PipeHandler
	{
	public:
		perspoutPlugHandler( const flowvr::portutils::ARGS &args )
		: flowvr::portutils::PipeHandler()
		, m_fov(60.0f)
		, m_znear(0.001f)
		, m_zfar(1000.0f)
		, m_verbose(false)
		, m_sceneout(true)
		{
			try
			{
				m_fov = args("FOV").getValue<float>();
				m_znear = args("ZNEAR").getValue<float>();
				m_zfar  = args("ZFAR").getValue<float>();

				m_verbose = args("VERBOSE").getValue<bool>();
				m_sceneout = args("SCENEOUT").getValue<bool>();

				if( m_verbose )
					std::cout << "switched output to mode: "
					          << (m_sceneout ? "SCENE" : "MATRIX") << std::endl;
			}
			catch( std::exception & )
			{

			}
		}

		virtual eState handleMessagePipe( const flowvr::Message &m_in,
						                          const flowvr::StampList *sl_in,
						                          flowvr::MessageWrite &m_out,
						                          flowvr::StampList *sl_out,
						                          flowvr::Allocator &allocate)
		{
			if( m_in.data.getSize() == 0 )
				return E_OK;

			ViewportChgMsg msg;
			msg.fromBuffer( m_in.data, 0 );

			if( m_verbose )
			{
				std::cout << " handler [" << this->getHandlerName() << "]: "
						  << "vp received: " << std::endl
						  << "x: " << msg.m_x << std::endl
						  << "y: " << msg.m_y << std::endl
						  << "w: " << msg.m_w << std::endl
						  << "h: " << msg.m_h << std::endl;
			}



			if( m_sceneout )
			{
				ftl::Mat4x4f m = getProjection( msg.m_w, msg.m_h );
				m_scene.addParam( ID_CAMERA, ChunkPrimParam::PROJECTION, "", m );
				m_out = m_scene.dump<flowvr::Allocator>( &allocate );
			}
			else
			{
				m_out.data = allocate.alloc( sizeof( Mat4x4f ) );
				Mat4x4f *m = m_out.data.getWrite<Mat4x4f>();
				*m = getProjection( msg.m_w, msg.m_h );
			}

			return E_OK;
		}


		ftl::Mat4x4f getProjection(int w, int h) const
		{
			return ftl::matrixPerspectiveProjection(m_fov, float(w)/float(h), m_znear, m_zfar);
		}

		ChunkRenderWriter        m_scene;

		float m_fov,
		      m_znear,
		      m_zfar;
		bool m_verbose, m_sceneout;
	};
}


DEFIMP_PORTPLUG_NOSERVICE( perspoutPlugHandler, perspoutPlug )

