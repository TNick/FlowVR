/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                                                                 *
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
*  Contact :                                                      *
*                                                                 *
******************************************************************/

#ifndef MODULEGENERICVIEWER_COMP_H_
#define MODULEGENERICVIEWER_COMP_H_

#include <flowvr/app/components/module.comp.h>

namespace flowvr
{
	namespace render
	{
			class ModuleGenericViewer : public flowvr::app::Module
			{
				public:
				ModuleGenericViewer(const std::string& id_) : flowvr::app::Module(id_)
				{
					addPort("scene", flowvr::app::OUTPUT);
				}

				virtual Component* create() const
				{
					return new ModuleGenericViewer(getId());
				}
			};
	}
}


#endif // MODULEGENERICVIEWER_COMP_H_
