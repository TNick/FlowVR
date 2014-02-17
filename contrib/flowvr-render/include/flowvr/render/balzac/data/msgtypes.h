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

#ifndef MSGTYPES_H_
#define MSGTYPES_H_

#include <flowvr/id.h>
#include <ftl/vec.h>
#include <ftl/mat.h>

namespace flowvr
{
	class BufferWrite;
	class Buffer;


	namespace render
	{
		namespace balzac
		{
			class MsgTypeBase
			{
			public:
				virtual bool toBuffer( flowvr::BufferWrite &, unsigned int nOffset = ~0 ) const = 0;
				virtual bool fromBuffer( const flowvr::Buffer &, unsigned int nOffset = 0 ) = 0;
			};

			class ViewportChgMsg : public MsgTypeBase
			{
			public:
				ViewportChgMsg();

				int m_x, m_y, m_w, m_h;


				virtual bool toBuffer( flowvr::BufferWrite &, unsigned int nOffset = ~0 ) const;
				virtual bool fromBuffer( const flowvr::Buffer &, unsigned int nOffset = 0 );
			};

			class ObjectBounds : public MsgTypeBase
			{
			public:
				ID id;
				ftl::Vec3f   m_a,m_b;
				ftl::Mat4x4f toWorld;

				virtual bool toBuffer( flowvr::BufferWrite &, unsigned int nOffset = 0 ) const;
				virtual bool fromBuffer( const flowvr::Buffer &, unsigned int nOffset = 0 );
			};
		}
	}
}


#endif // MSGTYPES_H_
