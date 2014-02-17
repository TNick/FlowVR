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
* File: autoboundsservice.cpp                                     *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include <flowvr/render/balzac/services/autoboundsservice.h>


DEFIMP_SERVICEPLUG(flowvr::render::balzac::autoboundsservice)

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			autoboundsservice::autoboundsservice( flowvr::portutils::Module &m )
			: IServiceLayer(m)
			, m_projection(m)
			, m_camstate(m)
			{

			}

			autoboundsservice::~autoboundsservice()
			{

			}

			bool autoboundsservice::setParameters( const flowvr::portutils::ARGS & )
			{
				return true;
			}
		}
	}
}
