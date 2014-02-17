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
* File: autoboundstest.comp.cpp                                   *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include <flowvr/app/core/genclass.h>
#include <flowvr/app/core/run.h>
#include <flowvr/render/balzac/components/balzaclostinspaceviewer.comp.h>
#include <flowvr/app/components/filterpresignal.comp.h>
#include <flowvr/app/components/filtersignaland.comp.h>

#include <flowvr/render/components/metamoduleviewer.comp.h>
#include <flowvr/portutils/components/portmodulerun.comp.h>

using namespace flowvr::app;
using namespace flowvr::portutils;


namespace flowvr
{
	namespace render
	{
		class TargetGroups : public Composite
		{
		public:
			TargetGroups(const std::string &strId)
			: flowvr::app::Composite(strId)
			{
			}

			Composite *create()
			{
				return new TargetGroups( getId() );
			}

			void execute()
			{

				FlowvrRunSSH ssh(this);
				ssh.setVerbose();

				// create the renderer object
				balzac::BalzacLostInSpaceViewer *balzac = new balzac::BalzacLostInSpaceViewer( "renderer", this );

				// one module for the mirror...
				PortModuleRun *mirror = new PortModuleRun("mirror", "mirror.xml", ssh, this );

				// we need this module to identify all objects in the scene by their id (we add them to
				// the targetgroup of the FBO by that key)
				PortModuleRun *extractIds = new PortModuleRun("extractIds", "bounds.xml", ssh, this );

				// connect the mesh output with the scene port of balzac
				link( mirror->getPort("scene"), balzac->getPort("scene") );
				link( balzac->getPort("scene_out"), extractIds->getPort("scene") );
				link( extractIds->getPort("ids"), mirror->getPort("ids"));


				// a mesh source, let's take the bunny
				Component* ViewerMesh = addObject<MetaModuleViewer>("viewer");
				link( ViewerMesh->getPort("scene"), balzac->getPort("scene") );

				// rotate the scene a bit
				PortModuleRun *matrix = new PortModuleRun("rotation", "rotationmatrix.xml", ssh, this );
				link( matrix->getPort("matrix"), ViewerMesh->getPort("matrix") );


				// communication patterns:

				// we do not put more matrices than the viewer is able to output:
				// link beginIt of the matrix source with the endIt of the mesh-producer,
				// add it a pre-signal to break the loop between the two.
				FilterPreSignal *ps = addObject<FilterPreSignal>("psMatrix");
				link( ViewerMesh->getPort("endIt"), ps->getPort("in") );


				// we limit the production of a new matrix by either the production speed
				// of the mesh-viewer or the rendering speed of the renderer, using a filterand
				FilterSignalAnd *fand = addObject<FilterSignalAnd>("filter-and");
				link( ps->getPort("out"), fand->getPort("in") );
				link( balzac->getPort("endIt"), fand->getPort("in") );
				link( fand->getPort("out"), matrix->getPort("beginIt") );
			}
		};

		GENCLASS(TargetGroups)
	}
}

