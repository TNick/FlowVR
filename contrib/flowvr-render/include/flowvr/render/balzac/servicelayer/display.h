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

#ifndef BALZACDISPLAY_H_
#define BALZACDISPLAY_H_


#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>


#include <vector>
#include <string>


#include <flowvr/moduleapi.h>
#include <flowvr/render/cg/cgengine.h>




namespace flowvr
{
	namespace portutils
	{
		class PortComm;
	}
}

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			class Input;

			class Display
			{
			public:
				Display(flowvr::render::cg::CgEngine *pWorldEngine,
							  flowvr::render::cg::CgEngine *pOverlayEngine );
				~Display();

				// APPLICATION RENDER STATE
				bool preDisplay();
				bool postDisplay();
				bool onReshape(int newWidth, int newHeight);
				bool onRepaint();
				bool onIdle();
				void cleanGlutState();

				// WINDOW STATE
				bool getPosition( int &winx, int &winy, int &winw, int &winh) const;
				bool setPosition( int winx, int winy, int winw, int winh);

				bool getSize( int &winw, int &winh ) const;
				bool setSize( int winw, int winh );

				double getViewportUpdateTs() const;

				int  getWidth() const;
				bool setWidth( int width );

				int  getHeight() const;
				bool setHeight( int height );

				int  getX() const;
				bool setX( int x );

				int  getY() const;
				bool setY( int y );

				bool getFullscreen() const;
				bool setFullscreen( bool bFullscreen );

				int getCursor() const;
				void setCursor(int);

				int  getId() const;

				std::string getTitle() const;
				bool setTitle( const std::string &strTitle );

				bool makeCurrent() const;

				Input *getInput() const;
				flowvr::render::cg::CgEngine *getWorldCgEngine() const;

				void setDebugMode( int idx, int mode );
				int  getDebugMode( int idx ) const;

				void setCommunicator( flowvr::portutils::PortComm *pComm );
                void getScreenshot();
				flowvr::portutils::PortComm *getCommunicator() const;

				//***********************
				// STATISTICS
				//***********************

				double getFrameRate() const;
				long   getNumberOfFrames() const;


			private:

				void displayBgImage();

				int  m_id;
				int  m_oldWidth,
					 m_oldHeight;
				bool m_bFullScreen;;
				std::string m_title;

				Input               *m_input;
				flowvr::portutils::PortComm *m_Comm;

				flowvr::ModuleAPI                 *m_swaplock;
				flowvr::render::cg::CgEngine      *m_WorldcgEngine;

				double m_lstVpUpdate;

				double m_nFrameRate, m_nPre, m_nPost;
				int    m_nTimeCap,
				       m_nNumTimeCaps;
				long   m_nNumFrames;
			};
		} // namespace balzac
	} // namespace render
} // namespace flowvr


#endif // DISPLAY_H_
