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
* File: extractboundsservice.cpp                                    
* Contacts: assenmac                                     
*                                                                 *
******************************************************************/

#include <flowvr/render/balzac/services/extractboundsservice.h>
#include <vector>
#include <set>


using namespace std;
using namespace ftl;
using namespace flowvr::render;

DEFIMP_SERVICEPLUG(flowvr::render::balzac::extractboundsservice)

using namespace flowvr::portutils;


extern "C" void getParameters( flowvr::portutils::ARGS &args )
{
	args["CAPCAM"] = Parameter( "false", "include cam-position in scene-bounding box (or just all other primitives, excluding bounds)", Parameter::P_BOOLEAN );
	args["IGNOREIDS"] = Parameter(Parameter::P_LIST, Parameter::PS_NUMBER, "List of IDs to ignore for autoscaling", Parameter::MD_OPTIONAL );
	args["OBSERVETARGETS"] = Parameter("0", Parameter::P_LIST, Parameter::PS_NUMBER, "List of target groups to observe", Parameter::ST_DEFAULT, Parameter::MD_OPTIONAL );
	args["VERBOSE"] = Parameter("false", Parameter::P_BOOLEAN, Parameter::PS_NONE, "Toggle verbose mode");
}

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{

			extractboundsservice::_primInfo::_primInfo()
			: m_id(-1)
			, m_tp(flowvr::render::TP_OBJECT)
			, m_tg(0)
			, m_bDirty(false)
			{
				initMatrices();
			}

			extractboundsservice::_primInfo::_primInfo( ID id )
			: m_id( id )
			, m_tp(flowvr::render::TP_OBJECT)
			, m_tg(0)
			, m_bDirty(false)
			{
				initMatrices();
			}

			extractboundsservice::_primInfo::_primInfo( ID id, unsigned int tp, unsigned int tg )
			: m_id( id )
			, m_tp(tp)
			, m_tg(tg)
			, m_bDirty(false)
			{
				initMatrices();
			}


			void extractboundsservice::_primInfo::updateAccumulated()
			{
				accum = transf * rotf * scalef;
				m_bDirty = false;
//				std::cout << "accum = " << accum << std::endl;
//				std::cout << "transf = " << transf << std::endl;
//				std::cout << "rotf = " << rotf << std::endl;
//				std::cout << "scalef = " << scalef << std::endl;
			}

			void extractboundsservice::_primInfo::initMatrices()
			{
				accum.identity();
				transf.identity();
				rotf.identity();
				scalef.identity();
			}

			void extractboundsservice::_primInfo::setTransf( const Mat4x4f &trans )
			{
				transf = trans;
				m_bDirty = true;
			}

			void extractboundsservice::_primInfo::setRotf( const Mat4x4f &trans )
			{
				rotf = trans;
				m_bDirty = true;
			}

			void extractboundsservice::_primInfo::setScalef( const Mat4x4f &trans )
			{
				scalef = trans;
				m_bDirty = true;
			}

			class _eraseResourceOp : public std::unary_function<void, std::pair<const ID,extractboundsservice::_primInfo> >
			{
			public:
				_eraseResourceOp( ID id )
				: m_id(id) {}

				class _findOp : public std::unary_function<bool, ID>
				{
				public:
					_findOp( ID id )
					: m_id(id) {}

					bool operator()( ID id ) const
					{
						return id <= m_id;
					}

					ID m_id;
				};

				void operator()( std::pair<const ID,extractboundsservice::_primInfo> &o ) const
				{
					std::vector<ID>::const_iterator cit = std::find_if( o.second.m_res.begin(), o.second.m_res.end(), _findOp( m_id ) );
					if( cit == o.second.m_res.end() )
						return;
					else
					{
						std::vector<ID>::iterator n = std::remove_if(o.second.m_res.begin(), o.second.m_res.end(), _findOp( m_id ) );
						o.second.m_res.erase(n, o.second.m_res.end());
					}
				}

				ID m_id;
			};


			extractboundsservice::extractboundsservice( flowvr::portutils::Module &m )
			: IServiceLayer(m)
			, m_bCapCam(false)
			, m_nRoundId(0)
			, m_nPrimitiveStateChange(0)
			, m_watchMask(0)
			, m_bVerbose(false)
			{
				m_sceneBounds.clear();
			}

			extractboundsservice::~extractboundsservice()
			{
			}

			bool extractboundsservice::setParameters( const flowvr::portutils::ARGS &args )
			{
				try
				{
					m_bCapCam = args("CAPCAM").getValue<bool>();
					m_bVerbose = args("VERBOSE").getValue<bool>();

					if( args.hasParameter("IGNOREIDS") )
					{
						m_ignoreIds = args("IGNOREIDS").getValue<Parameter::ParVecN>();
					}

					m_observeTg = args("OBSERVETARGETS").getValue<Parameter::ParVecN>();

					for( Parameter::ParVecN::const_iterator cit = m_observeTg.begin(); cit != m_observeTg.end(); ++cit )
						m_watchMask = m_watchMask bitor (1<<int((*cit)));

				}
				catch( std::exception &e )
				{

				}

				if(m_bCapCam)
					m_mpPrimitives[ID_CAMERA] = _primInfo(ID_CAMERA);

				return true;
			}


			bool extractboundsservice::extractBounds( const flowvr::Message &in )
			{
				std::set<_primInfo> this_round_prims;
				for (ftl::ChunkIterator it = ftl::chunkBegin(in); it != ftl::chunkEnd(in); ++it)
				{
					const ftl::Chunk* c = (const ftl::Chunk*) it; // Convert the iterator in a chunk base structure
					switch (c->type & 0x0F)
					{
						case ChunkRender::RES_ADD: // In the case of a keyboard input
						case ChunkRender::RES_UPDATE:
						{
							// dangerous cast, but both have the same memory layout... so... lets try...
							const ChunkResource3D *r3d = (const ChunkResource3D*)c;
							BBox bb;
							r3d->getBBox(bb);
							m_mpResources[ r3d->id ] = bb;
							break;
						}
						case ChunkRender::RES_DEL:
						{
							const ChunkResource *r = (const ChunkResource*)c;
							// erase resource id from m_mpResources
							RMAP::iterator it = m_mpResources.find( r->id );
							if( it != m_mpResources.end() )
							{
								m_mpResources.erase(it);
								std::for_each( m_mpPrimitives.begin(), m_mpPrimitives.end(), _eraseResourceOp( r->id ) ); // remove all links
							}
							break;
						}
						case ChunkRender::PRIM_ADD:
						{
							const ChunkPrimAdd *a = (const ChunkPrimAdd *)c;
							m_mpPrimitives[ a->id ] = _primInfo(a->id, a->tp, a->targetGroups);

							this_round_prims.insert(_primInfo(a->id));
							if( m_bVerbose )
								std::cout << "found ADD: id: " << a->id << "(" << std::hex << a->id << std::dec << ")" << std::endl;

							++m_nPrimitiveStateChange;
							break;
						}
						case ChunkRender::PRIM_DEL:
						{
							const ChunkPrimDel *d = (const ChunkPrimDel *)c;
								m_mpPrimitives.erase( d->id );
								++m_nPrimitiveStateChange;
							break;
						}
						case ChunkRender::PRIM_PARAM:
						{
							const ChunkPrimParam *p = (const ChunkPrimParam *)c;

							switch( p->param )
							{
								case ChunkPrimParam::TARGETGROUP:
								{
									PMAP::iterator it = m_mpPrimitives.find( p->id );
									if( it == m_mpPrimitives.end() )
									{
										// create an entry
										m_mpPrimitives[ p->id ] = _primInfo( p->id, flowvr::render::TP_OBJECT, *((unsigned int*)p->data())  );
										it = m_mpPrimitives.find(p->id);
										this_round_prims.insert(_primInfo(p->id));
										++m_nPrimitiveStateChange;
									}
									else
										(*it).second.m_tg = *((unsigned int*)p->data());
									break;
								}
								case ChunkPrimParam::TRANSFORM:
								case ChunkPrimParam::TRANSFORM_POSITION:
								case ChunkPrimParam::TRANSFORM_ROTATION:
								case ChunkPrimParam::TRANSFORM_SCALE:
								{
									// find primitive
									PMAP::iterator it = m_mpPrimitives.find( p->id );
									if( it == m_mpPrimitives.end() )
									{
										// create an entry
										m_mpPrimitives[ p->id ] = _primInfo( p->id );
										it = m_mpPrimitives.find(p->id);

										if( m_bVerbose )
											std::cout << "added primitive because of transform [" << p->id << "]" << std::endl;
									}

									this_round_prims.insert(_primInfo(p->id));

									TypedArray val(p->dataType,p->data(),p->dataSize());
									switch( p->param )
									{
										case ChunkPrimParam::TRANSFORM:
											val.assign((*it).second.accum);
											break;
										case ChunkPrimParam::TRANSFORM_POSITION:
										{
											Vec3f pos;
											bool b = val.assign(pos);
											(*it).second.transf = matrixTransform(pos);
											(*it).second.m_bDirty = true;
											break;
										}
										case ChunkPrimParam::TRANSFORM_ROTATION:
										{
											Mat3x3f rot;

											val.assign(rot);
											(*it).second.rotf.identity();
											for(int x=0; x<3;++x)
												for(int y=0; y<3;++y)
													(*it).second.rotf[x][y] = rot[x][y];

											(*it).second.m_bDirty = true;
											break;
										}
										case ChunkPrimParam::TRANSFORM_SCALE:
										{
											float fScale;
											val.assign(fScale);
											(*it).second.scalef = matrixScale( fScale );

											(*it).second.m_bDirty = true;
											break;
										}
										default:
											break;
									}

									break;
								}
								case ChunkPrimParam::VBUFFER_ID:
								{
									// find primitive
									PMAP::iterator it = m_mpPrimitives.find( p->id );
									if( it == m_mpPrimitives.end() )
									{
										// create an entry
										m_mpPrimitives[ p->id ] = _primInfo( p->id );
										it = m_mpPrimitives.find(p->id);
									}

									TypedArray val(p->dataType,p->data(),p->dataSize());
									ID resId;
									val.assign(resId);
									(*it).second.m_res.push_back(resId);
									break;
								}
								default:
									break;
							}
						}
						default:
							break;
					} // switch type if
				} // for-chunks

				// ok, now calculate resulting bounds
				if( !m_bCapCam
						and this_round_prims.size() == 1
						and ((*this_round_prims.begin()).m_id == ID_CAMERA
							or (*this_round_prims.begin()).m_tp == TP_CAMERA ) )
				{
					return true; // in case we do not want any cam in the resulting bounds, we can stop
					             // here if the only changes we got is a single camera change
					             // (this is just a quick test)
				}

				++m_nRoundId; // update statistics

				BBox bounds;
				for( PMAP::iterator it = m_mpPrimitives.begin(); it != m_mpPrimitives.end(); ++it )
				{

					_primInfo &p = (*it).second;
					if( ignore(p) ) // will skip a primitive eventually
					{
						if( m_bVerbose )
							std::cout << "skipping prim [" << p.m_id << "] for calculation of resulting bounds." << std::endl;
						continue;
					}

					if( !p.m_res.empty() or (m_bCapCam and (p.m_id == ID_CAMERA or p.m_tp == TP_CAMERA) ) )
					{
						if(p.m_bDirty)
							p.updateAccumulated();

						for( std::vector<ID>::iterator cit = p.m_res.begin(); cit != p.m_res.end(); ++cit )
						{
							RMAP::iterator rit = m_mpResources.find( *cit );
							if( rit == m_mpResources.end() )
								continue;

							if( bounds.isEmpty() )
								bounds  = (*rit).second * p.accum;
							else
								bounds += (*rit).second * p.accum;
						}

						if( m_bCapCam and (p.m_id == ID_CAMERA or p.m_tp == TP_CAMERA) )
						{
							BBox camBox;
							camBox += Vec3f(0,0,0); // add zero point to given bounds
							bounds += camBox * p.accum; // include in global bounds
						}
					}
				}

				m_sceneBounds = bounds;

				if( m_bVerbose )
					std::cout << "Changed scene bounds to: [" << m_sceneBounds << "]" << std::endl;
			}


			BBox extractboundsservice::getSceneBounds() const
			{
				return m_sceneBounds;
			}

			const extractboundsservice::PMAP &extractboundsservice::getPrimitives() const
			{
				return m_mpPrimitives;
			}

			const extractboundsservice::RMAP &extractboundsservice::getRessources() const
			{
				return m_mpResources;
			}

			long extractboundsservice::getRoundId() const
			{
				return m_nRoundId;
			}

			bool extractboundsservice::getCapCam() const
			{
				return m_bCapCam;
			}

			long extractboundsservice::getPrimitiveStateChange() const
			{
				return m_nPrimitiveStateChange;
			}

			bool extractboundsservice::ignore( const _primInfo &p ) const
			{
				if( !m_bCapCam and p.m_tp == TP_CAMERA )
					return true;

				// special ID to ignore?
				if((std::find( m_ignoreIds.begin(), m_ignoreIds.end(), p.m_id ) != m_ignoreIds.end()))
					return true;

				// special target group to ignore?
				if( !p.m_tg or !(p.m_tg bitand m_watchMask) )
					return true;

				return false;

			}

		} // namespace balzac
	} // namespace render
} // namespace flowvr


