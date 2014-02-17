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
* File: input.cpp                                              *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include "flowvr/render/balzac/servicelayer/input.h"
#include "flowvr/render/balzac/servicelayer/glutdata.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#if defined(BALZAC_USE_FREEGLUT)
 #include <GL/freeglut_ext.h>
#else
#endif // BALZAC_USE_FREEGLUT

#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <map>

namespace
{
#if !defined(BALZAC_USE_FREEGLUT)
	std::map<int, flowvr::render::balzac::Input*> S_winMaps;
#endif

	flowvr::render::balzac::Input *getCurrentInput()
	{
		int nWinId = glutGetWindow();
		if(nWinId == -1)
			return NULL;

#if defined(BALZAC_USE_FREEGLUT)
		void *pData = glutGetWindowData();
		return reinterpret_cast<flowvr::render::balzac::GlutData*>(pData)->m_input;
#else
		std::map<int, flowvr::render::balzac::Input*>::const_iterator cit = S_winMaps.find(nWinId);
		if( cit == S_winMaps.end() )
			return NULL;

		return (*cit).second;;
#endif
	}

	void key_func( unsigned char cKey, int x, int y )
	{
		flowvr::render::balzac::Input *pInput = getCurrentInput();
		if(pInput == NULL)
			return;

		pInput->onKeyPress( cKey, x, y );
	}

	void keyup_func( unsigned char cKey, int x, int y )
	{
		flowvr::render::balzac::Input *pInput = getCurrentInput();
		if(pInput == NULL)
			return;

		pInput->onKeyRelease( cKey, x, y );
	}

	void special_func( int nSpecial, int x, int y )
	{
		flowvr::render::balzac::Input *pInput = getCurrentInput();
		if(pInput == NULL)
			return;

		pInput->onSpecialKeyPress(nSpecial, x, y );
	}

	void specialup_func( int nSpecial, int x, int y )
	{
		flowvr::render::balzac::Input *pInput = getCurrentInput();
		if(pInput == NULL)
			return;

		pInput->onSpecialKeyRelease(nSpecial, x, y );
	}

	void mouse_func( int button, int nState, int x, int y )
	{
		flowvr::render::balzac::Input *pInput = getCurrentInput();
		if(pInput == NULL)
			return;

		if( button < 3 )
			pInput->onMouseButton( button, nState, x, y );
		else
			pInput->onWheel(button, (button==3 ? GLUT_UP:GLUT_DOWN), x, y );

	}

	void motion_func( int x, int y )
	{
		flowvr::render::balzac::Input *pInput = getCurrentInput();
		if(pInput == NULL)
			return;

		pInput->onMouseMotion( x, y );
	}

#if defined(BALZAC_USE_FREEGLUT)
	void wheel_func( int x1, int x2, int x3, int x4 )
	{
		flowvr::render::balzac::Input *pInput = getCurrentInput();
		if(pInput == NULL)
		{
			return;
		}
		pInput->onWheel(x1,x2,x3,x4);
	}
#endif // BALZAC_USE_FREEGLUT


} // anonymous namespace end

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			Input::Input()
			: m_nWheel(0)
			, m_nButton(0)
			, m_bActivated(false)
			{
				glutKeyboardFunc(key_func);
				glutKeyboardUpFunc(keyup_func);

				glutSpecialFunc(special_func);
				glutSpecialUpFunc(specialup_func);
				glutMouseFunc(mouse_func);
				glutMotionFunc(motion_func);
				glutPassiveMotionFunc(motion_func);
#if defined(BALZAC_USE_FREEGLUT)
				glutMouseWheelFunc(wheel_func);
#else
				S_winMaps[ glutGetWindow() ] = this;
#endif
			}

			Input::~Input()
			{
				glutKeyboardFunc(NULL);
				glutSpecialFunc(NULL);
				glutMouseFunc(NULL);
				glutMotionFunc(NULL);
				glutPassiveMotionFunc(NULL);
#if defined(BALZAC_USE_FREEGLUT)
				glutMouseWheelFunc(NULL);
#else
				std::map<int, Input*>::iterator it = S_winMaps.find( glutGetWindow() );
				if( it != S_winMaps.end() )
					S_winMaps.erase(it);
#endif
			}


			class _cHandle : public std::unary_function<void, Input::IEventSink*>
			{
			public:
				_cHandle( Input::IEventSink::Event *pEvent )
				: m_event(pEvent)
				{}

				void operator()( Input::IEventSink *sink )
				{
					sink->handleEvent( m_event );
				}

				Input::IEventSink::Event *m_event;
			};

			bool Input::onKeyPress( unsigned char nKey, int x, int y )
			{
				if(m_bActivated)
				{
					IEventSink::Event e(nKey, true, false, x, y, glutGetModifiers() );
					std::for_each( m_sinks.begin(), m_sinks.end(), _cHandle(&e) );
				}
				return true;
			}

			bool Input::onKeyRelease( unsigned char nKey, int x, int y )
			{
				if(m_bActivated)
				{
					IEventSink::Event e(nKey, false, false, x, y, glutGetModifiers() );
					std::for_each( m_sinks.begin(), m_sinks.end(), _cHandle(&e) );
				}
				return true;
			}

			bool Input::onSpecialKeyPress( unsigned char nKey, int x, int y )
			{
				if(m_bActivated)
				{
					IEventSink::Event e( nKey, true, true, x, y, glutGetModifiers() );
					std::for_each( m_sinks.begin(), m_sinks.end(), _cHandle(&e) );
				}
				return true;
			}

			bool Input::onSpecialKeyRelease( unsigned char nKey, int x, int y )
			{
				if(m_bActivated)
				{
					IEventSink::Event e( nKey, false, true, x, y, glutGetModifiers() );
					std::for_each( m_sinks.begin(), m_sinks.end(), _cHandle(&e) );
				}
				return true;
			}


			bool Input::onMouseButton( int button, int state, int x, int y )
			{
				if(!m_bActivated)
					return true;

				IEventSink::Event e( button, (state == GLUT_DOWN ? true:false), x, y, glutGetModifiers() );
				std::for_each( m_sinks.begin(), m_sinks.end(), _cHandle(&e) );

				return true;
			}

			bool Input::onMouseMotion( int x, int y )
			{
				if(!m_bActivated)
					return true;

				IEventSink::Event e( x, y, 0 );
				std::for_each( m_sinks.begin(), m_sinks.end(), _cHandle(&e) );

				return true;
			}

			bool Input::onWheel( int nBt, int nDir, int x, int y)
			{
				if(!m_bActivated)
					return true;

				IEventSink::Event e(nBt, (nDir==GLUT_UP?-1:+1), x, y, glutGetModifiers() );
				std::for_each( m_sinks.begin(), m_sinks.end(), _cHandle(&e) );

				return true;
			}

			bool Input::getActivated() const
			{
				return m_bActivated;
			}

			void Input::setActivated( bool bActive )
			{
				m_bActivated = bActive;
			}

			void Input::subscribe( IEventSink *sink )
			{
				m_sinks.push_back(sink);
			}

			void Input::unsubscribe( IEventSink *sink )
			{
				std::vector<IEventSink*>::iterator q =
						std::remove(m_sinks.begin(), m_sinks.end(), sink );

				m_sinks.erase( q, m_sinks.end());
			}

		}
	}
}
