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
* File: devices.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/render/balzac/servicelayer/servicelayer.h>
#include <flowvr/render/balzac/servicelayer/input.h>


namespace
{
	using namespace flowvr::render::balzac;
	using namespace ftl;

	class devicesPlugHandler : public flowvr::portutils::SourcePortHandler
	{
	public:
		class inputHandler : public Input::IEventSink
		{
		public:
			inputHandler(devicesPlugHandler *parent)
			: Input::IEventSink()
			, m_parent(parent)
			, m_nWheel(0)
			, m_nButton(0)
			{

			}



			bool handleEvent( Input::IEventSink::Event *p )
			{
				switch(p->m_type)
				{
				case Input::IEventSink::Event::TP_MOUSE:
				case Input::IEventSink::Event::TP_MOUSE_BUTTON_PRESS:
				case Input::IEventSink::Event::TP_MOUSE_BUTTON_RELEASE:
				case Input::IEventSink::Event::TP_MOUSE_WHEEL:
					return handleMouse(p);
				case Input::IEventSink::Event::TP_KEYBOARD_PRESS:
				case Input::IEventSink::Event::TP_KEYBOARD_RELEASE:
				case Input::IEventSink::Event::TP_KEYBOARD_SPECIAL_PRESS:
				case Input::IEventSink::Event::TP_KEYBOARD_SPECIAL_RELEASE:
					return handleKeyboard(p);
				default:
					return false;
				}
				return true;
			}

			bool handleMouse( Event *p )
			{
				float trans[2];
				trans[0] = float(p->pos[0]);
				trans[1] = float(p->pos[1]);

				int nReleased = 0;

				switch( p->m_type )
				{
				case Input::IEventSink::Event::TP_MOUSE_BUTTON_PRESS:
				case Input::IEventSink::Event::TP_MOUSE_BUTTON_RELEASE:
				{
					// change button state
					m_nButton &= ~(1 << p->state);
					m_nButton |= ((p->m_type == Input::IEventSink::Event::TP_MOUSE_BUTTON_PRESS) << p->state);

					if( p->m_type == Input::IEventSink::Event::TP_MOUSE_BUTTON_PRESS )
						nReleased = 1;
					else if( p->m_type == Input::IEventSink::Event::TP_MOUSE_BUTTON_RELEASE )
						nReleased = 2;
					else
						nReleased = 0;

//					std::cout << "BtState: " << m_nButton << "; nReleased = " << nReleased << std::endl;
					break;
				}
				case Input::IEventSink::Event::TP_MOUSE_WHEEL:
				{
					// change wheel state
					m_nWheel += p->wheeldir;

//					std::cout << "WHEEL: " << p->wheeldir << "; " << m_nWheel << std::endl;
					break;
				}
				default:
					// Normal position change
					break;
				}

				// keys = btmask
				// trans = pos in float
				// ID = 0 (?)
				// wheel = m_nWheel
				writer().addEventMouse(m_nButton, trans, 0, m_nWheel, p->wheeldir, nReleased, p->modifier );

				return true;
			}

			bool handleKeyboard( Event *p )
			{
				bool bPressed = false;
				bool bSpecial = false;

				switch( p->m_type )
				{
					case Input::IEventSink::Event::TP_KEYBOARD_PRESS:
					{
						bPressed = true;
					}
					case Input::IEventSink::Event::TP_KEYBOARD_RELEASE:
					{
						bSpecial = false;
						break;
					}
					case Input::IEventSink::Event::TP_KEYBOARD_SPECIAL_PRESS:
					{
						bPressed = true;
					}
					case Input::IEventSink::Event::TP_KEYBOARD_SPECIAL_RELEASE:
					{
						bSpecial = true;
						break;
					}
					default:
						return false;
				}
				writer().addEventKeyboard(p->nKey, bPressed, bSpecial, p->modifier );

				return true;
			}


			ChunkEventWriter &writer() const { return m_parent->m_writer; }
			devicesPlugHandler *m_parent;

			int m_nWheel, m_nButton;
		};

		devicesPlugHandler( ServiceLayer *pService, const flowvr::portutils::ARGS & )
		: flowvr::portutils::SourcePortHandler()
		, m_service(pService)
		, m_handler( new inputHandler(this) )
		{
			Input *inp = m_service->getInput();

			inp->subscribe( m_handler );
			inp->setActivated(true);
		}

		~devicesPlugHandler()
		{
			delete m_handler;
		}

		virtual eState handleMessage( flowvr::MessageWrite &out, 
                                      flowvr::StampList *stampsOut,
                                      flowvr::Allocator &allocate )
		{
//			Input *inp = m_service->getInput();
//			if(!inp)
//				return E_ERROR;

			if(m_writer.isDirty())
				out = m_writer.dump<flowvr::Allocator>( &allocate );

			return E_OK;
		}


		ServiceLayer      *m_service;
		ChunkEventWriter   m_writer;

		inputHandler      *m_handler;
	};
}


DEFIMP_PORTPLUG_NOPRM( devicesPlugHandler, ServiceLayer, devices )

