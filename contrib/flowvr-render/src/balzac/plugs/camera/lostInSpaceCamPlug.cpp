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
* File: lookatcamPlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/render/chunkrenderwriter.h>
#include <ftl/chunkevents.h>

#include <flowvr/render/balzac/services/transformstateservice.h>
#include <flowvr/render/balzac/data/msgtypes.h>

using namespace flowvr::portutils;
using namespace flowvr;
using namespace flowvr::render;
using namespace flowvr::render::balzac;
using namespace ftl;


extern "C" void getParameters( flowvr::portutils::ARGS &args )
{
	// add parameters here
	args["ZOOMSCALE"]  = Parameter( "1", "Factor for zoom-scaling", Parameter::P_NUMBER);
	args["TRANSSCALE"] = Parameter( "1", "Factor for translation-scaling", Parameter::P_NUMBER);
	args["ROTSCALE"] = Parameter( "1", "Factor for rotation-scaling", Parameter::P_NUMBER);
	args["ROTOFFSET"]  = Parameter( "0,0,-2", "Offset for rotation point relative to viewpoint", Parameter::P_LIST, Parameter::PS_NUMBER );
	args["WHEELSTEP"]  = Parameter( "1", "Translation value for 1 wheel click", Parameter::P_NUMBER);
	args["TSCALEFACTOR"] = Parameter( "0.01", "Scale factor for translation scale", Parameter::P_NUMBER );
	args["RSCALEFACTOR"] = Parameter( "0.01", "Scale factor for rotation scale", Parameter::P_NUMBER );
	args["WSCALEFACTOR"] = Parameter( "0.01", "Scale factor for rotation scale", Parameter::P_NUMBER );
}

namespace
{
	const float TRACKBALLSIZE = 1.0f;
	void axis_to_quat(ftl::Vec3f &a, float phi , ftl::Quat &q)
	{
		a.normalize();
		q.x = a[0]*sinf(phi/2.0f);
		q.y = a[1]*sinf(phi/2.0f);
		q.z = a[2]*sinf(phi/2.0f);
		q.w = cosf(phi/2.0f);
	}

	float tb_project_to_sphere(float r, float x, float y)
	{
		double d, t, z;

		d = sqrt(x*x + y*y);
		if (d < r * 0.70710678118654752440) {    /* Inside sphere */
			z = sqrt(r*r - d*d);
		} else {           /* On hyperbola */
			t = r / 1.41421356237309504880;
			z = t*t / d;
		}
		return float(z);
	}

	/**
	 * expects values
	 *  - [-1;1] [left ; right] (X)
	 *  - [-1;1] [up   ; down ] (Y)
	 * (values can be larger, but: [0;0] is the virtual center of the ball),
	 * p1 is "from", p2 is "to"
	 */
	void CalcRotation(float p1x, float p1y, float p2x, float p2y,
							 ftl::Quat &result, float nScale)
	{
		float phi;  /* how much to rotate about axis */
		float t;

		if (p1x == p2x && p1y == p2y) {
			/* Zero rotation */
			result = ftl::Quat(1,0,0,0);
			return;
		}

		/*
		 * First, figure out z-coordinates for projection of P1 and P2 to
		 * deformed sphere
		 */

		ftl::Vec3f v1(p1x, p1y, tb_project_to_sphere(TRACKBALLSIZE*nScale,p1x,p1y));
		ftl::Vec3f v2(p2x, p2y, tb_project_to_sphere(TRACKBALLSIZE*nScale,p2x,p2y));


		/*
		 *  Now, we want the cross product of V1 and V2
		 */

		ftl::Vec3f axis = ftl::cross(v1,v2);


		/*
		 *  Figure out how much to rotate around that axis.
		 */
		ftl::Vec3f diff = v2 - v1;

		t = diff.norm()/ 2.0f*TRACKBALLSIZE*nScale;

		/*
		 * Avoid problems with out-of-control values...
		 */
		if (t > 1.0)
			t = 1.0;
		if (t < -1.0)
			t = -1.0;

		phi = 2.0f * asinf(t);

		axis_to_quat(axis,phi,result);

	}

	class lookatcamPlugHandler : public flowvr::portutils::MultiplexHandler
	{
	public:
		lookatcamPlugHandler( TransformstateService *pService, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::MultiplexHandler()
		, m_service(pService)
		, m_dLastUpdate(0)
		, m_state(MODE_NONE)
		, m_vPos(0,0,0)
		, m_zdowny(0)
		, m_bDown(false)
		, m_nBtMask(0)
		, m_zoomscale(1.0f)
		, m_vOffset(0,0,-2)
		, m_tdownx(-1)
		, m_tdowny(-1)
		, m_transscale(1.0f)
		, m_wheelstep(1.0f)
		, m_bShiftDown(false)
		, m_bAltDown(false)
		, m_bCtrlDown(false)
		, m_ntscaleFac(0.01f)
		, m_nrscaleFac(0.01f)
		, m_nwscaleFac(0.01f)
		, m_rotscale(1.0f)
		{
			try
			{
				m_zoomscale  = args("ZOOMSCALE").getValue<float>();
				m_transscale = args("TRANSSCALE").getValue<float>();
				m_wheelstep  = args("WHEELSTEP").getValue<float>();
				m_ntscaleFac = args("TSCALEFACTOR").getValue<float>();
				m_nrscaleFac = args("RSCALEFACTOR").getValue<float>();
				m_nwscaleFac = args("WSCALEFACTOR").getValue<float>();
				m_rotscale = args("ROTSCALE").getValue<float>();

				Parameter::ParVecN vOffset = args("ROTOFFSET").getValue<Parameter::ParVecN>();
				if(vOffset.size() < 3)
					throw ParameterException("ROTOFFSET", "not enough values in parameter");
				m_vOffset[0] = vOffset[0];
				m_vOffset[1] = vOffset[1];
				m_vOffset[2] = vOffset[2];

				m_Offset  = matrixTranslation(m_vOffset);
				m_OffsetI = matrixTranslation(-m_vOffset);
			}
			catch( std::exception &e )
			{
			}
		}

		virtual eState handleMultiplexing( const PortComm::MsgMap::SLOTS &m_in,
				   const PortComm::MsgMap::SLOTSTAMPS  &sl_in,
                PortComm::MsgMap::OSLOT  &m_out,
                PortComm::MsgMap::SLOTSTAMPS &sl_out,
                flowvr::Allocator & allocate)
		{
			if( m_in[0].data.getSize() > 0 )
				updateViewport( m_in[0] );

			if( m_in.size() > 2 and m_in[2].data.valid() )
				updateRotationOffset( m_in[2] );


			updateInternalValues();

			for (ChunkIterator it = chunkBegin(m_in[1]); it != chunkEnd(m_in[1]); ++it)
			{
				const Chunk* c = (const Chunk*) it; // Convert the iterator in a chunk base structure
				switch (c->type & 0x0F)
				{ // Look for each type of IO input

					case ChunkEvent::MOUSE: // In the case of a keyboard input
					{
						handleMouseEvent(((ChunkEventMouse *) c));
						break;
					}
				}
			}

			if( m_dLastUpdate != m_service->getLastUpdateTs() )
			{
				m_dLastUpdate = m_service->getLastUpdateTs();
				Mat4x4f m = m_service->getTransform();

				m_scene.addParam( ID_CAMERA, ChunkPrimParam::TRANSFORM, "", m );
			}

			if( m_scene.isDirty() )
				m_out[0] = m_scene.dump<flowvr::Allocator>( &allocate );

			return E_OK;
		}

		void handleMouseButton( ChunkEventMouse *mouse )
		{
			if( mouse->mouseWheelDir )
				handleMouseWheel( mouse );

			if( !mouse->released )
				return;

			// get the bits that are changed with respect to the last state update
			char changeSet = m_nBtMask ^ mouse->mouseKeys;

			if( mouse->released == 2 )
			{
				if( changeSet & (1<<BS_LEFT) )
					handleLeftMouseButtonUp( mouse );

				if( changeSet & (1<<BS_MIDDLE) )
					handleMiddleMouseButtonUp( mouse );

				if( changeSet & (1<<BS_RIGHT) )
					handleRightMouseButtonUp( mouse );
			}
			else if( mouse->released == 1 )
			{
				if( changeSet & (1<<BS_LEFT) )
					handleLeftMouseButtonDown( mouse );

				if( changeSet & (1<<BS_MIDDLE) )
					handleMiddleMouseButtonDown( mouse );

				if( changeSet & (1<<BS_RIGHT) )
					handleRightMouseButtonDown( mouse );
			}
		}

		////////////////////////////////////////////////////////////////////////
		// ZOOM MODE
		////////////////////////////////////////////////////////////////////////
		void handleRightMouseButtonDown( ChunkEventMouse *mouse )
		{
			m_nBtMask |= (1 << BS_RIGHT);
			enterZoomMode(mouse);
		}

		void handleRightMouseButtonUp( ChunkEventMouse *mouse )
		{
			m_nBtMask &= ~(1 << BS_RIGHT);
			exitZoomMode(mouse);
		}

		void enterZoomMode( ChunkEventMouse *mouse )
		{
			m_state = eState(m_state | MODE_ZOOM);
			// record the click-place
			m_zdowny = m_nrmVp[1];
		}

		void handleZoom( ChunkEventMouse *mouse )
		{
			// move along z-axis, depending on distance in y on first click
			float dist = m_zoomscale * (m_zdowny - m_nrmVp[1]);
			Vec3f trans = Vec3f(0,0,1) * dist;
			if( trans.norm() )
			{
				Mat4x4f mT = matrixTranslation( trans );
				m_service->applyTransform( mT, TransformstateService::MD_RIGHT_MULT, true );
				m_zdowny = m_nrmVp[1];
			}
		}

		void exitZoomMode( ChunkEventMouse *mouse )
		{
			m_state = eState(m_state & ~MODE_ZOOM);
			m_zdowny = 0;
		}

		void handleMouseWheel( ChunkEventMouse *mouse )
		{
			if( m_bShiftDown and !m_bAltDown and !m_bCtrlDown )
			{
				// adapt translation scale
				m_transscale += (m_ntscaleFac*m_transscale*mouse->mouseWheelDir);
				std::cout << "m_transscale set to: " << m_transscale << std::endl;
			}
			else
			if( !m_bShiftDown and !m_bAltDown and m_bCtrlDown )
			{
				m_wheelstep += (m_nwscaleFac*m_wheelstep*mouse->mouseWheelDir);
				std::cout << "m_wheelstep set to: " << m_wheelstep << std::endl;
			}
			else
			if( !m_bShiftDown and m_bAltDown and !m_bCtrlDown )
			{
				m_rotscale += (m_nrscaleFac*m_rotscale*mouse->mouseWheelDir);
				std::cout << "m_rotscale set to: " << m_rotscale << std::endl;
			}
			else
			{
				Vec3f trans = (Vec3f(0,0,1) * m_wheelstep) * mouse->mouseWheelDir;
				if( trans.norm() )
				{
					Mat4x4f mT = matrixTranslation( trans );
					m_service->applyTransform( mT, TransformstateService::MD_RIGHT_MULT, true );
				}
			}

		}

		//////////////////////////////////////////////////////////////////////////
		// END ZOOM
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// MODE ROTATE
		//////////////////////////////////////////////////////////////////////////

		void handleLeftMouseButtonDown( ChunkEventMouse *mouse )
		{
			m_nBtMask |= (1 << BS_LEFT);
			enterRotate();
		}


		void handleLeftMouseButtonUp( ChunkEventMouse *mouse )
		{
			m_nBtMask &= ~(1 << BS_LEFT);
			exitRotate();
		}

		void enterRotate()
		{
			m_state = eState(m_state | MODE_ROTATE);
		}

		void exitRotate()
		{
			m_state = eState(m_state & ~MODE_ROTATE);
		}

		void handleRotate( ChunkEventMouse *mouse )
		{
			Quat qRot;
			CalcRotation( m_nrmVp[2], m_nrmVp[3], m_nrmVp[0], m_nrmVp[1], qRot, m_rotscale*2.0f );

			// relative 'local' trackball rotation in qRot
			Mat4x4f m = matrixRotation(qRot);

			m_service->applyTransform( m_Offset * m * m_OffsetI, TransformstateService::MD_RIGHT_MULT, true );
		}

		//////////////////////////////////////////////////////////////////////////
		// TRANSLATE
		//////////////////////////////////////////////////////////////////////////

		void handleMiddleMouseButtonUp( ChunkEventMouse *mouse )
		{
			m_nBtMask &= ~(1 << BS_MIDDLE);
			exitTranslate( mouse );
		}


		void handleMiddleMouseButtonDown( ChunkEventMouse *mouse )
		{
			m_nBtMask |= (1 << BS_MIDDLE);
			enterTranslate(mouse);
		}


		void enterTranslate( ChunkEventMouse *mouse )
		{
			m_state = eState(m_state | MODE_TRANSLATE);

			m_tdownx = m_nrmVp[0];
			m_tdowny = m_nrmVp[1];
		}

		void exitTranslate( ChunkEventMouse *mouse )
		{
			m_state = eState(m_state & ~MODE_TRANSLATE);
			m_tdownx = m_tdowny = 0;
		}

		void handleTranslate( ChunkEventMouse *mouse )
		{
			float distx = m_transscale * (m_tdownx - m_nrmVp[0]);
			float disty = m_transscale * (m_tdowny - m_nrmVp[1]);

			Vec3f transx =  Vec3f(1,0,0) * distx;
			Vec3f transy =  Vec3f(0,1,0) * disty;
			Vec3f trans = transx + transy;

			if( trans.norm() )
			{
				Mat4x4f mT = matrixTranslation( trans );
				m_service->applyTransform( mT, TransformstateService::MD_RIGHT_MULT, true );
				m_tdownx = m_nrmVp[0];
				m_tdowny = m_nrmVp[1];
			}

		}

		//////////////////////////////////////////////////////////////////////////
		void handleMouseEvent( ChunkEventMouse *mouse )
		{
//			std::cout << "handleMouseEvent -- " << std::endl;

			if( mouse->mouseTranslation[0] or mouse->mouseTranslation[1] )
			{
				float x = mouse->mouseTranslation[0];
				float y = mouse->mouseTranslation[1];

				// save last state in 2/3
				m_nrmVp[2] = m_nrmVp[0];
				m_nrmVp[3] = m_nrmVp[1];

				if( m_vp.m_w and m_vp.m_h )
				{
					// update current state in 0/1
					m_nrmVp[0] =    -1 + ( 2  * ( 1 - float((m_vp.m_w-x))/float(m_vp.m_w) ) );
					m_nrmVp[1] =  -(-1 + ( 2  * ( 1 - float((m_vp.m_h-y))/float(m_vp.m_h) ) ));
				}
			}

			m_bShiftDown = mouse->modifier bitand ChunkEventMouse::MOD_SHIFT_DOWN;
			m_bAltDown   = mouse->modifier bitand ChunkEventMouse::MOD_ALT_DOWN;
			m_bCtrlDown  = mouse->modifier bitand ChunkEventMouse::MOD_CTRL_DOWN;


			handleMouseButton( mouse );

			if( m_state & MODE_ZOOM )
				handleZoom( mouse );

			if( m_state & MODE_ROTATE )
				handleRotate( mouse );

			if( m_state & MODE_TRANSLATE )
				handleTranslate( mouse );
		}

		void updateViewport( const Message &m )
		{
			m_vp.fromBuffer( m.data );
		}

		void updateRotationOffset( const Message &m )
		{
			m_vOffset = *m.data.getRead<Vec3f>();
			m_Offset  = matrixTranslation(m_vOffset);
			m_OffsetI = matrixTranslation(-m_vOffset);
		}

		void updateInternalValues()
		{
			if( m_dLastUpdate != m_service->getLastUpdateTs() )
			{
				m_dLastUpdate = m_service->getLastUpdateTs();
				Vec3f pos(0,0,0), la(0,0,-2), v(0,1,0);

				Mat4x4f m = m_service->getTransform();
				m_vPos     = transform( m, pos );
			}
		}

		TransformstateService *m_service;
		ChunkRenderWriter        m_scene;

		Vec3f m_vPos, m_vOffset;
		Mat4x4f m_Offset, m_OffsetI;

		flowvr::utils::microtime m_dLastUpdate;

		ViewportChgMsg m_vp;
		Vec4f m_nrmVp;
		float m_zdowny, m_zoomscale;
		float m_tdownx, m_tdowny, m_transscale, m_wheelstep, m_rotscale, m_ntscaleFac, m_nrscaleFac, m_nwscaleFac;

		bool m_bDown, m_bShiftDown, m_bAltDown, m_bCtrlDown;
		char m_nBtMask;

		enum eState
		{
			MODE_NONE = 0,
			MODE_ROTATE = 1,
			MODE_ZOOM = 2,
			MODE_TRANSLATE = 4
		} m_state;

		enum eBtState
		{
			BS_NONE=-1,
			BS_LEFT=0,
			BS_MIDDLE=1,
			BS_RIGHT=2
		};
	};
}


DEFIMP_PORTPLUG( lookatcamPlugHandler, TransformstateService, lookatcamPlug )

