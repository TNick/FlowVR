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

#include "tutorial/components/metamoduleviewercube.comp.h"
#include "tutorial/components/metamoduleviewerobj.comp.h"
#include "tutorial/components/metamoduleviewertexture.comp.h"

#include <flowvr/app/components/syncmaxfrequency.comp.h>
#include "tutorial/components/viewersgroup.comp.h"

#include <flowvr/portutils/components/portmodulerun.comp.h>

using namespace flowvr::app;
using namespace flowvr::render;
using namespace flowvr::portutils;


namespace tutorial
{


    void ViewersGroup::execute()
    {
        const unsigned int exampleId = getParameter<unsigned int>("example");
        Component *viewer;

        switch(exampleId)
        {
            default:
            	std::cout<<"Tutorial not yet available"<<std::endl;
            case 1:
			{
				viewer = addObject<MetaModuleViewerCube>("viewercube");
				break;
			}
            case 2:
            {
                viewer = addObject<MetaModuleViewerObj>("viewerobj");
                break;
            }
            case 3:
            {
                viewer = addObject<MetaModuleViewerTexture>("viewertexture");
                break;
            }
            case 4:
            {
				FlowvrRunSSH ssh(this);
            	viewer = new PortModuleRun("multitexture", "multitexture.xml", ssh, this );
            	break;
            }
            case 5:
            {
				FlowvrRunSSH ssh(this);
            	viewer = new PortModuleRun("textureupdate", "textureupdate.xml", ssh, this );

            	// this viewer will put an animation, so bind the frequency accordingly
				SyncMaxFrequency *mf = addObject<SyncMaxFrequency>("mf");
				link( mf->getPort("out"), viewer->getPort("beginIt") );
				link( viewer->getPort("endIt"), mf->getPort("endIt") );
				mf->setParameter<float>("freq", 1.0f); // 1Hz update is enough for this example

            	break;
            }
        }

        link(viewer->getPort("scene"), getPort("scenes"));
    }
};
