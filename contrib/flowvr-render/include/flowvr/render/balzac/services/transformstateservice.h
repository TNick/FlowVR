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

#ifndef TRANSFORMSTATESERVICE_H_
#define TRANSFORMSTATESERVICE_H_

#include <flowvr/portutils/portservicelayer.h>

#include <ftl/mat.h>
#include <flowvr/utils/timing.h>
#include <flowvr/id.h>

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			class TransformstateService : public flowvr::portutils::IServiceLayer
			{
			public:
				TransformstateService( flowvr::portutils::Module & );
				~TransformstateService();

				virtual bool setParameters( const flowvr::portutils::ARGS & );

				ftl::Mat4x4f getTransform() const;
				virtual ftl::Mat4x4f getRelativeTransform() const;

				virtual void setTransform( const ftl::Mat4x4f &m, bool bUpdateTs = true );

				enum eMode
				{
					MD_RIGHT_MULT = 0,
					MD_LEFT_MULT
				};

				void applyTransform( const ftl::Mat4x4f &, eMode md = MD_RIGHT_MULT, bool bUpdateTs = true );

				flowvr::utils::microtime getLastUpdateTs() const;

				void getTargetIds( std::vector<flowvr::ID> &v ) const;
				void setTargetIds( const std::vector<flowvr::ID> &v );
			private:
				std::vector<flowvr::ID> m_vecIds;

				ftl::Mat4x4f m_Transform,
				             m_LastRelative;
				flowvr::utils::microtime m_nLastUpdate;
			};
		}
	}
}


#endif // TRANSFORMSTATESERVICE_H_
