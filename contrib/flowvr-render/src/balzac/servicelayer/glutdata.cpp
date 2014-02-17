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
* File: glutdata.cpp                                              *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include "flowvr/render/balzac/servicelayer/glutdata.h"


namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			GlutData::GlutData( Display *pDisp, Input *pInput )
			: m_display(pDisp)
			, m_input(pInput)
			{

			}
		}

	}
}
