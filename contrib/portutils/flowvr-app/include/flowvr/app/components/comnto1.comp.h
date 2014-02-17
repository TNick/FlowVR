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
 *  File : ./include/flowvr/app/components/comnto1.comp.h          *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/



#ifndef __FLOWVR_APP_COMNTO1_H_
#define __FLOWVR_APP_COMNTO1_H_

#include "flowvr/app/components/tree.comp.h"


/**
 * \file comnto1.comp.h
 * \brief N to 1 communication  pattern 
 *
 * Two versions available: 
 * -# connections are automaticaly generated
 * -# connections are specified using a <connect> template 
 *
 * It is defined using 2 templates. Node is the component used on each tree vertex to
 * gather data, Connect is the communication pattern used on the tree edges.
 */


namespace flowvr
{
  namespace app
  {


    /**
     * \class ComNto1
     * \brief Communication pattern N ("in" port) to 1 ("out" port) with automatic connection generation
     * 
     * \param Node  : node component used at each  tree vertex
     */


    // For backward compatibility
    #define ComNTo1 ComNto1

    template <class Node>
        class ComNto1 : public Tree<Node>
    {

        public :


            /**
             * \brief Constructor. N (in port) to 1 (out port)  communication pattern based on a tree.
             * \param id_ the component id
             * \param Node  : node component used at each  tree vertex
             */
            ComNto1(const std::string& id_) : Tree<Node>(id_, GATHER)
            {
                this->setInfo("Communication pattern that gathers  messages on leave port 'in' \
							and forwards them on the  port 'out'. It has 1 parameter: 'TREE_ARITY' \
							the arity of the tree.");
            };

            /**
             * \brief destructor
             */
            virtual ~ComNto1() {}

            /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const { return new ComNto1<Node>(this->getId()); }
    };


 
  } // namespace app
} // namespace flowvr

#endif //__COMNTO1_H_
