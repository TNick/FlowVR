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


#ifndef _VTKFLOWVR_H_
#define _VTKFLOWVR_H_

using namespace flowvr::app;

namespace vtkflowvr
{

    // Vtkflowvr is a composite component
    class Vtkflowvr : public Composite
    {

        public :
            Vtkflowvr(const std::string id_) : Composite(id_)
        {
            setInfo("Vtkflowvr can execute a vtk script and renders fvrender primitives in a flowvr-render");

        };

            virtual ~Vtkflowvr(){};

            // Composite components need an execute method. 
            virtual void execute();

            // Mandatory create method
            virtual Component* create() const	{ return new Vtkflowvr(this->getId());	};

    };


};


#endif //_VTKFLOWVR_H_
