/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                       Template Library                          *
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
* File: genericviewertest.comp.cpp                                *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include <flowvr/app/core/genclass.h>
#include <flowvr/app/components/syncmaxfrequency.comp.h>
#include <flowvr/portutils/components/portmodulerun.comp.h>
#include <flowvr/render/balzac/components/balzaclostinspaceviewer.comp.h>

#include "metamodulegenericviewer.comp.h"

using namespace flowvr::app;
using namespace flowvr::portutils;


namespace flowvr
{
	namespace render
	{
		class GenericViewer : public Composite
		{
		public:
			GenericViewer( const std::string &id_ )
			: Composite(id_)
			{
				addParameter( "balzac-portfile", "balzac-normal.xml" );
				addParameter( "navigate-portfile", "genericviewer-navigate.xml" );
			}

			virtual void execute()
			{
				balzac::BalzacLostInSpaceViewer *balzac = new balzac::BalzacLostInSpaceViewer( "renderer", this );

				MetaModuleGenericViewer *gView = addObject<MetaModuleGenericViewer>("viewer");
				link( gView->getPort("scene"), balzac->getPort("scene") );

				SyncMaxFrequency *ff = addObject<SyncMaxFrequency>( "ff" );
				link( ff->getPort("out"), gView->getPort("beginIt") );
				link( gView->getPort("endIt"), ff->getPort("endIt") );
			}
		};
		GENCLASS(GenericViewer)
	}
}
