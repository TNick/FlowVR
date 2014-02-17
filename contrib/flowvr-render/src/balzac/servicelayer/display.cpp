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
* File: display.cpp                                              *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include <flowvr/render/balzac/servicelayer/display.h>

#include <flowvr/render/balzac/servicelayer/glutdata.h>
#include <flowvr/render/balzac/servicelayer/input.h>

#include <flowvr/portutils/portcomm.h>
#include <flowvr/utils/timing.h>



#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#if BALZAC_USE_FREEGLUT
#include <GL/freeglut_ext.h>
#endif
#endif


#include <iostream>
#include <map>

namespace
{
#if !defined(BALZAC_USE_FREEGLUT)
	using namespace std;
	using namespace flowvr::render::balzac;


	map<int, GlutData*> S_dispMap;
#endif


	flowvr::render::balzac::Display *getCurrentDisplay()
	{
		int nWindow = glutGetWindow(); // find the current window
		if( nWindow == -1 )
			return NULL;
#if BALZAC_USE_FREEGLUT
		void *pData = glutGetWindowData();
		return reinterpret_cast<flowvr::render::balzac::GlutData*>(pData)->m_display;
#else
		/// @todo implement this function on glut? (can it be done?)
		map<int,GlutData*>::const_iterator cit = S_dispMap.find( nWindow );
		if( cit == S_dispMap.end() )
			return NULL;
		return (*cit).second->m_display;
#endif
	}

	void display_func( )
	{
		flowvr::render::balzac::Display *pDisp = getCurrentDisplay();
		if(pDisp == NULL)
			return;


        //pDisp->setTitle("TESTE");
		pDisp->preDisplay();
		pDisp->onRepaint();
		pDisp->postDisplay();
	}

	void reshape_func(int width, int height)
	{
		flowvr::render::balzac::Display *pDisp = getCurrentDisplay();
		if(pDisp == NULL)
			return;

		pDisp->onReshape(width, height);
	}

	void idle_func()
	{
		flowvr::render::balzac::Display *pDisp = getCurrentDisplay();
		if(pDisp == NULL)
			return;
		if(pDisp->onIdle() == false)
		{
#if BALZAC_USE_FREEGLUT
			glutLeaveMainLoop();
			// call cleanGlutState() here, as otherwise
			// once the inner loop is left, all glut-state
			// will believe, it is not initialized, yet
			// leading to errors.
			pDisp->cleanGlutState();
#else
			pDisp->cleanGlutState();
			exit(-1);
#endif
		}
	}


} // anonymous namespace


namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			Display::Display(flowvr::render::cg::CgEngine *pEngine,
									 flowvr::render::cg::CgEngine *pOverlays)
			:  m_swaplock(NULL)
			 , m_id(-1)
			 , m_bFullScreen(false)
			 , m_input(NULL)
			 , m_WorldcgEngine(pEngine)
//			 , m_cgOverlays(pOverlays)
			 , m_Comm(NULL)
			 , m_oldWidth(640)
			 , m_oldHeight(480)
			 , m_nFrameRate(0)
			 , m_nTimeCap(0)
			 , m_nPre(0)
			 , m_nPost(0)
			 , m_nNumFrames(0)
			 , m_nNumTimeCaps(100)
			{
				glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

				glutInitWindowPosition(10,10);
				glutInitWindowSize(640,480);
				m_id = glutCreateWindow("<>");

				makeCurrent();

				m_input = new Input;

				glutReshapeFunc(reshape_func);
				glutIdleFunc(idle_func);
				glutDisplayFunc(display_func);

				// clear all buffers
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glutSwapBuffers();
				glClear(GL_COLOR_BUFFER_BIT);
				glutSwapBuffers();

				glViewport(0, 0, 640, 480);
				glEnable(GL_DEPTH_TEST);
//				glEnable(GL_RESCALE_NORMAL);
				// GL_LIGHT1 is a directional light
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT1);
				const GLfloat light_ambient1[4] =	{ 0.2, 0.2, 0.2, 1.0 };
				const GLfloat light_specular1[4] =	{ 0.3, 0.3, 0.3, 1.0 };
				const GLfloat light_diffuse1[4] =	{ 0.5, 0.5, 0.5, 1.0 };

				glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
				glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
				glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
				// GL_LIGHT1 follows the camera
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				const GLfloat pos1[4] =  { 0.0, 0.0, 0.0, 1.0 };
				glLightfv(GL_LIGHT1, GL_POSITION, pos1);
				glPopMatrix();

				// init cg engine
				m_WorldcgEngine->init();

				// we set a display, so assign the current time stamp to the update variable
				m_lstVpUpdate = flowvr::utils::getNtpTimeStamp();

#ifdef BALZAC_USE_FREEGLUT
				glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
				glutSetWindowData( new GlutData( this, m_input ) );
				std::cout << "Balzac: BALZAC_USE_FREEGLUT" << std::endl;
#else
				S_dispMap[ glutGetWindow() ] = new GlutData(this, m_input);
#endif
			}

			Display::~Display()
			{
				delete m_WorldcgEngine;
			}

			void Display::cleanGlutState()
			{
				makeCurrent();
#if BALZAC_USE_FREEGLUT
				GlutData *pData = reinterpret_cast<GlutData*>(glutGetWindowData());
				delete pData;
#else
				std::map<int,GlutData*>::iterator it = S_dispMap.find(glutGetWindow());
				delete (*it).second;

				S_dispMap.erase( it );
#endif
				glutReshapeFunc(NULL);
				glutIdleFunc(NULL);

				delete m_input;

				glutHideWindow();
				glutDestroyWindow(m_id);

				m_id = 0;
				m_input = NULL;
			}


			void Display::setDebugMode( int idx, int nDebug )
			{
				if( getDebugMode( idx ) != nDebug )
				{
					if( nDebug >= flowvr::render::cg::CgEngine::NBDEBUGMODES )
						nDebug = flowvr::render::cg::CgEngine::DEBUG_STD; // reset to 0

					nDebug = std::max<int>( flowvr::render::cg::CgEngine::DEBUG_STD, nDebug );
					getWorldCgEngine()->debugMode = nDebug;
				}
			}


            void Display::getScreenshot()
            {
            
                unsigned int w, h; /* width and hight of the glut window */
                size_t wsize;
                
                w = getWidth();
                h = getHeight();
                wsize = w * h * 3;
                
                /* allocates a RGB buffer */                       
                GLubyte *pixels = (GLubyte *) malloc(wsize);
                
                /* Reads the RGB buffer from OpenGL and copies to the pixel array */
                glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
                
                /* The buffer is written upside down, so, we have to copy and turn it */
                
                GLubyte *pixels2 = (GLubyte *) malloc(wsize);
                
                for(int y = 0; y < h; y++)
                    for(int x = 0; x < w * 3; x++)
                        pixels2[((h - 1) - y) * w * 3 + x] = pixels[y * 3 * w + x];
                
                free(pixels);
                
                /* if the array is valid, write it to a file */
                if (pixels2)
                {
                    DIR *dirp;
                    struct dirent *dp;
                    struct stat buf;
                    int i_num = 0;
                    dirp = opendir(".");
                    std::vector <int> result;
                
                    /* Look at each entry in turn */
                    while ((dp = readdir(dirp)) != NULL)
                    {
                        if (stat(dp->d_name, &buf) == -1)
                            perror("stat\n");
                        
                        if (S_ISREG(buf.st_mode))
                            if (sscanf (dp->d_name, "balzac-screenshot-%d.ppm", &i_num))
                                result.push_back(i_num);
                    }
                    
                    (void) closedir(dirp);
                    
                    
                    char filename[64];
                    
                    if(result.size() > 0) //if there's is something in the vector, increases the counter
                    {
                        std::sort(result.begin(), result.end());
                        sprintf(filename, "balzac-screenshot-%d.ppm", result.back()+1);
                    
                    }
                    else
                        sprintf(filename, "balzac-screenshot-%d.ppm", 0);
                    
                    std::cout << filename << std::endl;    
                    
                    FILE *fp = fopen(filename, "w"); 
                    
                    (void)fprintf(fp, "P3\n%d %d\n255\n", w, h);
                    
                    for (int i = 0; i < wsize; i++)
                        fprintf(fp,"%d\n", *(pixels2+i));
                    
                    (void)fclose(fp);
                    free(pixels2);
                }
            }


			int Display::getDebugMode( int idx ) const
			{
				return getWorldCgEngine()->debugMode;
			}


			bool Display::preDisplay()
			{
				return true;
			}
            
			bool Display::postDisplay()
			{
				glutSwapBuffers();
				return true;
			}

			double Display::getViewportUpdateTs() const
			{
				return m_lstVpUpdate;
			}

			bool Display::onReshape(int newWidth, int newHeight)
			{
				glViewport(0, 0, newWidth, newHeight);

				glutPostRedisplay();
				m_lstVpUpdate = flowvr::utils::getMicroStamp();
				return true;
			}


			bool Display::onRepaint()
			{
				bool bCapPre = false;
				bool bCapPost = false;

				if(m_nTimeCap == 0)
				{
					bCapPre = true;
				}
				else if( m_nTimeCap == m_nNumTimeCaps )
				{
					bCapPost = true;
					m_nTimeCap = 0;
				}

				if( bCapPre )
					m_nPre = flowvr::utils::getNtpTimeStamp();

				// ########################################################################
				// render scene
				// ########################################################################

				m_WorldcgEngine->render( float(getWidth()) / float(getHeight()) );

				// ########################################################################
				// ########################################################################


				if( bCapPost )
				{
					m_nPost      = flowvr::utils::getNtpTimeStamp();
					m_nFrameRate = 1.0 / ( (m_nPost-m_nPre) / m_nNumTimeCaps );

					if( m_nNumTimeCaps < m_nFrameRate / 4.0 )
					{
						++m_nNumTimeCaps;
					}
					else
					{
						m_nNumTimeCaps = std::max<int>(2, m_nNumTimeCaps-1);
					}
				}
				else
					++m_nTimeCap;

				++m_nNumFrames;
				return true;
			}

			bool Display::onIdle()
			{
				if(m_Comm)
				{
					switch(m_Comm->process())
					{
						case flowvr::portutils::PortComm::RET_DIRTY:
							// dirty layout: recalc bb
							m_WorldcgEngine->updateBBox();
							// force repaint
							glutPostRedisplay();
							// no break here...
						case flowvr::portutils::PortComm::RET_CLEAN:
							return true;
						case flowvr::portutils::PortComm::RET_FAIL:
						default:
							return false;
					}
				}

				return true;
			}

			bool Display::getPosition( int &winx, int &winy, int &winw, int &winh) const
			{
				winx = getX();
				winy = getY();
				winw = getWidth();
				winh = getHeight();
				return true;
			}

			bool Display::setPosition( int winx, int winy, int winw, int winh)
			{
				makeCurrent();
				glutPositionWindow( winx, winy );
				glutReshapeWindow( winw, winh );
				if(m_bFullScreen) // reset full-screen flag, when set
					m_bFullScreen = false;
				return true;
			}

			bool Display::getSize( int &winw, int &winh ) const
			{
				winw = getWidth();
				winh = getHeight();
				return true;
			}

			bool Display::setSize( int winw, int winh )
			{
				makeCurrent();
				glutReshapeWindow( winw, winh );
				if(m_bFullScreen) // reset full-screen flag, when set
					m_bFullScreen = false;
				return true;
			}

			int  Display::getWidth() const
			{
				makeCurrent();
				return glutGet( GLUT_WINDOW_WIDTH );
			}

			bool Display::setWidth( int width )
			{
				makeCurrent();
				glutReshapeWindow( width, getHeight() );
				if(m_bFullScreen) // reset full-screen flag, when set
					m_bFullScreen = false;
				return false;
			}


			int  Display::getHeight() const
			{
				makeCurrent();
				return glutGet(GLUT_WINDOW_HEIGHT);
			}

			bool Display::setHeight( int height )
			{
				makeCurrent();
				glutReshapeWindow( getWidth(), height );
				if(m_bFullScreen) // reset full-screen flag, when set
					m_bFullScreen = false;
				return true;
			}

			int  Display::getX() const
			{
				makeCurrent();
				return glutGet(GLUT_WINDOW_X);
			}

			bool Display::setX( int x )
			{
				makeCurrent();
				glutPositionWindow( x, getY() );
				if(m_bFullScreen) // reset full-screen flag, when set
					m_bFullScreen = false;
				return true;
			}

			int  Display::getY() const
			{
				makeCurrent();
				return glutGet(GLUT_WINDOW_Y);
			}

			bool Display::setY( int y )
			{
				makeCurrent();
				glutPositionWindow( getX(), y );
				if(m_bFullScreen) // reset full-screen flag, when set
					m_bFullScreen = false;
				return true;
			}

			bool Display::getFullscreen() const
			{
				return m_bFullScreen;
			}

			std::string Display::getTitle() const
			{
				return m_title;
			}

			bool Display::setTitle( const std::string &strTitle )
			{
				glutSetWindowTitle( strTitle.c_str() );
				glutSetIconTitle( strTitle.c_str() );
				m_title = strTitle;
				return true;
			}

			bool Display::setFullscreen( bool bFullscreen )
			{
				makeCurrent();
				if( m_bFullScreen && !bFullscreen )
				{
					// we are in fullscreen and are supposed to switch back
					// so we do...
					glutReshapeWindow(m_oldWidth,m_oldHeight);
					m_bFullScreen = false;
				}
				else if( bFullscreen ) // we are not in fullscreen, but are supposed to do so...
				{
					getSize(m_oldWidth, m_oldHeight);

					glutFullScreen();
					m_bFullScreen = true;
				}
				return true;
			}

			int  Display::getId() const
			{
				return m_id;
			}

			int Display::getCursor() const
			{
				return glutGet(GLUT_WINDOW_CURSOR);
			}

			void Display::setCursor(int cs)
			{
				glutSetCursor(cs);
			}



			bool Display::makeCurrent() const
			{
				glutSetWindow( m_id );
				return (glutGetWindow() == m_id);
			}

			Input *Display::getInput() const
			{
				return m_input;
			}

			flowvr::render::cg::CgEngine *Display::getWorldCgEngine() const
			{
				return m_WorldcgEngine;
			}

			void Display::setCommunicator( flowvr::portutils::PortComm *pComm )
			{
				m_Comm = pComm;
				if(m_Comm)
					m_Comm->setDefaultReturn( flowvr::portutils::PortComm::RET_DIRTY );
			}

			flowvr::portutils::PortComm *Display::getCommunicator() const
			{
				return m_Comm;
			}

			double Display::getFrameRate() const
			{
				return m_nFrameRate;
			}

			long   Display::getNumberOfFrames() const
			{
				return m_nNumFrames;
			}

		} // namespace balzac
	} // namespace render
} // namespace flowvr
