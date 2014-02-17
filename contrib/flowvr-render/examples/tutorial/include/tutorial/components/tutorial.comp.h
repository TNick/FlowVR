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


#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_

using namespace flowvr::app;

namespace tutorial
{

    // Tutorial is a composite component
    class Tutorial : public Composite
    {

        public :
            Tutorial(const std::string id_) : Composite(id_)
        {
            setInfo("Tutorial is a set of basic examples of FlowVR-Render. It shows how create, display and compose FlowVR-Render primitives");
        };

            virtual ~Tutorial(){};

            // Composite components need an execute method. 
            virtual void execute();

            // Mandatory create method
            virtual Component* create() const	{ return new Tutorial(this->getId());	};

    };


};


#endif //_TUTORIAL_H_
