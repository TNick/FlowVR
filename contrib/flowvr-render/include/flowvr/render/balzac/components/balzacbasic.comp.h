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

#ifndef BALZACBASIC_COMP_H_
#define BALZACBASIC_COMP_H_

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
			class FilterMultiplexMerge;

			/**
			 * @brief wraps a renderer and an optional FilterMultiplexMerge to
			 *        automatically merge scene parts coming from different viewers.
			 *
			 * The component allows to trigger the render loop from extern, use a stamp
			 * message on the <i>render.trigger</i> port of this component to trigger
			 * the rendering. In case there is no connection to that port, the rendering
			 * will be done either
			 * - on the base of a new scene update (blocking balzac scene port given)
			 * - on the base of the rendering speed of balzac (non-blocking balzac scene port given)
			 *
			 * Parameters to this composite are:
			 * - There are no parameters to this composite.
			 *
			 * Constant ports of this composite are
			 * - input
			 *   - scene (flowvr-render-chunks)
			 *   - beginIt (forwarded to balzac.beginIt)
			 *   - render.trigger (when present, use as forward to filter-multiplex merge).
			 *     This port is only effective, when there is more than one scene attached.
			 *     Otherwise, you can use beginIt to do the same.
			 * - output
			 *   - scene_out (flowvr-render-chunks, simply the mirrored output of scene)
			 *   - endIt (forwarded from balzac)
			 *
			 * Additionally, the composite offers
			 * - all ports that are defined by the balzac configuration file
			 *  - that do not conflict in name with one of the above
			 *  - that are properly marked for export in the interface definition
			 */
			class BalzacBasic : public flowvr::app::Composite
			{
			public:
				BalzacBasic( const std::string &id,
						     const std::string &strPortFile,
						     flowvr::app::Composite *parent = NULL,
						     flowvr::app::CmdLine *run = NULL );

				~BalzacBasic();

				virtual void execute();
				virtual Component *create() const;
			private:
				FilterMultiplexMerge *createMMP( Composite *balzac );
				std::string m_interfaceKey, m_sceneportname;

				flowvr::app::CmdLine *m_run;
			};
		}
	}
}


#endif // BALZACBASIC_COMP_H_
