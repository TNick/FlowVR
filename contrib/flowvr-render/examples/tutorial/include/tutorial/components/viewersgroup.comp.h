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

// core library flowvr-app include
#include <flowvr/app/core/component.h>


#ifndef _VIEWERSGROUP_H_
#define _VIEWERSGROUP_H_

using namespace flowvr::app;

namespace tutorial
{

    // ViewersGroup is a composite component
    class ViewersGroup : public Composite
    {

        public :
            ViewersGroup(const std::string id_) : Composite(id_)
        {
            setInfo("ViewersGroup contains all viewers included in the scene"); 

            addPort("scenes", OUTPUT);

            addParameter("example", "1");
	    //addParameter("textureFileName", "images/exampleTexture.png");
        };

            virtual ~ViewersGroup(){};

            // Composite components need an execute method. 
            virtual void execute();

            // Mandatory create method
            virtual Component* create() const	{ return new ViewersGroup(this->getId());	};

    };


};


#endif //_VIEWERSGROUP_H_
