/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                     Application Library                         *
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
*    Jean-Denis Lesage.                                           *	
*    Clement Menier,                                              *
*    Bruno Raffin                                                 *
*                                                                 *
*******************************************************************
*                                                                 *
*  Contact : Jean-Denis Lesage                                    *
*                                                                 *
******************************************************************/

// flowvr-app core includes
#include <flowvr/app/core/genclass.h>


// tutorial components includes
#include "tutorial/components/viewersgroup.comp.h"

// flowvr-render components includes
#include <flowvr/render/balzac/components/balzaclostinspaceviewer.comp.h>


#include "tutorial/components/tutorial.comp.h"

#include <vector>

using namespace flowvr::app;
using namespace flowvr::render;
using namespace flowvr::render::balzac;

namespace tutorial
{


  // Required to enable dynamic component loading
  // Argument given in parameter is the class name (respect case) (also requires a Tutorial(id) constructor)
  GENCLASS(Tutorial)
     

	void Tutorial::execute()
        {
            ViewersGroup* viewers = addObject<ViewersGroup>("viewers");

            // Add component that renders the scene.
            // This component receives all primitives on a scene input port.
            // for this tutorial, we use a special component consisting of Balzac
            // and its default mouse / keyboard interaction
            BalzacLostInSpaceViewer* renderer = addObject<BalzacLostInSpaceViewer>("renderer");

            // link all viewers to renderer.
            link(viewers->getPort("scenes"), renderer->getPort("scene"));
        }
};
