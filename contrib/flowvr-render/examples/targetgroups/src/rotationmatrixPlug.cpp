/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
                          flowvr-petaflow
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
* File: rotationmatrix.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/porthandler.h>

#include <flowvr/utils/timing.h>
#include <ftl/mat.h>
#include <ftl/vec.h>
#include <ftl/quat.h>



using namespace ftl;
using namespace flowvr;
using namespace flowvr::utils;
using namespace flowvr::portutils;


extern "C" void getParameters( flowvr::portutils::ARGS &args )
{
	// add parameters here
	args["VELOCITY"] = Parameter( "1", "Velocity (turns / min)", Parameter::P_NUMBER );
	args["AXIS"] = Parameter("0,1,0", "Rotation axis", Parameter::P_LIST, Parameter::PS_NUMBER );
	args["PIVOT"] = Parameter("0,0,0", "Pivot point", Parameter::P_LIST, Parameter::PS_NUMBER );
	args["SCALE"] = Parameter("1", "Model scale", Parameter::P_NUMBER );
}

namespace
{

	class rotationmatrixHandler : public flowvr::portutils::SourcePortHandler
	{
	public:
		rotationmatrixHandler( const flowvr::portutils::ARGS &args )
		: flowvr::portutils::SourcePortHandler()
		, m_velocity(0.0)
		, m_nLast(0)
		, m_vAxis(Vec3f(0,1,0))
		, m_scale(1)
		{
			try
			{
				m_velocity = args("VELOCITY").getValue<double>();
				Parameter::ParVecN v = args("AXIS").getValue<Parameter::ParVecN>();
				if( v.size() == 3 )
				{
					m_vAxis[0] = v[0];
					m_vAxis[1] = v[1];
					m_vAxis[2] = v[2];
				}

				v = args("PIVOT").getValue<Parameter::ParVecN>();
				if( v.size() == 3 )
				{
					m_vPivot[0] = v[0];
					m_vPivot[1] = v[1];
					m_vPivot[2] = v[2];
				}

				m_scale = args("SCALE").getValue<float>();
			}
			catch( std::exception &e )
			{
			}
		}

		virtual void preLoopActivation()
		{

		}
		virtual eState handleMessage( MessageWrite &out,
                                      StampList *stampsOut,
                                      Allocator &allocate )
		{
			if( m_nLast == 0 )
			{
				m_nLast = getNtpTimeStamp();
			}
			else
			{
				microtime now = getNtpTimeStamp();
				microtime dt = now - m_nLast;

				m_nLast = now;

				m_nAngle += float(m_velocity*dt);

				if( m_nAngle >= 360 )
					m_nAngle -= 360;
			}


			out.data = allocate.alloc( sizeof(Mat4x4f) );

			Mat4x4f *m = out.data.getWrite<Mat4x4f>();
			*m = matrixRotation<float>(m_nAngle, m_vAxis ) * matrixScale(m_scale);

			*m = matrixTransform(m_vPivot) * (*m);
			return E_OK;
		}

		double m_velocity, m_nLast, m_nAngle;
		float m_scale;
		Vec3f m_vAxis, m_vPivot;
	};
}


DEFIMP_PORTPLUG_NOSERVICE( rotationmatrixHandler,rotationmatrix )

