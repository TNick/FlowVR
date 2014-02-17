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
* File: defaultkeyactions.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>


#include <flowvr/render/balzac/servicelayer/servicelayer.h>
#include <flowvr/render/balzac/servicelayer/input.h>
#include <flowvr/render/balzac/servicelayer/display.h>
#include <flowvr/render/balzac/servicelayer/systemcontrol.h>

#include <flowvr/render/cg/cgengine.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

namespace
{
	using namespace flowvr::render::balzac;

	class defaultkeyactionsPlugHandler : public flowvr::portutils::NoPortHandler
	{
		class defKeys : public Input::IEventSink
		{
		public:
			defKeys(ServiceLayer *parent)
			: Input::IEventSink()
			, m_parent(parent) {	}

			bool handleEvent( Input::IEventSink::Event *p )
			{
				switch(p->m_type)
				{
				case Input::IEventSink::Event::TP_KEYBOARD_PRESS:
				case Input::IEventSink::Event::TP_KEYBOARD_RELEASE:
				case Input::IEventSink::Event::TP_KEYBOARD_SPECIAL_PRESS:
				case Input::IEventSink::Event::TP_KEYBOARD_SPECIAL_RELEASE:
					return handleKeyboard(p);
				default:
					break;
				}
				return true;
			}


			bool handleKeyboard( Event *p )
			{
				switch( p->m_type )
				{
					case Input::IEventSink::Event::TP_KEYBOARD_PRESS:
					case Input::IEventSink::Event::TP_KEYBOARD_SPECIAL_PRESS:
					{
						return handleKeyPress(p->nKey, p->modifier);
					}
					case Input::IEventSink::Event::TP_KEYBOARD_RELEASE:
					case Input::IEventSink::Event::TP_KEYBOARD_SPECIAL_RELEASE:
					{
						return handleKeyRelease(p->nKey, p->modifier);
					}
					default:
						return false;
				}

				return true;
			}

			bool handleKeyPress( unsigned char cKey, int mod )
			{
				switch(cKey)
				{
					case 'c':
						m_parent->getControl()->setCurrentCursor( GLUT_CURSOR_NONE );
						break;
					case 'C':
						m_parent->getControl()->setCurrentCursor( GLUT_CURSOR_INHERIT );
						break;
					case 'D':
					{
						m_parent->getDisplay()->setDebugMode( 0, m_parent->getDisplay()->getDebugMode(0)+1 );
						break;
					}
					case 'B':
					{
						m_parent->getDisplay()->getWorldCgEngine()->updateBBox();
						std::cout << "scene bounds: ";
						std::cout << m_parent->getDisplay()->getWorldCgEngine()->sceneBBox << std::endl;
						break;
					}
					case 'T':
					{
						m_parent->getDisplay()->getWorldCgEngine()->printTargetGroups();
						break;
					}
					case 't':
					{
						m_parent->getDisplay()->getWorldCgEngine()->printTextures();
						break;
					}
					case '$':
					{
						// reload shaders
						m_parent->getDisplay()->getWorldCgEngine()->reloadShaders();
						break;
					}
                                        case 'f':
                                        {
                                                /* 
                                                 * takes a screenshot and saves into a files when pressing 'f'
                                                 * over balzac window (OpenGL/GLUT context). Writes into a PPM
                                                 * file.
                                                 */
                                                 
                                                 m_parent->getDisplay()->getScreenshot();
                        
                                                break;
                                        }

//					case 'V':
//					{
//						ftl::Vec3f pos, yup, lat(0,0,-2);
//						m_parent->getDisplay()->getWorldCgEngine()->showViewPose(pos, yup, lat);
//						std::cout << "pos: " << toString(pos) << std::endl
//								  << "yup: " << toString(yup) << std::endl
//								  << "lat: " << toString(lat) << std::endl;
//						break;
//					}
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					{
						if(mod == 0)
						{
							int idx = cKey - '0';
							m_parent->getDisplay()->getWorldCgEngine()->dumpScene(idx);
							break;
						}
						else if(mod == Input::IEventSink::Event::MOD_ALT)
						{
							int idx = cKey - '0';
							m_parent->getDisplay()->getWorldCgEngine()->toggleScene(idx);
							break;
						}
					}



				}
				return true;
			}

			bool handleKeyRelease( unsigned char cKey, int mod )
			{
				return true;
			}

			ServiceLayer *m_parent;
		};

	public:
		defaultkeyactionsPlugHandler( ServiceLayer *service, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::NoPortHandler()
		, m_service(service)
		, m_keys( new defKeys(service) )
		{
			service->getInput()->subscribe( m_keys );
		}

		~defaultkeyactionsPlugHandler()
		{
			delete m_keys;
		}

		virtual eState handleNoMessage( )
		{
			return E_OK;
		}

		ServiceLayer *m_service;
		defKeys *m_keys;
	};
}


DEFIMP_PORTPLUG_NOPRM( defaultkeyactionsPlugHandler, ServiceLayer, defaultkeyactions )

