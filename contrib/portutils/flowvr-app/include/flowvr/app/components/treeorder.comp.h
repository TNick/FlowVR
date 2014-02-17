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
*  Original Contributors:  
*    Helene Coullon,                                              *
*    Sebastien Limet,                                             *
*    Sophie Robert,                                               *
*    Emmanuel Melin,                                              *
*                                                                 * 
*******************************************************************
 *                                                                *
 *  File : ./include/flowvr/app/components/treeorder.comp.h    *
 *                                                                *
 * Contacts:                                                      *
*  01/06/2008 Helene Coullon <helene.coullon@univ-orleans.fr>     *
*                                                                 *
******************************************************************/


#ifndef __FLOWVR_APP_TREEORDER_H_
#define __FLOWVR_APP_TREEORDER_H_

#include <climits>

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/connection.comp.h"


/**
 * \file treeorder.comp.h
 * \brief  Tree   communication  pattern
 *
 * Two versions available: 
 * -# connections are automaticaly generated
 * -# connections are specified using a <connect> template 
 *
 * @todo: A  lot of duplicated code. Factorise it to ease maintenance.
 */


namespace flowvr { namespace app {

    /**
     *  \enum  TreeType
     * \brief define if messages flow from root to leaves (gather) or from leaves to root (scatter)
     */
    enum TreeTypeTest{GATHERTEST,SCATTERTEST};

    /**
     *\class Tree with automatic connection insertion
     * \brief  A tree pattern with an "in" and "out" port. Tree arity is a parameter ("TREE_ARITY"). Default arity: infinity.
     *
     * \tparam Node : template for the node to use in the tree. Must have one  "in" and one "out" port. Exemple: use a FilterRoutingNode for a broadcast tree.    We usually use a composite node like FilterMerge that hides the actual ports of the underlying primitive component.
     *
    */
    template<class Node>
        class TreeOrder : public Composite
    {

        private :
            TreeTypeTest treetype;///< tree type (SCATTER/GATHER)

		public :
    			/**
    			 * \brief  Tree constructor
    			 * \param id_   prefix id
    			 * \param treetype_  tree type (GATHER/SCATTER)
    			*/
			   TreeOrder(const std::string& id_, const TreeTypeTest& treetype_) : Composite(id_,"comtree"), treetype(treetype_)
		        {

                setInfo( "Tree pattern used to build communication patterns. It has one 'in' and one 'out' port and an additional input port order. It has 1 parameter: 'TREE_ARITY':  the arity of the  tree (inifinty by default). It is defined using 2 templates. Node is the component used on each tree vertex to gather data, Connect is the communication pattern used on the tree edges.");

                addPort("in", INPUT);
                addPort("order", INPUT);
                addPort("out", OUTPUT);
                addParameter<unsigned int>("TREE_ARITY",UINT_MAX); ///< default arity  parameter set to infinity
            }
			


            /**
             * \brief destructor
             **/
            virtual ~TreeOrder(){}


             /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const
            {
                return new TreeOrder<Node>(getId(), treetype);
            }


             /**
             * \brief execute: recusrive tree construction
             */
            virtual void execute()
            {
                
                std::string leavesPortId = (treetype == GATHERTEST ? "in" : "out");
                std::string rootPortId = (treetype == GATHERTEST ? "out" : "in");
                std::string annexPortId = "order";
                
                

                // compute the list of primitive  components connected to leave port.
                std::list<const Port*> leavesports;
                getPort(leavesPortId)->getPrimitiveSiblingList(leavesports);
                // get arity parameter
                unsigned int arity = getParameter<unsigned int>("TREE_ARITY");

                //std::cout<<"--------------------Number Ports = "<<leavesports.size()<<"------------------------"<<std::endl;

                // Error control:

                // no leave node
                if (leavesports.empty())  throw NoPrimitiveConnectedException(leavesPortId, getFullId(), __FUNCTION_NAME__);

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



                // End recursion: only one leave port. Just link  leave port to root port
                if (leavesports.size() == 1)
                    {
                        // Recursivity. Simple case
                        // Insert an automatic connection (type resolved during the @see resolveMsgType() traverse)
                        ConnectionAuto * ct = addObject<ConnectionAuto>("cTree");
                        link(getPort(leavesPortId), ct->getPort(leavesPortId));
                        ct->getPort(leavesPortId)->addRestrictionToPrimitiveSibling(*(leavesports.begin()));
                        link(getPort(rootPortId), ct->getPort(rootPortId));
			//link(getPort(annexPortId), ct->getPort(annexPortId));
                    }
                else
                    {

                        // std::cout << "******************************** leavesports.size() != 1 " << std::endl;
                        // More than  one leave port:
                        //  1) Add a node and a connection between  node  and  root
                        //  2) Split the leave ports in arity groups  and recursively build a tree on each groups.
                        //  3) Link each sub tree component to the leave port of the node.


                        Node* node = addObject<Node>("Node"); ///<  add a node

                        // Link node to root 
                         link(getPort(rootPortId), node->getPort(rootPortId));
                         link(getPort(annexPortId), node->getPort(annexPortId));

                        //real arity = min(arity, leavesports.size()) -- because if artiy == UINT_MAX, it could take all memory!
                        unsigned int realArity = arity;
                        if (leavesports.size() <= realArity)
                        {
                                realArity = leavesports.size();
                        }

                        // Split into realArity groups  and call the algorithm recursively on each group
                        // If tree high is n, then split making all groups, except last one, of size arity^(n-1)
                        std::vector<std::list<const Port*> > listArityLeaves(realArity);///< to store  sort the leaves ports
                        unsigned int i = 0;
                        unsigned int j = 0;
                        // Split in groups closer to the  have splitsize
                        unsigned int splitsize=realArity;
                        for(; splitsize < leavesports.size();splitsize*=realArity){};
                        splitsize =  splitsize / realArity;

                        for (std::list<const Port*>::iterator it = leavesports.begin(); it != leavesports.end(); ++it)
                            {
                                listArityLeaves[j].push_back(*it);
                                i++;
                                if ( i % splitsize == 0) j++;
                            }

                        // Add a sub tree for each group (recursive tree construction)
                        for(std::vector<std::list<const Port*> >::iterator it = listArityLeaves.begin(); it != listArityLeaves.end(); ++it)
                            {
                                if (!it->empty())
                                    {
                                        Component* c= NULL;
                                        // Sub Tree
                                        c = addObject(TreeOrder<Node>("Tree"+toString<unsigned int>(it - listArityLeaves.begin()), treetype));
                                        // link sub tree root port to node root port
                                        link(c->getPort(rootPortId), node->getPort(leavesPortId));
                                        link(getPort(annexPortId), c->getPort(annexPortId));
                                        // link sub tree leave ports to leave ports of the group
                                        for(std::list<const Port*>::iterator itPort = it->begin(); itPort != it->end(); ++itPort)
                                            {
                                                link(getPort(leavesPortId), c->getPort(leavesPortId));
                                                c->getPort(leavesPortId)->addRestrictionToPrimitiveSibling(*itPort);
                                            }
                                    }
                            }
                    }

            };


            /**
              * \brief Host mapper using the hosts of the leaves components to set the hosts of the tree nodes.
              **/
             virtual void setHosts()
             {
             	// List of primitives outside of the current composite object connected to the "leaves" port
             	std::string leavesPortId = (treetype == GATHERTEST ? "in" : "out");

             	std::list<const Port*> primitivePorts;
             	getPort(leavesPortId)->getPrimitiveSiblingList(primitivePorts);
             	if (primitivePorts.empty())
             		throw NoPrimitiveConnectedException(leavesPortId, getFullId(), __FUNCTION_NAME__);

             	if (primitivePorts.size() > 1)
             	{
             		if (getComponent("Node") )
             		{
             			bool nodeIsMapped = false;
             			std::list<const Port*>::iterator it = primitivePorts.begin();
             			do
             			{
             				Component* owner = (*it)->getOwner();
             				// Case of connection...
             				if(dynamic_cast<ConnectionBase*>(owner) )
             				{
             					// Go to the next neighbor
             					Port* otherSide;
             					if ((*it)->getId() == "in")
             					{
             						otherSide = owner->getPort("out");
             					}
             					else
             					{
             						otherSide = owner->getPort("in");
             					}
             					std::list<const Port*> primlist;
             					otherSide->getPrimitiveSiblingList(primlist);
             					owner = (*(primlist.begin()))->getOwner();
             				}

             				if (!owner->hostList.empty())
             				{
             					getComponent("Node")->hostList= owner->hostList.front();
             					getComponent("Node")->hostList.front().appendtoOrigin(owner->getFullId());
             					nodeIsMapped = true;
             					// Msg::debug("Automatic Mapping : Id = " + getFullId() + "/Node mapped on " + *(hostListSib.begin()), __FUNCTION_NAME__);
             				}
             				++it;
             			}
             			while( (!nodeIsMapped) && it != primitivePorts.end());

             			if (!nodeIsMapped)
             				throw CustomException("Component id=" + getFullId() + ": No host for mapping", __FUNCTION_NAME__);
             		}
             	}

            };
    };


}; };
#endif //__TREEORDER_H_
