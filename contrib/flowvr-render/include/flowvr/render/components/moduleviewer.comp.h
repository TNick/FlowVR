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
*    Benjamin Petit,                                              *
*    Jeremie Allard,                                              *
*    Thomas Arcila,                                               *
*    Jean-Denis Lesage.                                           *	
*    Clement Menier,                                              *
*    Bruno Raffin                                                 *
*                                                                 *
*******************************************************************
*                                                                 *
* File: ./include/flowvr/render/components/moduleviewer.comp.h    *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef MODULEVIEWER_COMP_H_
#define MODULEVIEWER_COMP_H_

#include <flowvr/app/components/module.comp.h>

using namespace flowvr::app;

namespace flowvr { namespace render {
	
    // This module can be used for a lot of standard viewer
	class ModuleViewer : public Module
    {
	    public:
		    ModuleViewer(const std::string& id_) :
			    Module(id_)
		    {
                        setInfo("A viewer generates primitives for flowvr-render");
                        addPort("scene", OUTPUT);
                        addPort("matrix", INPUT);
		    }

		    virtual Component* create() const
		    {
			    return new ModuleViewer(getId());
		    }

    };
} }

#endif /*MODULEVIEWER_COMP_H_*/
