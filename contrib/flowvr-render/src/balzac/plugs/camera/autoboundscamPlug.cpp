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
* File: autoboundscamPlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/render/balzac/services/autoboundsservice.h>
#include <flowvr/render/bbox.h>
#include <flowvr/utils/timing.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <ftl/chunkevents.h>

#include <flowvr/render/balzac/data/msgtypes.h>



using namespace flowvr::render::balzac;
using namespace flowvr::portutils;

using namespace ftl;
using namespace flowvr::render;

extern "C" void getParameters( flowvr::portutils::ARGS &args )
{
	args["FOV"]       = Parameter("60",   "Field-of-view in degree (y-aspect)", Parameter::P_NUMBER );
	args["ASPECT"]    = Parameter("1.33", "view-ratio (in relative units) (x-aspect)", Parameter::P_NUMBER );
	args["KEEPBIGGESTBB"] = Parameter("true", "Always show the biggest bounding box encountered", Parameter::P_BOOLEAN );
	args["VERBOSE"] = Parameter("false", "Output cam transformation to console", Parameter::P_BOOLEAN);
	args["CAMID"] = Parameter("0", "ID of the camera to control", Parameter::P_BOOLEAN);
}


#define PI 3.14159265358979323846264338327

namespace
{

	class autoboundscamPlugHandler : public flowvr::portutils::MultiplexHandler
	{
	public:
		enum eBtState
		{
			BS_NONE=0,
			BS_LEFT=1,
			BS_MIDDLE=2,
			BS_RIGHT=4
		};

		autoboundscamPlugHandler( autoboundsservice *pService, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::MultiplexHandler()
		, m_service(pService)
		, m_fov(60.0f)
		, m_aspect( 0.75f )
		, m_vp(0,0,640,480)
		, m_lastVpUpd(0)
		, m_lastCamUpd(0)
		, m_bKeepBiggestBB(true)
		, m_bVerbose(false)
		, m_camId(ID_CAMERA)
		, m_lastIt(-1)
		{
			try
			{
				m_fov    = args("FOV").getValue<float>();
				m_aspect = args("ASPECT").getValue<float>();
				m_bKeepBiggestBB = args("KEEPBIGGESTBB").getValue<bool>();
				m_bVerbose = args("VERBOSE").getValue<bool>();
				m_camId = args("CAMID").getValue<ID>();
			}
			catch( std::exception &e )
			{

			}

			m_first[0] = m_first[1] = -1;

			updateProjection();
		}

		virtual eState handleMultiplexing( const PortComm::MsgMap::SLOTS &m_in,
										   const PortComm::MsgMap::SLOTSTAMPS  &sl_in,
				                           PortComm::MsgMap::OSLOT  &m_out,
				                           PortComm::MsgMap::SLOTSTAMPS &sl_out,
				                           flowvr::Allocator & allocate )
		{
			/////////////////////////////////////////////////////////////////////////
			// HANDLE INPUT
			/////////////////////////////////////////////////////////////////////////

			if( m_in[0].data.getSize() )
				handleViewportUpdate( m_in[0] );

			if( m_in[1].data.getSize() >= sizeof(BBox) )
				handleBoundingBoxUpdate( m_in[1] );

			/////////////////////////////////////////////////////////////////////////
			// HANDLE OUTPUT
			/////////////////////////////////////////////////////////////////////////
			if( m_lastVpUpd != m_service->m_projection.getLastUpdateTs() )
			{
				m_lastVpUpd = m_service->m_projection.getLastUpdateTs();
				m_scene.addParam( m_camId, ChunkPrimParam::PROJECTION, "", m_service->m_projection.getTransform() );
				if( m_bVerbose )
					std::cout << "proj: " << m_service->m_projection.getTransform() << std::endl;

			}

			if( m_lastCamUpd != m_service->m_camstate.getLastUpdateTs() )
			{
				m_lastCamUpd = m_service->m_camstate.getLastUpdateTs();
				m_scene.addParam( m_camId, ChunkPrimParam::TRANSFORM, "", m_service->m_camstate.getTransform() );
				if( m_bVerbose )
					std::cout << "cam: " << m_service->m_camstate.getTransform() << std::endl;
			}


			if( m_scene.isDirty() )
				m_out[0] = m_scene.dump<flowvr::Allocator>( &allocate );

			return E_OK;
		}




		void handleViewportUpdate( const flowvr::Message &m_in )
		{
			ViewportChgMsg msg;
			msg.fromBuffer( m_in.data, 0 );

			m_vp[0] = msg.m_x;
			m_vp[1] = msg.m_y;
			m_vp[2] = msg.m_w;
			m_vp[3] = msg.m_h;

			updateProjection();
		}

		void handleBoundingBoxUpdate( const flowvr::Message &in )
		{
			if( m_bKeepBiggestBB )
			{
				BBox bb;
				bb = *( in.data.getRead<BBox>());

				float size = m_curBB.size();

				m_curBB +=  bb ;

				if( m_curBB.size() != size )
				{
					updateCamPosition();
					updateProjection();
				}
			}
			else
			{
				m_curBB = *( in.data.getRead<BBox>());
				updateCamPosition();
				updateProjection();
			}

			if(m_bVerbose)
				std::cout << "BB = " << m_curBB << std::endl;
		}

		void updateCamPosition()
		{
			Mat4x4f cam = m_service->m_camstate.getTransform();

			if (!m_curBB.isEmpty())
			{
				Vec3f bbsize   =  m_curBB.b - m_curBB.a; // get the diagonal size of the bounding box
				Vec3f bbcenter = (m_curBB.a + m_curBB.b) * 0.5; // find the center of the bounding box
				Vec3f campos   = bbcenter; // set the initial campos to the center of the bounding box

				if(m_bVerbose)
				{
					std::cout << "a = " << m_curBB.a << std::endl
							  << "b = " << m_curBB.b << std::endl
							  << "bbsize = " << bbsize << std::endl
							  << "bbcenter = " << bbcenter << std::endl
							  << "campos = " << campos << std::endl;
				}

				// get the distance of half the depth of the bounding box
				// to position the cam directly in front of the box
				float dist = (m_curBB.b.z() - m_curBB.a.z()) / 2.0f;

				// see for the dominating axis
				// (normalized x by view aspect) dominates y?
				if ( (bbsize.x() / m_aspect) > bbsize.y())
				{
					dist += (1.5f * (bbsize.x() / m_aspect) ) * ::tan((m_fov / 2.0f) * PI / 180.0f);
				}
				else
				{
					// no, y dominates
					dist += (1.5f * bbsize.y()) * tan((m_fov / 2.0f) * PI / 180.0f);
				}

				campos.z() = dist + ((m_curBB.b.z() + m_curBB.a.z()) / 2.0f);

				cam[0][3] = campos[0];
				cam[1][3] = campos[1];
				cam[2][3] = campos[2];

				m_service->m_camstate.setTransform( cam );

				if( m_bVerbose )
					std::cout << "result: pos: " << campos << " ; dist: " << dist << " ; zoom: "  << std::endl;
			}
		}

		void updateProjection()
		{
			float fov    = 1.0f / ::tan(((m_fov/180.0f)*PI)/2.0f);

			m_aspect = float(m_vp[2])/float(m_vp[3]);

			// determine the distance of the bounds from the current cam position
			Mat4x4f cam;
			if(!cam.setAsInverseOf( m_service->m_camstate.getTransform() )) // camstate is in world-space, so we
			                                                  // use the inverse to compute the distance to m_curBB
			{
				std::cerr << "could not invert cam matrix?!" << std::endl
						  << m_service->m_camstate.getTransform() << std::endl
						  << m_service->m_camstate.getLastUpdateTs() << std::endl;
				return;
			}

			BBox cambox;
			cambox.apply( m_curBB, cam ); // transform m_curBB to cam-space


			// in case we have bounds: slightly exaggerate the edges by 1%
			float znear  = m_curBB.isEmpty() ? 0.001f  : std::max<float>( 0.01f,         -cambox.a.z() * 1.01f ); // max: if any of the two is 'behind' then clip
			float zfar   = m_curBB.isEmpty() ? 1000.0f : std::max<float>( znear/1000.0f, -cambox.b.z() * 0.99f ); // max: if any of the two is 'behind' then clip

			if( zfar < znear )
			{
				float zf = znear;
				znear = zfar;
				zfar = zf;
			}

			Mat4x4f proj = ftl::matrixPerspectiveProjection( fov, m_aspect, znear, zfar );

			if(m_bVerbose)
			{
				std::cout << "camst  = " << m_service->m_camstate.getLastUpdateTs() << std::endl
				          << "fov    = " << fov << std::endl
						  << "aspect = " << m_aspect << std::endl
				          << "znear  = " << znear << " ; " << -cambox.a.z() * 1.01f << std::endl
						  << "zfar   = " << zfar  << " ; " << znear / 1000.0f << " ; "<< -cambox.b.z() * 0.99f << std::endl
						  << "curbb  = " << m_curBB.toString() << std::endl
						  << "cam    = " << cam << std::endl
						  << "cambox = " << cambox.toString() << std::endl;
			}

			m_service->m_projection.setTransform( proj );
		}

		BBox  m_curBB;
		Vec4i m_vp;
		Vec2f m_first;

		ChunkRenderWriter  m_scene;

		autoboundsservice *m_service;

		flowvr::utils::microtime m_lastVpUpd,
		                         m_lastCamUpd;


		float m_fov,
		      m_aspect;

		bool m_bKeepBiggestBB, m_bVerbose;

		ID m_camId;

		int m_lastIt;
	};
}


DEFIMP_PORTPLUG( autoboundscamPlugHandler, autoboundsservice, autoboundscamPlug )

