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
	args["WHEELSTEP"]  = Parameter( "1", "Translation value for 1 wheel click", Parameter::P_NUMBER);
}

namespace
{

	class halfSphereCamPlugHandler : public flowvr::portutils::MultiplexHandler
	{
	public:
		halfSphereCamPlugHandler( TransformstateService *pService, const flowvr::portutils::ARGS &args )
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
		, m_xspin(0)
		, m_yspin(0)
		, m_xdown(0)
		, m_ydown(0)
		{
			try
			{
				m_zoomscale  = args("ZOOMSCALE").getValue<float>();
				m_wheelstep  = args("WHEELSTEP").getValue<float>();
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
		}

		void handleRightMouseButtonUp( ChunkEventMouse *mouse )
		{
			m_nBtMask &= ~(1 << BS_RIGHT);
		}


		void handleMouseWheel( ChunkEventMouse *mouse )
		{
			Vec3f trans = (Vec3f(0,0,1) * m_wheelstep) * mouse->mouseWheelDir;
			if( trans.norm() )
			{
				Mat4x4f mT = matrixTranslation( trans );
				m_service->applyTransform( mT, TransformstateService::MD_RIGHT_MULT, true );
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
			m_xdown = m_nrmVp[0];
			m_ydown = m_nrmVp[1];
		}

		void exitRotate()
		{
			m_state = eState(m_state & ~MODE_ROTATE);
			m_xdown = m_ydown = 0;
		}

		void handleRotate( ChunkEventMouse *mouse )
		{
			float ydist = (m_xdown - m_nrmVp[0])/2.0f;
			float xdist = (m_ydown - m_nrmVp[1])/2.0f;

			Mat4x4f m;

			m = matrixTransform( Vec3f(0,0,0), float(RAD2DEG( ydist )), Vec3f(0,1,0) );
			m_service->applyTransform( m, TransformstateService::MD_LEFT_MULT, false );

			float xspin_deg = RAD2DEG( m_xspin + xdist );

			if( xspin_deg >= 90.0f )
				xdist = 0;
			else
				m_xspin += xdist;

			if( xspin_deg < 0.0f )
				xdist = 0;
			else
				m_xspin += xdist;

			m_Offset  = matrixTransform( Vec3f(0,0,-2) );
			m_OffsetI = matrixTransform( Vec3f(0,0, 2) );

			m = m_Offset * matrixTransform( Vec3f(0,0,0), float(RAD2DEG(xdist)), Vec3f(1,0,0) ) * m_OffsetI;
			m_service->applyTransform( m, TransformstateService::MD_RIGHT_MULT, true );

			m_xdown = m_nrmVp[0];
			m_ydown = m_nrmVp[1];
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
//			float distx = m_transscale * (m_tdownx - m_nrmVp[0]);
//			float disty = m_transscale * (m_tdowny - m_nrmVp[1]);
//
//			Vec3f transx =  Vec3f(1,0,0) * distx;
//			Vec3f transy =  Vec3f(0,1,0) * disty;
//			Vec3f trans = transx + transy;
//
//			if( trans.norm() )
//			{
//				Mat4x4f mT = matrixTranslation( trans );
//				m_service->applyTransform( mT, TransformstateService::MD_RIGHT_MULT, true );
//				m_tdownx = m_nrmVp[0];
//				m_tdowny = m_nrmVp[1];
//			}
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

			handleMouseButton( mouse );

//			if( m_state & MODE_ZOOM )
//				handleZoom( mouse );

			if( m_state & MODE_ROTATE )
				handleRotate( mouse );

			if( m_state & MODE_TRANSLATE )
				handleTranslate( mouse );
		}

		void updateViewport( const Message &m )
		{
			m_vp.fromBuffer( m.data );
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
		float m_tdownx, m_tdowny, m_transscale, m_wheelstep;

		float m_xspin, m_yspin, m_xdown, m_ydown;

		bool m_bDown;
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


DEFIMP_PORTPLUG( halfSphereCamPlugHandler, TransformstateService, halfSphereCamPlug )

