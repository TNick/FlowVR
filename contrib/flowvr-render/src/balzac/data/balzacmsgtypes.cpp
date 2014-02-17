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
* File: msgtypes.cpp
*                                                                 *
* Contacts: assenmac
*                                                                 *
******************************************************************/


#include <flowvr/render/balzac/data/msgtypes.h>
#include <flowvr/buffer.h>
#include <flowvr/message.h>

using namespace ftl;

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
		ViewportChgMsg::ViewportChgMsg()
			: m_x(0)
			, m_y(0)
			, m_w(0)
			, m_h(0)
			{
			}

			bool ViewportChgMsg::toBuffer( flowvr::BufferWrite &oStore, unsigned int nOffset ) const
			{
				if( nOffset == ~0 )
				{
					// this is the index, therefore subtract 1
					nOffset = (oStore.getSize() / sizeof( ViewportChgMsg ))-1;

					// is the offset valid within the numbers of items?
					if( oStore.getSize() < (nOffset+1) * sizeof( ViewportChgMsg) )
					{
						// no, realign buffer by adding one entry at the end
						oStore.resize( oStore.getSize() + sizeof( ViewportChgMsg ) ); // add one at the very end
						if(!oStore.valid()) // did it work?
							return false; // no, abort mission...
					}
				}


				ViewportChgMsg *head = oStore.getWrite<ViewportChgMsg>(nOffset * sizeof( ViewportChgMsg ) );
				*head = *this;
				return true;
			}

			bool ViewportChgMsg::fromBuffer( const flowvr::Buffer &oRead, unsigned int nOffset )
			{
				if( oRead.getSize() == 0 )
					return false;
				if( nOffset > (oRead.getSize() / sizeof(ViewportChgMsg))-1)
					return false;

				const ViewportChgMsg *head = oRead.getRead<ViewportChgMsg>(nOffset * sizeof(ViewportChgMsg) );
				if(head == NULL)
					return false;

				*this = *head;
				return true;
			}

			bool ObjectBounds::toBuffer( flowvr::BufferWrite &oStore, unsigned int nOffset ) const
			{
				if( nOffset == ~0 )
				{
					nOffset = oStore.getSize() / sizeof(ObjectBounds); // normalize by number of elements
					oStore.resize( oStore.getSize() + sizeof( ObjectBounds ) ); // add one at the very end
					if(!oStore.valid())
						return false;
				}

				ObjectBounds *bounds = oStore.getWrite<ObjectBounds>( nOffset * sizeof( ObjectBounds ) );
				*bounds = *this;
				return true;
			}

			bool ObjectBounds::fromBuffer( const flowvr::Buffer &oRead, unsigned int nOffset )
			{
				const ObjectBounds *head = oRead.getRead<ObjectBounds>(nOffset * sizeof(ObjectBounds) );
				*this = *head;
				return true;
			}
		}
	}
}
