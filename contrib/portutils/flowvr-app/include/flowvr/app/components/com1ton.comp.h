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
 *  File : ./include/flowvr/app/components/com1ton.comp.h          *
 *                                                                 *
 *  Contact : Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/



#ifndef __FLOWVR_APP_COM1TON_H_
#define __FLOWVR_APP_COM1TON_H_

#include "flowvr/app/components/tree.comp.h"

/**
 * \file com1ton.comp.h
 * \brief Communication pattern 1 ("in" port) to N ("out" port)
 *
 * Two versions available: 
 * -# connections are automaticaly generated
 * -# connections are specified using a <connect> template 
 *
 * It is defined using 2 templates. Node is the component used on
 * each tree vertex to scatter data, Connect is the communication
 * pattern used on the tree edges. Implements a simple broadcast
 * when used with a FilterRoutingNode and a Connection (full or stamps).
 */

namespace flowvr
{
  namespace app
  {
    /**
     * @class Com1toN
     * @brief  com 1 to N with automatic connection generation
     *
     * \param Node  : node component used at each  tree vertex
     */

    // For backward compatibility
       #define Com1ToN Com1toN

    template <class Node>
        class Com1toN : public Tree<Node>
    {
        public :

            /**
             * \brief Constructor.  1 (in port) to N (out port)   communication pattern based on a tree.
             * By default, the TREE_ARITY is set to infinity. In that case, the Node parameter is not
             * used at all, this instance implements a simple broadcast then.
             * \param id the component id
             */
            Com1toN(const std::string& id_) : Tree<Node>(id_, SCATTER)
            {
                this->setInfo("Communication patter that scatter  messages received on 'in'\
							port on the 'out' port. It has 1 parameter: 'TREE_ARITY' the arity of \
							the scattering tree.");
            }

            /**
             * \brief destructor
             */
            virtual ~Com1toN() {}

            /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const
            {
                return new Com1toN<Node>(this->getId());
            }

    };


  } // namespace app
} // namespace flowvr

#endif //__COM1TON_H_
