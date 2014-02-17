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

#ifndef AUTOBOUNDSSERVICE_H_
#define AUTOBOUNDSSERVICE_H_

#include <flowvr/portutils/portservicelayer.h>


#include "transformstateservice.h"

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			class autoboundsservice : public flowvr::portutils::IServiceLayer
			{
				public:
					autoboundsservice( flowvr::portutils::Module & );
					~autoboundsservice();

					virtual bool setParameters( const flowvr::portutils::ARGS & );

					TransformstateService m_projection;
					TransformstateService m_camstate;
			};
		}
	}
}

#endif // AUTOBOUNDSSERVICE_H_
