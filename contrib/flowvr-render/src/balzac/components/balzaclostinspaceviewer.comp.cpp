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
* File: balzaclostinspaceviewer.comp.cpp                          *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include "flowvr/render/balzac/components/balzaclostinspaceviewer.comp.h"
#include <flowvr/app/core/run.h>
#include <flowvr/portutils/components/portmodulerun.comp.h>
#include <flowvr/render/balzac/components/balzacbasic.comp.h>

using namespace flowvr::app;
using namespace flowvr::portutils;

namespace flowvr
{ 
	namespace render
	{
		namespace balzac
		{
		BalzacLostInSpaceViewer::BalzacLostInSpaceViewer( const std::string &id, Composite *parent, CmdLine *run )
		: Composite(id)
		, m_run( run ? (CmdLine*)run->clone() : NULL )
		{
			addPort( "scene", INPUT, FULL );
			addPort( "endIt", OUTPUT, STAMPS );
			addPort( "scene_out", OUTPUT, FULL );
			addPort( "viewport", OUTPUT, FULL );
			addPort( "camstate", OUTPUT, FULL );
        
			addParameter( "interface", "balzac-normal.xml" );
			addParameter( "navigate", "balzac-lostinspacenavigate.xml");

			if( m_run == NULL )
			{
				FlowvrRunSSH *ssh = new FlowvrRunSSH(this);
				m_run = ssh;
			
			}

			if( parent )
				parent->insertObject(this);
		}

		BalzacLostInSpaceViewer::~BalzacLostInSpaceViewer()
		{
			delete m_run;
		}

		Component *BalzacLostInSpaceViewer::create() const
		{
			return new BalzacLostInSpaceViewer( getId() );
		}

		CmdLine *BalzacLostInSpaceViewer::getRun()
		{
			return m_run;
		}



		void  BalzacLostInSpaceViewer::execute()
		{
			//////////////////////////////////////////////////////////////////////
			// the basic scene viewer
			//////////////////////////////////////////////////////////////////////
			std::string portfile  = getParameter<std::string>("interface");
			std::string navigate  = getParameter<std::string>("navigate");
			
            //BalzacBasic *balzac = new BalzacBasic( "BalzacBasic", portfile, this );
	        BalzacBasic *balzac = new BalzacBasic( "BalzacBasic", portfile, this, m_run );

			link( getPort("scene"), balzac->getPort("scene") );
			link( balzac->getPort("endIt"), getPort("endIt") ); // forward endIt

			PortModuleRun *navcomp = new PortModuleRun( "navigate", navigate, *m_run, this );

			link( balzac->getPort("devices"), navcomp->getPort("devices") );
            
            
			link( balzac->getPort("viewport"), navcomp->getPort("viewport") );
			link( navcomp->getPort("scene"), balzac->getPort("scene") );

			// forward scene_out
			link( balzac->getPort("scene_out"), getPort("scene_out") );
			link( balzac->getPort("viewport"), getPort("viewport") );

			if( navcomp->hasPort("transform") )
				link( navcomp->getPort("transform"), getPort("camstate") );
		}


		}
	}
}
