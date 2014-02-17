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
 * File: ./include/flowvr/app/components/flowvr-render/modulefread.comp.h*
 *                                                                 *
 * Contacts:                                                       *
 *                                                                 *
 ******************************************************************/
#ifndef MODULEFREAD_COMP_H_
#define MODULEFREAD_COMP_H_

#include <flowvr/app/components/module.comp.h>

namespace flowvr { namespace modules {
	
    class ModuleFRead : public Module
    {
        public:
            ModuleFRead(const std::string& id_) :
                Module(id_)
            {		    
                setInfo("Module reading messages in a file");
                addPort("out", OUTPUT);
            }

            virtual Component* create() const
            {
                return new ModuleFRead(getId());
            }
    };
	
} }

#endif /*MODULEFREAD_COMP_H_*/
