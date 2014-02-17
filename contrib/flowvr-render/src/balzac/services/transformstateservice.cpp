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
* File: transformstateservice.cpp                                 *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include <flowvr/render/balzac/services/transformstateservice.h>

DEFIMP_SERVICEPLUG_NOPRM(flowvr::render::balzac::TransformstateService)

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			TransformstateService::TransformstateService( flowvr::portutils::Module &parent )
			: flowvr::portutils::IServiceLayer(parent)
			, m_nLastUpdate(0)
			{
				m_Transform.identity();
				m_LastRelative.identity();
			}

			TransformstateService::~TransformstateService()
			{

			}

			bool TransformstateService::setParameters( const flowvr::portutils::ARGS & )
			{
				return true;
			}

			ftl::Mat4x4f TransformstateService::getTransform() const
			{
				return m_Transform;
			}

			void TransformstateService::setTransform( const ftl::Mat4x4f &m, bool bUpdateTs )
			{
				m_Transform = m;
				if(bUpdateTs)
					m_nLastUpdate = flowvr::utils::getNtpTimeStamp();
			}

			ftl::Mat4x4f TransformstateService::getRelativeTransform() const
			{
				return m_LastRelative;
			}

			void TransformstateService::applyTransform( const ftl::Mat4x4f &rel, eMode md, bool bUpdateTs )
			{
				m_LastRelative = rel;
				if( md == MD_RIGHT_MULT )
				{
					setTransform(m_Transform * m_LastRelative, bUpdateTs);
				}
				else
					setTransform(m_LastRelative * m_Transform, bUpdateTs);
			}

			flowvr::utils::microtime TransformstateService::getLastUpdateTs() const
			{
				return m_nLastUpdate;
			}


			void TransformstateService::getTargetIds( std::vector<flowvr::ID> &v ) const
			{
				v = m_vecIds;
			}

			void TransformstateService::setTargetIds( const std::vector<flowvr::ID> &v )
			{
				m_vecIds = v;
			}
		}
	}
}
