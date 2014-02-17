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

#ifndef BALZACSERVICELAYER_H_
#define BALZACSERVICELAYER_H_


#include <flowvr/moduleapi.h>
#include <flowvr/tinyxml.h>

#include <vector>
#include <flowvr/portutils/portservicelayer.h>



namespace flowvr
{
	namespace render
	{
		namespace balzac
		{
			class Display;
			class Input;
			class PortHandler;
			class IPluginInterface;
			class SystemControl;

			class ServiceLayer : public flowvr::portutils::IServiceLayer
			{
			public:
				ServiceLayer(flowvr::portutils::Module &parent);
				~ServiceLayer();

				virtual bool setParameters( const flowvr::portutils::ARGS & );


				int run();

				Display       *getDisplay() const;
				Input         *getInput() const;
				SystemControl *getControl() const;

			private:
				bool configureDisplay( const flowvr::xml::TiXmlHandle &displayConf );

				flowvr::ModuleAPI    *m_Render;
				Display              *m_display;
				SystemControl        *m_ctrl;
				std::string           m_strConfigFile;
			}; // class


		} // namespace balzac
	} // namespace render
} // namespace flowvr



#endif // SERVICELAYER_H_
