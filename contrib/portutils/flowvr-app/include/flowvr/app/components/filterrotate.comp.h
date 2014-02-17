/******* COPYRIGHT ************************************************
*                                                                 *
*                         FlowVR VRPN                             *
*                    FlowVR Coupling Modules                      *
*                                                                 *
*-----------------------------------------------------------------*
 * COPYRIGHT (C) 2003-2011                by                       *
* Laboratoire d'Informatique Fondamentale d'Orleans               *
* (EA 4022) ALL RIGHTS RESERVED.                                  *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING-LIB file for further information.                       *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Sebastien Limet,                                             *
*    Sophie Robert,                                               *
*    Emmanuel Melin,                                              *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: ./include/flowvr/app/components/filterrotate.comp.h       *
*                                                                 *
* Contacts:                                                       *
*  01/06/2008 Helene Coullon <helene.coullon@univ-orleans.fr>     *
*                                                                 *
******************************************************************/

#ifndef __FILTERROTATE_H
#define __FILTERROTATE_H

#include "flowvr/app/core/component.h"


/**
 * \file filterrotate.comp.h
 * \brief rotate filter: composite shell  encapsulating a filterrotateprimitive
*/

namespace flowvr { namespace app {


    /**
     * \class Filterrotate
     */
    class FilterRotate : public Composite
    {
        public :

            /**
             * \brief constructor with an id
             */
            FilterRotate(const std::string& id_) : Composite(id_, "Shell")
            {
		setInfo("A shell around a filterrotate component with one output port  masking the actual number of ports  of filterrotate");

                addPort("in", INPUT);
                addPort("out", OUTPUT);

            }

            /**
             * \brief add a FilterrotatePrimitive with nb inputs
             *  nb defined according to primitive objects
             */
            virtual void execute();

            virtual Component* create() const	{	return new FilterRotate(getId());		};

    };


};}; // end namespace

#endif // __FILTERROTATE_H


