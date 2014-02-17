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
* File: systemcontrol.cpp                                         *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include "flowvr/render/balzac/servicelayer/systemcontrol.h"
#include "flowvr/render/balzac/servicelayer/servicelayer.h"
#include "flowvr/render/balzac/servicelayer/display.h"

#include <flowvr/portutils/portcomm.h>


namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			SystemControl::SystemControl( ServiceLayer &app )
			: m_app(app)
			{

			}

			SystemControl::~SystemControl()
			{

			}


			void SystemControl::quitLoop()
			{
//				m_app.getCommunicator()->setDefaultReturn( flowvr::portutils::PortComm::RET_FAIL );
			}


			int SystemControl::getCurrentCursor() const
			{
				return m_app.getDisplay()->getCursor();
			}

			void SystemControl::setCurrentCursor(int cs)
			{
				m_app.getDisplay()->setCursor(cs);
			}

			bool SystemControl::getIsFullscreen() const
			{
				return m_app.getDisplay()->getFullscreen();
			}

			void SystemControl::setIsFullscreen( bool bFullscreen )
			{
				m_app.getDisplay()->setFullscreen(bFullscreen);
			}

			int SystemControl::getLocalDisplayWidth() const
			{
				return m_app.getDisplay()->getWidth();
			}

			void SystemControl::setLocalDisplayWidth( int w )
			{
				m_app.getDisplay()->setWidth(w);
			}

			int SystemControl::getLocalDisplayHeight() const
			{
				return m_app.getDisplay()->getHeight();
			}

			void SystemControl::setLocalDisplayHeight( int h)
			{
				m_app.getDisplay()->setHeight(h);
			}

			int SystemControl::getLocalDisplayX() const
			{
				return m_app.getDisplay()->getX();
			}

			void SystemControl::setLocalDisplayX(int x)
			{
				m_app.getDisplay()->setX(x);
			}

			void SystemControl::setLocalDisplaySize(int w, int h)
			{
				m_app.getDisplay()->setSize(w,h);
			}

			int SystemControl::getLocalDisplayY() const
			{
				return m_app.getDisplay()->getY();
			}

			void SystemControl::setLocalDisplayY(int y)
			{
				m_app.getDisplay()->setY(y);
			}

			std::string SystemControl::getLocalDisplayTitle() const
			{
				return m_app.getDisplay()->getTitle();
			}

			void SystemControl::setLocalDisplayTitle( const std::string &strTitle )
			{
				m_app.getDisplay()->setTitle(strTitle);
			}

		} // namespace balzac
	} // namespace render
} // namespace flowvr

