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
* File: keyboardmovePlug.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>


#include <flowvr/render/chunkrenderwriter.h>
#include <ftl/chunkevents.h>

#include <flowvr/render/balzac/services/transformstateservice.h>

using namespace ftl;
using namespace flowvr::render;
using namespace flowvr::render::balzac;
using namespace flowvr::portutils;

extern "C" void getParameters( flowvr::portutils::ARGS &args )
{
	// insert parameter here
	args["FORWARD"]      = Parameter("w", "Forward trigger", Parameter::P_STRING);
	args["BACKWARD"]     = Parameter("s", "Backward trigger", Parameter::P_STRING);
	args["STRAFE_LEFT"]  = Parameter("a", "Strafe left", Parameter::P_STRING);
	args["STRAFE_RIGHT"] = Parameter("d", "Strafe right", Parameter::P_STRING);
	args["TURN_LEFT"]    = Parameter("q", "Turn left", Parameter::P_STRING);
	args["TURN_RIGHT"]   = Parameter("e", "Turn right", Parameter::P_STRING);
	args["UP"]           = Parameter("z", "Upward trigger", Parameter::P_STRING);
	args["DOWN"]         = Parameter("c", "Downward trigger", Parameter::P_STRING);
	args["TURN_UP"]      = Parameter("1", "Rotate up trigger", Parameter::P_STRING);
	args["TURN_DOWN"]    = Parameter("3", "Rotate down trigger", Parameter::P_STRING);
	args["ROLL_LEFT"]    = Parameter("4", "Roll left", Parameter::P_STRING);
	args["ROLL_RIGHT"]   = Parameter("5", "Roll right", Parameter::P_STRING);

	args["INCTSCALE"]    = Parameter("[", "Increase translation scale", Parameter::P_STRING );
	args["DECTSCALE"]    = Parameter("]", "Decrease translation scale", Parameter::P_STRING );
	args["INCRSCALE"]    = Parameter("-", "Increase rotation scale", Parameter::P_STRING );
	args["DECRSCALE"]    = Parameter("=", "Decrease rotation scale", Parameter::P_STRING );

	args["RESET"]        = Parameter("r", "Reset trigger", Parameter::P_STRING);

	args["CAMID"]        = Parameter("0", "Camera ID to control", Parameter::P_NUMBER);
	args["TSCALE"]       = Parameter("1", "Transform scale factor", Parameter::P_NUMBER);
	args["RSCALE"]       = Parameter("1", "Rotation scale factor", Parameter::P_NUMBER);
	args["TSCALEFACTOR"] = Parameter("0.01", "Factor to increase / decrease translation scale", Parameter::P_NUMBER );
	args["RSCALEFACTOR"] = Parameter("0.01", "Factor to increase / decrease rotation scale", Parameter::P_NUMBER );


	args["INITIALPOS"]   = Parameter(Parameter::P_LIST, Parameter::PS_NUMBER, "Initial cam pos / reset point", Parameter::MD_OPTIONAL );
	args["VERBOSE"]      = Parameter("false", "Verbose mode", Parameter::P_BOOLEAN);
}

namespace
{

	class keyboardmovePlugHandler : public flowvr::portutils::PipeHandler
	{
	public:

		enum eMoveToken
		{
			ID_NONE = 0,
			ID_FORWARD,
			ID_STRAFE_LEFT,
			ID_STRAFE_RIGHT,
			ID_BACKWARD,
			ID_TURN_LEFT,
			ID_TURN_RIGHT,
			ID_TURN_UP,
			ID_TURN_DOWN,
			ID_ROLL_LEFT,
			ID_ROLL_RIGHT,
			ID_UP,
			ID_DOWN,
			ID_RESET,
			ID_INCTSCALE,
			ID_DECTSCALE,
			ID_INCRSCALE,
			ID_DECRSCALE
		};


		keyboardmovePlugHandler( TransformstateService *service, const flowvr::portutils::ARGS &args )
		: flowvr::portutils::PipeHandler()
		, m_service(service)
		, m_bHasIniPos(false)
		, m_bVerbose(false)
		, m_tfactor(0.01f)
		, m_rfactor(0.01f)
		{
			try
			{
				// parse arguments here
				eMoveToken forward, backward, strafe_left, strafe_right,
				           turn_left, turn_right, up, down, reset,
				           turn_up, turn_down, roll_left, roll_right,
				           inctscale, dectscale, incrscale, decrscale;
				forward      = eMoveToken(int(args("FORWARD").getValueString()[0]));
				backward     = eMoveToken(int(args("BACKWARD").getValueString()[0]));
				strafe_left  = eMoveToken(int(args("STRAFE_LEFT").getValueString()[0]));
				strafe_right = eMoveToken(int(args("STRAFE_RIGHT").getValueString()[0]));
				turn_left    = eMoveToken(int(args("TURN_LEFT").getValueString()[0]));
				turn_right   = eMoveToken(int(args("TURN_RIGHT").getValueString()[0]));
				up           = eMoveToken(int(args("UP").getValueString()[0]));
				down         = eMoveToken(int(args("DOWN").getValueString()[0]));
				reset        = eMoveToken(int(args("RESET").getValueString()[0]));
				turn_up      = eMoveToken(int(args("TURN_UP").getValueString()[0]));
				turn_down    = eMoveToken(int(args("TURN_DOWN").getValueString()[0]));
				roll_left    = eMoveToken(int(args("ROLL_LEFT").getValueString()[0]));
				roll_right	 = eMoveToken(int(args("ROLL_RIGHT").getValueString()[0]));

				inctscale	 = eMoveToken(int(args("INCTSCALE").getValueString()[0]));
				dectscale	 = eMoveToken(int(args("DECTSCALE").getValueString()[0]));
				incrscale	 = eMoveToken(int(args("INCRSCALE").getValueString()[0]));
				decrscale	 = eMoveToken(int(args("DECRSCALE").getValueString()[0]));

				m_tkmap[ _stimulus(forward)] = ID_FORWARD;
				m_tkmap[ _stimulus(backward)] = ID_BACKWARD;
				m_tkmap[ _stimulus(strafe_left)] = ID_STRAFE_LEFT;
				m_tkmap[ _stimulus(strafe_right)] = ID_STRAFE_RIGHT;
				m_tkmap[ _stimulus(turn_left)] = ID_TURN_LEFT;
				m_tkmap[ _stimulus(turn_right)] = ID_TURN_RIGHT;
				m_tkmap[ _stimulus(up)]   = ID_UP;
				m_tkmap[ _stimulus(down)] = ID_DOWN;
				m_tkmap[ _stimulus(reset)] = ID_RESET;
				m_tkmap[ _stimulus(turn_up)] = ID_TURN_UP;
				m_tkmap[ _stimulus(turn_down)] = ID_TURN_DOWN;
				m_tkmap[ _stimulus(roll_left)] = ID_ROLL_LEFT;
				m_tkmap[ _stimulus(roll_right)] = ID_ROLL_RIGHT;

				m_tkmap[ _stimulus(inctscale)] = ID_INCTSCALE;
				m_tkmap[ _stimulus(dectscale)] = ID_DECTSCALE;
				m_tkmap[ _stimulus(incrscale)] = ID_INCRSCALE;
				m_tkmap[ _stimulus(decrscale)] = ID_DECTSCALE;

				m_camId = args("CAMID").getValue<flowvr::ID>();

				m_tscale = args("TSCALE").getValue<float>();
				m_rscale = args("RSCALE").getValue<float>();

				m_tfactor = args("TSCALEFACTOR").getValue<float>();
				m_rfactor = args("RSCALEFACTOR").getValue<float>();

				if( args.hasParameter("INITIALPOS") )
				{
					Parameter::ParVecN v = args("INITIALPOS").getValue<Parameter::ParVecN>();
					if(v.size() != 3)
					{
						std::cerr << "initial pos: bad format (expecting 3 numbers)" << std::endl;
					}
					else
					{
						m_vIniTrans = matrixTransform( Vec3f(v[0], v[1], v[2]) );
						m_bHasIniPos = true;
					}
				}

				m_bVerbose = args("VERBOSE").getValue<bool>();

			}
			catch( std::exception &e )
			{
			}
		}



		bool once(flowvr::ModuleAPI &, flowvr::MessageWrite &m_out,
				  flowvr::StampList *sl_out, flowvr::Allocator &alloc )
		{
			if( m_bHasIniPos )
			{
				Mat4x4f m = matrixTransform<float>(m_vIniPos);
				m_service->setTransform(m, true);
				m_scene.addParam( m_camId, ChunkPrimParam::TRANSFORM, "", m );

				m_out = m_scene.dump<flowvr::Allocator>( &alloc );
			}
			else
			{
				m_vIniTrans = m_service->getTransform();
			}
			return true;
		}


		virtual eState handleMessagePipe( const flowvr::Message &m_in,
				                          const flowvr::StampList *sl_in,
				                          flowvr::MessageWrite &m_out,
				                          flowvr::StampList *sl_out,
				                          flowvr::Allocator &allocate )
		{
                        // std::cout << "Handling message pipe" << std::endl;
			if( m_in.data.getSize() == 0 )
				return E_OK;

			for (ChunkIterator it = chunkBegin(m_in); it != chunkEnd(m_in); ++it)
			{
				const Chunk* c = (const Chunk*) it; // Convert the iterator in a chunk base structure
				switch (c->type & 0x0F)
				{ // Look for each type of IO input

					case ChunkEvent::KEYBOARD: // In the case of a keyboard input
					{
						handleKeyboardEvent(((ChunkEventKeyboard *) c));
						break;
					}
				}
			}

			if( m_scene.isDirty() )
				m_out = m_scene.dump<flowvr::Allocator>( &allocate );
			return E_OK;
		}

		void handleKeyboardEvent( ChunkEventKeyboard *keyEvent )
		{

			static bool isKeyDown[13] = {0}; // Only used for movement

			int  modifiers = keyEvent->modifier;
			bool special   = keyEvent->special;
			int key        = keyEvent->key;

			// true: down, false: up key event
			bool bDown     = keyEvent->val;

			std::cout << "key = " << char(key) << " ; special: " << special << " ; mod: " << modifiers << std::endl;

			eMoveToken what = mapKeyToToken( key, special, modifiers );
			Vec3f translate(0,0,0);
			Quat  rotate(1,0,0,0);

			if( bDown )
			{
				switch( what )
				{
				case ID_BACKWARD:
					isKeyDown[ID_BACKWARD] = true;
					//translate = Vec3f( 0,0,1 );
					break;
				case ID_FORWARD:
					isKeyDown[ID_FORWARD] = true;
					//translate = Vec3f( 0,0,-1 );
					break;
				case ID_STRAFE_LEFT:
					isKeyDown[ID_STRAFE_LEFT] = true;
					//translate = Vec3f( -1, 0, 0 );
					break;
				case ID_STRAFE_RIGHT:
					isKeyDown[ID_STRAFE_RIGHT] = true;
					//translate = Vec3f(  1, 0, 0 );
					break;
				case ID_TURN_LEFT:
					isKeyDown[ID_TURN_LEFT] = true;
					//rotate.fromDegreeAngAxis( -1*m_rscale, Vec3f( 0, 1, 0 ) );
					break;
				case ID_TURN_RIGHT:
					isKeyDown[ID_TURN_RIGHT] = true;
					//rotate.fromDegreeAngAxis(  1*m_rscale, Vec3f( 0, 1, 0 ) );
					break;
				case ID_UP:
					isKeyDown[ID_UP] = true;
					//translate = Vec3f( 0,1,0 );
					break;
				case ID_DOWN:
					isKeyDown[ID_DOWN] = true;
					//translate = Vec3f( 0,-1,0 );
					break;
				case ID_ROLL_LEFT:
					isKeyDown[ID_ROLL_LEFT] = true;
					//rotate.fromDegreeAngAxis(  -1*m_rscale, Vec3f( 0, 0, 1 ) );
					break;
				case ID_ROLL_RIGHT:
					isKeyDown[ID_ROLL_RIGHT] = true;
					//rotate.fromDegreeAngAxis(  1*m_rscale, Vec3f( 0, 0, 1 ) );
					break;
				case ID_TURN_UP:
					isKeyDown[ID_TURN_UP] = true;
					//rotate.fromDegreeAngAxis(  -1*m_rscale, Vec3f( 1, 0, 0 ) );
					break;
				case ID_TURN_DOWN:
					isKeyDown[ID_TURN_DOWN] = true;
					//rotate.fromDegreeAngAxis(  1*m_rscale, Vec3f( 1, 0, 0 ) );
					break;
				case ID_RESET:
				{
//					std::cout << "RESET" << std::endl;
					// set absolute transform here
					m_service->setTransform(m_vIniTrans, true);
					m_scene.addParam( m_camId, ChunkPrimParam::TRANSFORM, "", m_vIniTrans );
					break;
				}
				case ID_INCTSCALE:
				{
					m_tscale += (m_tscale*m_tfactor);
					if( m_bVerbose )
						std::cout << "m_tscale = " << m_tscale << std::endl;
					return;
				}
				case ID_DECTSCALE:
				{
					m_tscale -= (m_tscale*m_tfactor);
					if( m_bVerbose )
						std::cout << "m_tscale = " << m_tscale << std::endl;
					return;
				}
				case ID_INCRSCALE:
				{
					m_rscale += (m_rscale*m_rfactor);
					if( m_bVerbose )
						std::cout << "m_rscale = " << m_rscale << std::endl;
					return;
				}
				case ID_DECRSCALE:
				{
					m_rscale -= (m_rscale*m_rfactor);
					if( m_bVerbose )
						std::cout << "m_rscale = " << m_rscale << std::endl;
					return;
				}
				default:
					break;
				}
			} else {
				// Key up event

				switch( what )
					{
					case ID_BACKWARD:
						isKeyDown[ID_BACKWARD] = false;
						//translate = Vec3f( 0,0,1 );
						break;
					case ID_FORWARD:
						isKeyDown[ID_FORWARD] = false;
						//translate = Vec3f( 0,0,-1 );
						break;
					case ID_STRAFE_LEFT:
						isKeyDown[ID_STRAFE_LEFT] = false;
						//translate = Vec3f( -1, 0, 0 );
						break;
					case ID_STRAFE_RIGHT:
						isKeyDown[ID_STRAFE_RIGHT] = false;
						//translate = Vec3f(  1, 0, 0 );
						break;
					case ID_TURN_LEFT:
						isKeyDown[ID_TURN_LEFT] = false;
						//rotate.fromDegreeAngAxis( -1*m_rscale, Vec3f( 0, 1, 0 ) );
						break;
					case ID_TURN_RIGHT:
						isKeyDown[ID_TURN_RIGHT] = false;
						//rotate.fromDegreeAngAxis(  1*m_rscale, Vec3f( 0, 1, 0 ) );
						break;
					case ID_UP:
						isKeyDown[ID_UP] = false;
						//translate = Vec3f( 0,1,0 );
						break;
					case ID_DOWN:
						isKeyDown[ID_DOWN] = true;
						//translate = Vec3f( 0,-1,0 );
						break;
					case ID_ROLL_LEFT:
						isKeyDown[ID_ROLL_LEFT] = true;
						//rotate.fromDegreeAngAxis(  -1*m_rscale, Vec3f( 0, 0, 1 ) );
						break;
					case ID_ROLL_RIGHT:
						isKeyDown[ID_ROLL_RIGHT] = true;
						//rotate.fromDegreeAngAxis(  1*m_rscale, Vec3f( 0, 0, 1 ) );
						break;
					case ID_TURN_UP:
						isKeyDown[ID_TURN_UP] = true;
						//rotate.fromDegreeAngAxis(  -1*m_rscale, Vec3f( 1, 0, 0 ) );
						break;
					case ID_TURN_DOWN:
						isKeyDown[ID_TURN_DOWN] = true;
						//rotate.fromDegreeAngAxis(  1*m_rscale, Vec3f( 1, 0, 0 ) );
						break;
					}
			}


			// Apply movement

			if (isKeyDown[ID_BACKWARD])
				translate += Vec3f( 0,0,1 );

			if (isKeyDown[ID_FORWARD])
				translate += Vec3f( 0,0,-1 );

			if (isKeyDown[ID_STRAFE_LEFT])
				translate += Vec3f( -1, 0, 0 );

			if (isKeyDown[ID_STRAFE_RIGHT])
				translate += Vec3f(  1, 0, 0 );

			if (isKeyDown[ID_TURN_LEFT])
				rotate.fromDegreeAngAxis( -1*m_rscale, Vec3f( 0, 1, 0 ) );

			if (isKeyDown[ID_TURN_RIGHT])
				rotate.fromDegreeAngAxis(  1*m_rscale, Vec3f( 0, 1, 0 ) );

			if (isKeyDown[ID_UP])
				translate += Vec3f( 0,1,0 );

			if (isKeyDown[ID_DOWN])
				translate += Vec3f( 0,-1,0 );

			if (isKeyDown[ID_ROLL_LEFT])
				rotate.fromDegreeAngAxis(  -1*m_rscale, Vec3f( 0, 0, 1 ) );

			if (isKeyDown[ID_ROLL_RIGHT])
				rotate.fromDegreeAngAxis(  1*m_rscale, Vec3f( 0, 0, 1 ) );

			if (isKeyDown[ID_TURN_UP])
				rotate.fromDegreeAngAxis(  -1*m_rscale, Vec3f( 1, 0, 0 ) );

			if (isKeyDown[ID_TURN_DOWN])
				rotate.fromDegreeAngAxis(  1*m_rscale, Vec3f( 1, 0, 0 ) );


			updateTransform( translate*m_tscale, rotate );

		}

		eMoveToken mapKeyToToken( int key, int special, int modifier )
		{
			TKMAP::const_iterator c = m_tkmap.find(_stimulus( key, 0, 0 ));
			if(c == m_tkmap.end())
			{
//				std::cout << "key [ " << int(key) << "] not mapped." << std::endl;
//				for( TKMAP::const_iterator cit = m_tkmap.begin(); cit != m_tkmap.end(); ++cit )
//					(*cit).first.print();
				return ID_NONE;
			}

//			std::cout << "found token [" << (*c).second << "]" << std::endl;
//			 (*c).first.print();

			return (*c).second;
		}

		void updateTransform( const Vec3f &direction, const Quat &rotate )
		{
			Mat4x4f m = matrixTransform( direction, rotate ); // get relative locomotion

			m_service->applyTransform( m, TransformstateService::MD_RIGHT_MULT, true ); // apply in object space

			// set absolute transform here
			m = m_service->getTransform();
			m_scene.addParam( m_camId, ChunkPrimParam::TRANSFORM, "", m );

			if(m_bVerbose)
				std::cout << "Updating transform:" << std::endl
				          << "\tv: " << direction
				          << "\tq: " << rotate
				          << std::endl;
		}

		ChunkRenderWriter        m_scene;
		TransformstateService *m_service;

		struct _stimulus
		{
			_stimulus()
			: key(DONTCARE)
			, special(DONTCARE)
			, modifier(DONTCARE)
			{}

			_stimulus(int _key, int _special = DONTCARE, int _modifier = DONTCARE )
			: key(_key)
			, special(_special)
			, modifier(_modifier)
			{}

			int key,
			    special,
			    modifier;

			enum
			{
				DONTCARE = 0
			};

			bool operator==( const _stimulus &other ) const
			{
				if( key == other.key and special == other.special and modifier == other.modifier )
					return true;
				return false;
			}

			bool operator<( const _stimulus &other ) const
			{
				return key < other.key and special <= other.special and modifier <= other.modifier;
			}

			void print() const
			{
				std::cout << "[" << key << " ; " << special << " ; " << modifier << "]" << std::endl;
			}
 		};

		typedef std::map<_stimulus, eMoveToken> TKMAP;
		TKMAP m_tkmap;

		flowvr::ID m_camId;

		float m_tscale, m_rscale, m_tfactor, m_rfactor;

		Vec3f m_vIniPos;
		Mat4x4f m_vIniTrans;

		bool m_bHasIniPos, m_bVerbose;


	};
}


DEFIMP_PORTPLUG( keyboardmovePlugHandler, TransformstateService, keyboardmovePlug )

