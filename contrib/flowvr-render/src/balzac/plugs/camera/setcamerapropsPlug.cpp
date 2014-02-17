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
* File: setcameraprops.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>
#include <flowvr/render/chunkrenderwriter.h>

#include <flowvr/render/balzac/services/transformstateservice.h>

#include <ftl/vec.h>

using namespace flowvr::portutils;
using namespace ftl;
using namespace flowvr;
using namespace flowvr::render;
using namespace flowvr::render::balzac;


extern "C" void getParameters( ARGS &args )
{
	args["TARGETS"]    = Parameter("0",   "List of targetgroups to assign the main cam to", Parameter::P_LIST, Parameter::PS_NUMBER );
	args["CAMID"]      = Parameter("0", "ID of the camera to manipulate", Parameter::P_NUMBER);
	args["CLEARCOLOR"] = Parameter(Parameter::P_LIST, Parameter::PS_NUMBER, "RGB value to assign to the clear color of the camera", Parameter::MD_OPTIONAL );
	args["FOVY"]       = Parameter("60.0", "Field of view", Parameter::P_NUMBER );
	args["ASPECT"]     = Parameter("0.75", "Aspect ratio (y)", Parameter::P_NUMBER );
	args["ZMIN"]       = Parameter("0.001", "Near plane distance (view-coordinates)", Parameter::P_NUMBER );
	args["ZMAX"]       = Parameter("1000.0", "Far plane distance (view-coordinates)", Parameter::P_NUMBER );
	args["POSITION"]   = Parameter("0,0,0", "Initial position of the cam (x,y,z) in world coordinates.", Parameter::P_LIST, Parameter::PS_NUMBER );
	args["CENTER"]     = Parameter("0,0,-2", "Initial center of the view (x,y,z) in world coordinates.", Parameter::P_LIST, Parameter::PS_NUMBER );
	args["UPVECTOR"]   = Parameter("0,1,0", "Initial up-vector of the view as world axis", Parameter::P_LIST, Parameter::PS_NUMBER );
	args["VERBOSE"]    = Parameter("false", "Output camera props on first run ", Parameter::P_BOOLEAN );
}

namespace
{
	Vec3f getV3FromParVec( const ARGS &args, const std::string &propName, bool bThrow = false )
	{
		if( !args.hasParameter(propName) )
		{
			if(bThrow)
				throw std::exception();
			return Vec3f();
		}

		Parameter::ParVecN v = args(propName).getValue<Parameter::ParVecN>();

		if( v.size() < 3 )
		{
			if( bThrow )
				throw std::exception();
			return Vec3f();
		}

		return Vec3f( v[0], v[1], v[2] );
	}

	Vec4f getV4FromParVec( const ARGS &args, const std::string &propName, bool bThrow = false )
	{
		if( !args.hasParameter(propName) )
		{
			if(bThrow)
				throw std::exception();
			return Vec4f();
		}

		Parameter::ParVecN v = args(propName).getValue<Parameter::ParVecN>();

		if( v.size() < 4 )
		{
			if( bThrow )
				throw std::exception();
			std::cerr << "## PARAMETER-ERROR: expecting 4 values for a Vec4f [" << propName << "]" << std::endl;

			return Vec4f();
		}

		return Vec4f( v[0], v[1], v[2], v[3]);
	}

	/**
	 * This plugin is meant for static scenes or initial settings.
	 * For more flexible scene control, you need a backlink to the application/scene management.
	 */
	class setcamerapropsPlugHandler : public SourcePortHandler
	{
	public:
		setcamerapropsPlugHandler( TransformstateService *service, const ARGS &args )
		: SourcePortHandler()
		, m_service(service)
		, m_clearColor( Vec4f(0,0,0,0) )
		, m_fov(60.0f)
		, m_aspect(4.0f/3.0f)
		, m_zmin(0.001)
		, m_zmax(1000)
		, m_hasClearColor(false)
		, m_vPos(Vec3f(0,0,0))
		, m_vCenter(Vec3f(0,0,-2))
		, m_vUp( Vec3f( 0, 1, 0) )
		, m_CamId(ID_CAMERA)
		, m_bVerbose(false)
		{
			try
			{
				m_vecTargets = args("TARGETS").getValue<Parameter::ParVecN>();
				m_CamId = args("CAMID").getValue<ID>();
				m_bVerbose = args("VERBOSE").getValue<bool>();

				if( args.hasParameter("CLEARCOLOR") )
				{
					m_clearColor = getV4FromParVec( args, "CLEARCOLOR" );
					m_hasClearColor = true;
				}

				m_vPos = getV3FromParVec( args, "POSITION" );
				m_vCenter = getV3FromParVec( args, "CENTER" );
				m_vUp = getV3FromParVec( args, "UPVECTOR" );

				m_vUp.normalize();


				m_fov = args("FOVY").getValue<float>();
				m_aspect = args("ASPECT").getValue<float>();
				m_zmin = args("ZMIN").getValue<float>();
				m_zmax = args("ZMAX").getValue<float>();
			}
			catch( std::exception & )
			{

			}

		}

		class _compose : public std::unary_function<double, void>
		{
		public:
			_compose() : m_val(0) {}
			unsigned int m_val;

			void operator()( double &d )
			{
				m_val += ChunkPrimParam::TG( (unsigned int)d );
			}
		};

		/**
		 * Callback fired once after a connection is detected, and then only on demand (through a plug-re-init).
		 * Demonstrates the basics of camera properties manipulation.
		 */
		virtual bool once(flowvr::ModuleAPI &,
				          flowvr::MessageWrite &mw,
				          flowvr::StampList *sl,
				          flowvr::Allocator &alloc )
		{
			ChunkRenderWriter scene;

			_compose c = std::for_each( m_vecTargets.begin(), m_vecTargets.end(), _compose() );

			scene.addParam(m_CamId, ChunkPrimParam::TARGETGROUP, "", c.m_val );
			if(m_hasClearColor)
				scene.addParam(m_CamId, ChunkPrimParam::PARAMOPENGL, "clear", &m_clearColor[0], Type::Vec4f );

			scene.addParam(m_CamId, ChunkPrimParam::PARAMOPENGL, "fovy", &m_fov, Type::Float );
			scene.addParam(m_CamId, ChunkPrimParam::PARAMOPENGL, "zmin", &m_zmin, Type::Float );
			scene.addParam(m_CamId, ChunkPrimParam::PARAMOPENGL, "zmax", &m_zmax, Type::Float);
			scene.addParam(m_CamId, ChunkPrimParam::PARAMOPENGL, "aspect", &m_aspect, Type::Float );


			Mat4x4f m = matrixLookAt( m_vPos, m_vCenter, m_vUp );

			if( m_bVerbose )
			{
				std::cout << "[" << this->getHandlerName() << "]: " << std::endl;
				std::cout << "\tpos    = " << m_vPos << std::endl;
				std::cout << "\tcen    = " << m_vCenter << std::endl;
				std::cout << "\tupv    = " << m_vUp << std::endl;
				std::cout << "\tm      = " << m << std::endl;
				std::cout << "\tfov    = " << m_fov << std::endl;
				std::cout << "\tzmax   = " << m_zmax << std::endl;
				std::cout << "\tzmin   = " << m_zmin << std::endl;
				std::cout << "\taspect = " << m_aspect << std::endl;
				std::cout << "\tclear  = " << m_clearColor << std::endl;
				std::cout << "\tTG     = " << c.m_val << std::endl;
			}

			scene.addParam(m_CamId, ChunkPrimParam::TRANSFORM, "", m );

			m_service->setTransform(m, true);

			mw = scene.dump<Allocator>( &alloc );

			return true;
		}

		virtual eState handleMessage( MessageWrite &mw, StampList *sl, Allocator &alloc )
		{
			return E_REMOVE;
		}



		Vec4f               m_clearColor;
		Vec3f               m_vPos, m_vCenter, m_vUp;
		Parameter::ParVecN  m_vecTargets;
		ID                  m_CamId;
		float m_fov, m_aspect, m_zmin, m_zmax;
		bool m_hasClearColor, m_bVerbose;

		TransformstateService *m_service;


	};
}


DEFIMP_PORTPLUG( setcamerapropsPlugHandler, TransformstateService, setcameraprops )

