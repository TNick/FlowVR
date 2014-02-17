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
*******************************************************************
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Thomas Arcila,                                               *
*    Jean-Denis Lesage,                                           *
*    Clement Menier,                                              *
*    Ingo Assenmacher,                                            *
*    Bruno Raffin                                                 *
*                                                                 *
*  Contact :                                                      *
*                                                                 *
******************************************************************/

#ifndef EXTRACTBOUNDSSERVICE_H_
#define EXTRACTBOUNDSSERVICE_H_

#include <flowvr/portutils/portservicelayer.h>
#include <flowvr/render/chunk.h>
#include <flowvr/render/bbox.h>
#include <map>
#include <vector>

namespace flowvr
{
	class Message;

	namespace render
	{
		namespace balzac
		{
			class extractboundsservice : public flowvr::portutils::IServiceLayer
			{
				public:
					extractboundsservice( flowvr::portutils::Module & );
					~extractboundsservice();

					virtual bool setParameters( const flowvr::portutils::ARGS & );

					bool extractBounds( const flowvr::Message & );

					bool getCapCam() const;


					struct _primInfo
					{
						_primInfo();
						_primInfo( ID id );
						_primInfo( ID id, unsigned int tp, unsigned int m_tg );

						void updateAccumulated();
						void initMatrices();
						void setTransf( const ftl::Mat4x4f &trans );
						void setRotf( const ftl::Mat4x4f &trans );

						void setScalef( const ftl::Mat4x4f &trans );


						ID              m_id;   /** primitive ID */
						unsigned int    m_tp;
						unsigned int    m_tg;
						std::vector<ID> m_res;  /** resource ID */

						ftl::Mat4x4f transf,
								rotf,
								scalef,
								accum; /** transformation of m_id */

						bool m_bDirty;

						bool operator<( const _primInfo &other ) const
						{
							return m_id < other.m_id;
						}
					};

					typedef std::map<ID, flowvr::render::BBox>      RMAP;
					typedef std::map<ID, _primInfo> PMAP;

					const PMAP &getPrimitives() const;
					const RMAP &getRessources() const;
					long getRoundId() const;

					long getPrimitiveStateChange() const;

					flowvr::render::BBox getSceneBounds() const;

					bool ignore( const _primInfo & ) const;

				private:
					bool m_bCapCam, m_bVerbose;
					long m_nRoundId,
					     m_nPrimitiveStateChange;
					unsigned int m_watchMask;

					RMAP m_mpResources;
					PMAP m_mpPrimitives;

					flowvr::render::BBox m_sceneBounds;
					flowvr::portutils::Parameter::ParVecN m_ignoreIds, m_observeTg;
			};
		}
	}
}


#endif // EXTRACTBOUNDSSERVICE_H_
