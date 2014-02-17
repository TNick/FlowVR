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
 * File: ./include/flowvr/app/components/flowvr-render/modulefwrite.comp.h*
 *                                                                 *
 * Contacts:                                                       *
 *                                                                 *
 ******************************************************************/
#ifndef MODULEFWRITE_COMP_H_
#define MODULEFWRITE_COMP_H_

#include <flowvr/app/components/module.comp.h>
using namespace flowvr::app;
namespace flowvr { namespace modules {
	
    class ModuleFWrite : public Module
    {
        public:
            ModuleFWrite(const std::string& id_) :
                Module(id_)
            {		    
                setInfo("Module writing messages in a file");
                addPort("in", INPUT);
            }

            virtual Component* create() const
            {
                return new ModuleFWrite(getId());
            }
    };
	
} }

#endif /*MODULEFWRITE_COMP_H_*/
