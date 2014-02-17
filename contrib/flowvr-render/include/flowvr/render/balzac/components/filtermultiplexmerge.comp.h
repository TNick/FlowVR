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

#ifndef FILTERMULTIPLEXMERGE_COMP_H_
#define FILTERMULTIPLEXMERGE_COMP_H_

#include "flowvr/app/components/filter.comp.h"

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			class FilterMultiplexMerge : public flowvr::app::Composite
			{
			public:
				FilterMultiplexMerge( const std::string &strId );
				virtual Component* create() const;

				virtual void execute();
			};
		}
	}
}

#endif // FILTERMULTIPLEXMERGE_COMP_H_
