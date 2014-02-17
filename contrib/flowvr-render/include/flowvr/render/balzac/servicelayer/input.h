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

#ifndef BALZACINPUT_H_
#define BALZACINPUT_H_

#include <vector>
#include <ftl/chunkevents.h>

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			class Input
			{
			public:
				Input();
				~Input();

				bool onKeyPress( unsigned char nKey, int x, int y );
				bool onKeyRelease( unsigned char nKey, int x, int y );

				bool onSpecialKeyPress( unsigned char nKey, int x, int y );
				bool onSpecialKeyRelease( unsigned char nKey, int x, int y );

				bool onMouseButton( int button, int state, int x, int y );
				bool onMouseMotion( int x, int y );

				bool onWheel( int, int, int, int );

				bool getActivated() const;
				void setActivated( bool bActive );

				class IEventSink
				{
				public:

					struct Event
					{
						enum eType
						{
							TP_NONE=0,
							TP_MOUSE,
							TP_MOUSE_BUTTON_PRESS,
							TP_MOUSE_BUTTON_RELEASE,
							TP_MOUSE_WHEEL,
							TP_KEYBOARD_PRESS,
							TP_KEYBOARD_RELEASE,
							TP_KEYBOARD_SPECIAL_PRESS,
							TP_KEYBOARD_SPECIAL_RELEASE
						};

						enum eMod
						{
							MOD_SHIFT = 0x001,
							MOD_CRTL  = 0x002,
							MOD_ALT   = 0x004
						};

						Event()
						: m_type(TP_NONE)
						, state(0)
						, modifier(0)
						, wheeldir(0)
						, nKey(0)
						{
							pos[0] = pos[1] = 0;
						}

						Event( unsigned char key, bool down, bool special, int x, int y, int mod )
						: state(0)
						, nKey(key)
						, modifier(mod)
						, wheeldir(0)
						{
							pos[0] = x;
							pos[1] = y;
							if( special )
								m_type = ( down ? TP_KEYBOARD_SPECIAL_PRESS : TP_KEYBOARD_SPECIAL_RELEASE );
							else
								m_type = ( down ? TP_KEYBOARD_PRESS : TP_KEYBOARD_RELEASE );

						}
						Event( int mbutton, bool down, int x, int y, int mod )
						: modifier(mod)
						, nKey(0)
						, wheeldir(0)
						, state( mbutton )
						{
							if( down )
								m_type = TP_MOUSE_BUTTON_PRESS;
							else
								m_type = TP_MOUSE_BUTTON_RELEASE;

							pos[0] = x;
							pos[1] = y;
						}

						Event( int x, int y, int mod )
						: m_type( TP_MOUSE )
						, modifier( mod )
						, nKey(0)
						, wheeldir(0)
						, state(0)
						{
							pos[0] = x;
							pos[1] = y;
						}

						Event( int nBt, int nDir, int x, int y, int mod )
						: m_type(TP_MOUSE_WHEEL)
						, state(nBt)
						, modifier(mod)
						, wheeldir(nDir)
						, nKey(0)
						{
							pos[0] = x;
							pos[1] = y;
						}



						eType m_type;
						int   pos[2];
						int   state;
						int   modifier;
						int   wheeldir;

						unsigned char nKey;

					};

					virtual bool handleEvent( Event * ) = 0;
				};

				void subscribe( IEventSink * );
				void unsubscribe( IEventSink * );

			public:
//

			private:
				int m_nWheel;
				int m_nButton;
				bool m_bActivated;

				std::vector<IEventSink*> m_sinks;
			};

		}
	}
}




#endif // INPUT_H_
