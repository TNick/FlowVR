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
 *    Jean-Denis Lesage,                                           *
 *    Benjamin Petit,                                              *
 *    Clement Menier,                                              *
 *    Bruno Raffin                                                 *
 *                                                                 *
 *******************************************************************
 *                                                                 *
 *  Contact : Benjamin Petit                                       *
 *                                                                 *
 ******************************************************************/

#ifndef _PARTICLESCOMP_H_
#define _PARTICLESCOMP_H_

// core library flowvr-app include
#include <flowvr/app/core/component.h>


namespace particles
{

    // Particles is a composite component
    class Particles : public flowvr::app::Composite
    {
        public :
            Particles(const std::string &id_);

            virtual ~Particles() {};

            // Composite components need an execute method.
            virtual void execute();

            // Mandatory create method
            virtual flowvr::app::Component* create() const	{ return new Particles(this->getId());	};
    };
}


#endif //_PARTICLES_H_
