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
* File: servicelayer.cpp                                          *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include "flowvr/render/balzac/servicelayer/servicelayer.h"

#include "flowvr/render/balzac/servicelayer/display.h"
#include "flowvr/render/balzac/servicelayer/balzacutil.h"
#include "flowvr/render/balzac/servicelayer/systemcontrol.h"

#include <flowvr/portutils/porthandler.h>
#include <flowvr/portutils/portplugin.h>
#include <flowvr/portutils/portmodule.h>
#include <flowvr/portutils/porthandlerregistry.h>
#include <flowvr/portutils/portconfigure.h>

#include <flowvr/render/cg/cgengine.h>
#include <flowvr/utils/timing.h>

#include <flowvr/utils/cmdline.h>
#include <flowvr/tinyxml.h>
#include <cstring>
#include <set>

#include <dlfcn.h>              // POSIX dl stuff


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

DEFIMP_SERVICEPLUG(flowvr::render::balzac::ServiceLayer)




extern "C" void getParameters( flowvr::portutils::ParameterContainer &prm )
{
	prm["displayfile"] = flowvr::portutils::Parameter("config/balzac-config.xml", "absolute or relative path to the balzac-display-file to use.");
	prm["dostats"] = flowvr::portutils::Parameter("false", "enabled statistics in CgEngine");
}


namespace
{
	bool getBool( const std::string &strBool )
	{
		if( strcasecmp( strBool.c_str(), "true" ) == 0 )
			return true;
		else if( strBool == "1" )
			return true;
		return false;
	}
}


namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			ServiceLayer::ServiceLayer( flowvr::portutils::Module &parent )
			: IServiceLayer( parent )
			, m_Render(NULL)
			, m_display( NULL )
			, m_ctrl(NULL)
			{
				int    argc;
				char **argv;

				parent.getArgs(argc, argv);

				glutInit( &argc, argv );
				m_display = new Display( new flowvr::render::cg::CgEngine,
									     new flowvr::render::cg::CgEngine );

				m_ctrl = new SystemControl(*this);

//				bool b=false;
//				while(!b)
//				{
//					flowvr::utils::microsleep(1000);
//				}
			}

			ServiceLayer::~ServiceLayer()
			{
				delete m_ctrl;
				delete m_display;

				if(m_Render)
					m_Render->close();
				delete m_Render;
			}

			bool ServiceLayer::setParameters( const flowvr::portutils::ARGS &arg )
			{
				try
				{
					const flowvr::portutils::Parameter &dispFile = arg("displayfile");
					m_strConfigFile = dispFile.getValueString();

//					bool doStats = arg("dostats").getValue<bool>();
//					m_display->getWorldCgEngine()->setDoStats(doStats);
				}
				catch( flowvr::portutils::ParameterException &e )
				{
					std::cerr << e.what() << std::endl;
					return false;
				}
				return true;
			}

			bool ServiceLayer::configureDisplay( const flowvr::xml::TiXmlHandle &displayConf )
			{
				flowvr::xml::TiXmlHandle window = displayConf.FirstChild("window");
				if(!window.Element())
					return false;
				{
					flowvr::xml::TiXmlHandle width      = window.FirstChild("width").FirstChild();
					flowvr::xml::TiXmlHandle height     = window.FirstChild("height").FirstChild();
					flowvr::xml::TiXmlHandle left       = window.FirstChild("left").FirstChild();
					flowvr::xml::TiXmlHandle top        = window.FirstChild("top").FirstChild();
					flowvr::xml::TiXmlHandle fullscreen = window.FirstChild("fullscreen").FirstChild();
					flowvr::xml::TiXmlHandle title      = window.FirstChild("title").FirstChild();
					flowvr::xml::TiXmlHandle cursor     = window.FirstChild("cursor").FirstChild();

					int x,y,w,h;
					w = (width.Text()  ? atoi(width.Text()->Value()) : 640);
					h = (height.Text() ? atoi(height.Text()->Value()): 480);
					x = (left.Text()   ? atoi(left.Text()->Value())  : 0);
					y = (top.Text()    ? atoi(top.Text()->Value())   : 0);

					int c = (cursor.Text() ? atoi(cursor.Text()->Value()) : 0 );
					c = std::max<int>(0,c);

					bool bFullscreen       = (fullscreen.Text() ? getBool( fullscreen.Text()->Value() ) : false);
					std::string strTitle   = (title.Text() ? title.Text()->Value() : "");

					if( !strTitle.empty() )
						m_display->setTitle( strTitle );

					m_display->setPosition( x,y,w,h );
					m_display->setFullscreen( bFullscreen );
					m_display->setCursor(c);
				}
				return false;
			}


			int ServiceLayer::run()
			{
				m_display->setCommunicator( &getParent().getPortComm() );


				if(!m_strConfigFile.empty())
				{
					std::cout << "balzac was told to use [" << m_strConfigFile << "] as displayfile." << std::endl;
					flowvr::xml::TiXmlDocument config( m_strConfigFile.c_str() );
					if( config.LoadFile() )
					{
						flowvr::xml::TiXmlHandle docHandle( &config );
						flowvr::xml::TiXmlHandle config  = docHandle.FirstChild("config");
						flowvr::xml::TiXmlHandle display = config.FirstChild("display");
						configureDisplay( display );
					}
					else
					{
						std::cerr << "error during parsing of displayfile [" << m_strConfigFile << "]" << std::endl
								  << config.ErrorDesc() << std::endl;
					}
				}

				glutMainLoop();
				return 0;
			}

			Display *ServiceLayer::getDisplay() const
			{
				return m_display;
			}


			Input        *ServiceLayer::getInput() const
			{
				return m_display->getInput();
			}


			SystemControl *ServiceLayer::getControl() const
			{
				return m_ctrl;
			}

		} // namespace balzac
	} // namespace render
} // namespace flowvr





