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

#ifndef _PARTICLESOURCECOMP_H_
#define _PARTICLESOURCECOMP_H_

// core library flowvr-app include
#include <flowvr/app/core/component.h>


namespace particles
{

    // Particles is a composite component
    class ParticleSource : public flowvr::app::Composite
    {
        public :
    		ParticleSource(const std::string &id_);

            virtual ~ParticleSource() {}
            virtual void execute();
            virtual flowvr::app::Component* create() const;
    };
}


#endif // _PARTICLESOURCECOMP_H_
