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

#ifndef BALZACLOSTINSPACEVIEWER_COMP_H_
#define BALZACLOSTINSPACEVIEWER_COMP_H_

#include <flowvr/app/core/component.h>



namespace flowvr
{
	namespace app
	{
		class CmdLine;
	}

	namespace render
	{
		namespace balzac
		{
			/**
			 * @brief implements a *very* simple scene viewer with a lost in space cam,
			 *        keyboard navigation and initial explicit camera positioning
			 */
			class BalzacLostInSpaceViewer : public flowvr::app::Composite
			{
			public:
				BalzacLostInSpaceViewer( const std::string &strId,
										 flowvr::app::Composite *parent = NULL,
						                 flowvr::app::CmdLine *cmdLine = NULL );
				~BalzacLostInSpaceViewer();

				Component *create() const;
				void execute();
				flowvr::app::CmdLine *getRun();

			private:
				flowvr::app::CmdLine *m_run;
			};
		}
	}
}


#endif // BALZACLOSTINSPACEVIEWER_COMP_H_
