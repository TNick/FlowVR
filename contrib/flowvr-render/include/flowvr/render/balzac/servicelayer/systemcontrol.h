/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                   Balzac FlowVR render frontend                 *
*                                                                 *
*-----------------------------------------------------------------*
 * COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
* ALL RIGHTS RESERVED.	                                          *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING file for further information.                           *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Thomas Arcila,                                               *
*    Jean-Denis Lesage,                                           *
*    Clement Menier,                                              *
*    Ingo Assenmacher,                                            *
*    Bruno Raffin                                                 *
*                                                                 *
*******************************************************************
*                                                                 *
*  Contact :                                                      *
*                                                                 *
******************************************************************/

#ifndef BALZACSYSTEMCONTROL_H_
#define BALZACSYSTEMCONTROL_H_

#include <string>

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			class ServiceLayer;

			class SystemControl
			{
			public:
				SystemControl( ServiceLayer& );
				~SystemControl();

				// ####################################################################
				// PROCESSING
				// ####################################################################
				void quitLoop();


				// ####################################################################
				// DISPLAY RELATED
				// ####################################################################
				int getCurrentCursor() const;
				void setCurrentCursor(int);

				bool getIsFullscreen() const;
				void setIsFullscreen( bool bFullscreen );

				int getLocalDisplayWidth() const;
				void setLocalDisplayWidth( int );

				void setLocalDisplaySize(int w, int h);

				int getLocalDisplayHeight() const;
				void setLocalDisplayHeight( int );

				int getLocalDisplayX() const;
				void setLocalDisplayX(int);

				int getLocalDisplayY() const;
				void setLocalDisplayY(int);

				std::string getLocalDisplayTitle() const;
				void setLocalDisplayTitle( const std::string & );

			private:
				ServiceLayer &m_app;
			};

		} // namespace balzac
	} // namespace render
} // namespace flowvr


#endif // SYSTEMCONTROL_H_
