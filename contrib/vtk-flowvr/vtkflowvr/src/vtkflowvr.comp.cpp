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


// vtkflowvr components includes
#include "vtkflowvr/components/metamoduleviewervtk.comp.h"

// flowvr-render components includes
#include <flowvr/render/balzac/components/balzaclostinspaceviewer.comp.h>


#include "vtkflowvr/components/vtkflowvr.comp.h"

using namespace flowvr::app;
using namespace flowvr::render;
using namespace flowvr::render::balzac;

namespace vtkflowvr
{


  // Required to enable dynamic component loading
  // Argument given in parameter is the class name (respect case) (also requires a Vtkflowvr(id) constructor)
  GENCLASS(Vtkflowvr)
     

	void Vtkflowvr::execute()
        {
            // This component contains the viewer vtk. This component has a mandatory parameter called VTK_SCRIPT. The value of this parameter is the path to the vtk script to execute
            MetaModuleViewerVTK* viewer = addObject<MetaModuleViewerVTK>("vtk");


            // Add component that renderers the scene. This component receives all primitives on a scene input port
            BalzacLostInSpaceViewer* renderer = addObject<BalzacLostInSpaceViewer>("Renderer");

            //link all viewers to renderer.
            //Remark: All communication schemas dedicated to flowvr-render are included in renderer. It is the component renderer that merges all primitives from different viewers. Renderer component also deals with synchronization issues. You just have to link scenes port to renderer to get your primitives rendered 
            link(viewer->getPort("scene"), renderer->getPort("scene"));

        }
};
