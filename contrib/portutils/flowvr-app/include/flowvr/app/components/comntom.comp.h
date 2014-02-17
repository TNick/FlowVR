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
 *  File : ./include/flowvr/app/components/comntom.comp.h          *
 *                                                                 *
 *  Contact : Dreher Matthieu                                      *
 *                                                                 *
 ******************************************************************/



#ifndef __FLOWVR_APP_COMNTOM_H_
#define __FLOWVR_APP_COMNTOM_H_

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/connection.comp.h"
#include "flowvr/app/components/comnto1.comp.h"

/**
 * \file comntom.comp.h
 * \brief Communication pattern N ("in" port) to M ("out" port)
 *
 * Communication pattern design to partially reduce a flow from N sources
 * to M destination with N >= M. It is designed to be a point to point
 * communication. Internally, this pattern use the ComNto1 to
 * merge the communications.
 */

namespace flowvr
{
  namespace app
  {
    /**
     * @class ComNtoM
     * @brief  com N to M with automatic connection generation
     *
     * \param Node  : node component used at each  tree vertex
     */


    template <class Node>
        class ComNtoM : public Composite
    {
        public :

            /**
             * \brief Constructor.  N (in port) to N (out port)   communication pattern based on a tree.
             * By default, the TREE_ARITY is set to infinity. In that case, the Node parameter is not
             * used at all, this instance implements a simple broadcast then.
             * \param id the component id
             */
            ComNtoM(const std::string& id_) : Composite(id_,"ComNtoM")
            {
                this->setInfo("Communication patter that partially gather  messages received on 'in'\
							port on the 'out' port. It has 1 parameter: 'TREE_ARITY' the arity of \
							the scattering tree.");
		addPort("in", INPUT);
                addPort("out", OUTPUT);
                addParameter<unsigned int>("TREE_ARITY",UINT_MAX); ///< default arity  parameter set to infinity
            }

            /**
             * \brief destructor
             */
            virtual ~ComNtoM() {}

            /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const
            {
                return new ComNtoM<Node>(this->getId());
            }

	    /**
             * \brief execute: recusrive tree construction
             */
            virtual void execute()
            {
                
                std::string leavesPortId = "in";
                std::string rootPortId = "out";
                
                

                // compute the list of primitive  components connected to leave port.
                std::list<const Port*> leavesports;
                this->getPort(leavesPortId)->getPrimitiveSiblingList(leavesports);

		// compute the list of primitive  components connected to leave port.
                std::list<const Port*> rootsports;
                this->getPort(rootPortId)->getPrimitiveSiblingList(rootsports);

                // get arity parameter
                unsigned int arity = this->getParameter<unsigned int>("TREE_ARITY");

                // Error control:

                // no leave node
                if (leavesports.empty())  throw NoPrimitiveConnectedException(leavesPortId, this->getFullId(), __FUNCTION_NAME__);

                // Arity must be >0
                if (arity == 0)
                    {
                        throw IncorrectValueException("arity", __FUNCTION_NAME__);
                    }
                // If arity == 1  then only one leave node expected
                if (arity == 1 &&  leavesports.size() !=1 )
                    {
                        throw IncorrectValueException("arity", __FUNCTION_NAME__);
                    }

		if ( leavesports.size() < rootsports.size() )
		{
			throw IncorrectValueException("N < M", __FUNCTION_NAME__);
		}



                // Case Com1To1 (because N >= M and N = 1)
                if (leavesports.size() == 1)
                {
                        // Insert an automatic connection (type resolved during the @see resolveMsgType() traverse)
                        ConnectionAuto * ct = addObject<ConnectionAuto>("cTree");
                       	link(this->getPort(leavesPortId), ct->getPort(leavesPortId));
                       	ct->getPort(leavesPortId)->addRestrictionToPrimitiveSibling(*(leavesports.begin()));
                       	link(this->getPort(rootPortId), ct->getPort(rootPortId));
		 	ct->getPort(rootPortId)->addRestrictionToPrimitiveSibling(*(rootsports.begin()));

                }
                else
                    {

                        // More than  one leave port:
                        // 1) Subdivide the N entry ports in M groups
			// 2) Connect each group with 1 output port with a ComNto1 pattern

			//Compute the groupesize. The first offset groupe will have groupsize+1 ports and the others groupsize ports
			unsigned int groupsize = leavesports.size() / rootsports.size();
			unsigned int offset = leavesports.size() % rootsports.size();

			//Assign the ports to the different groups
			std::vector<std::list<const Port*> > listGroupLeaves(rootsports.size());///< to store  sort the leaves ports
			unsigned int i = 0;	//Port counter for a group
                        unsigned int j = 0;	//Current group
			for (std::list<const Port*>::iterator it = leavesports.begin(); it != leavesports.end(); ++it)
                        {
                        	listGroupLeaves[j].push_back(*it);
                                i++;
                                if (!( i < groupsize || (j < offset && i < groupsize+1))){ 
					j++;
					i = 0;
				}
                        }

			
                        // Add a ComNto1 for each group
			std::list<const Port*>::iterator itRoot = rootsports.begin();
                        for(std::vector<std::list<const Port*> >::iterator it = listGroupLeaves.begin(); it != listGroupLeaves.end(); ++it)
                            {
                                if (!it->empty())
                                    {
                                        Component *c;
                                        // Create and configure the subtree
                                        c = addObject(ComNTo1<Node>("ComNto1/"+toString<unsigned int>(it - listGroupLeaves.begin())));
					setParameter<unsigned int>("TREE_ARITY", arity,"ComNto1/"+toString<unsigned int>(it - listGroupLeaves.begin()));

                                        // link sub tree root port to node root port
                                        link(c->getPort(rootPortId), getPort(rootPortId));
					//Add a restriction so the output of comNto1 is connected only on the output port corresponding to his group
					c->getPort(rootPortId)->addRestrictionToPrimitiveSibling(*(itRoot));
					itRoot++;

                                        // link sub tree leave ports to leave ports of the group
                                        for(std::list<const Port*>::iterator itPort = it->begin(); itPort != it->end(); ++itPort)
                                            {
                                                link(this->getPort(leavesPortId), c->getPort(leavesPortId));
                                                c->getPort(leavesPortId)->addRestrictionToPrimitiveSibling(*itPort);
                                            }
                                    }
                            }
                    }

            };

    };


  }; // namespace app
}; // namespace flowvr

#endif //__COMNTOM_H_
