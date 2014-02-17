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
 *  File : ./include/flowvr/app/components/comnton.comp.h          *
 *                                                                 *
 *  Contact : Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP_COMNTON_H_
#define __FLOWVR_APP_COMNTON_H_

#include <flowvr/app/components/patternparallelfromports.comp.h>
#include <flowvr/app/core/data.h>
#include <flowvr/app/core/component.h>

/**
 * \file comnto1.comp.h
 * \brief N to N communication  pattern
 */


namespace flowvr { namespace app {


    /**
     * \class ComNtoN
     * \brief Communication patern N ("in" port) to N ("out" port)
     */


    // For backward compatibility
    #define ComNToN ComNtoN

    template <class Connect>
        class ComNtoN : public PatternParallelFromPorts<Connect>
    {

        public :

            /**
             * \brief Constructor. N (in port) to N (out port)  communication pattern based on a tree.
             * \tparam Connect : communication component to use for each point-to-point connection
             * \param id_ the component id
             */
            ComNtoN(const std::string& id_) : PatternParallelFromPorts<Connect>(id_)
            {
                Component::setInfo("N to N (point-to-point) communication. 1 parameter (inherited from PatternParallel). 'instances_from_port' : name of the port to look at to define N (number of primitive components connected). 1 template. 'Connect': the communication component to use for each point-to-point connection");
            };

            /**
             * \brief destructor
             */
            virtual ~ComNtoN(){};

            /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const { return new ComNtoN<Connect>(this->getId()); };

    };

}; };

#endif //__COMNTON_H_
