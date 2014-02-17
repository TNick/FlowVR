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
 *  File : ./include/flowvr/app/components/tree.comp.h             *
 *                                                                 *
 *  Contact : Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP_TREE_H_
#define __FLOWVR_APP_TREE_H_

#include <climits>

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/connection.comp.h"


/**
 * \file tree.comp.h
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
    enum TreeType{GATHER,SCATTER};

    /**
     *\class Tree with automatic connection insertion
     * \brief  A tree pattern with an "in" and "out" port. Tree arity is a parameter ("TREE_ARITY"). Default arity: infinity.
     *
     * \tparam Node : template for the node to use in the tree. Must have one  "in" and one "out" port. Exemple: use a FilterRoutingNode for a broadcast tree.    We usually use a composite node like FilterMerge that hides the actual ports of the underlying primitive component.
     *
    */
    template<class Node>
        class Tree : public Composite
    {

        private :
            TreeType treetype;///< tree type (SCATTER/GATHER)

		public :
    			/**
    			 * \brief  Tree constructor
    			 * \param id_   prefix id
    			 * \param treetype_  tree type (GATHER/SCATTER)
    			*/
			   Tree(const std::string& id_, const TreeType& treetype_) : Composite(id_,"comtree"), treetype(treetype_)
		        {

                setInfo( "Tree pattern used to build communication patterns. It has one 'in' and one 'out' port. It has 1 parameter: 'TREE_ARITY':  the arity of the  tree (inifinty by default). It is defined using 2 templates. Node is the component used on each tree vertex to gather data, Connect is the communication pattern used on the tree edges.");

                addPort("in", INPUT);
                addPort("out", OUTPUT);
                addParameter<unsigned int>("TREE_ARITY",UINT_MAX); ///< default arity  parameter set to infinity
		addParameter<std::string>("Host","");		   ///< default host is empty. If host not empty, all the tree is host on "Host"
            }
			


            /**
             * \brief destructor
             **/
            virtual ~Tree(){}


             /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const
            {
                return new Tree<Node>(getId(), treetype);
            }


             /**
             * \brief execute: recusrive tree construction
             */
            virtual void execute()
            {
                
                std::string leavesPortId = (treetype == GATHER ? "in" : "out");
                std::string rootPortId = (treetype == GATHER ? "out" : "in");
                
                

                // compute the list of primitive  components connected to leave port.
                std::list<const Port*> leavesports;
                getPort(leavesPortId)->getPrimitiveSiblingList(leavesports);
                // get arity parameter
                unsigned int arity = getParameter<unsigned int>("TREE_ARITY");

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
                                        c = addObject(Tree<Node>("Tree"+toString<unsigned int>(it - listArityLeaves.begin()), treetype));
                                        // link sub tree root port to node root port
                                        link(c->getPort(rootPortId), node->getPort(leavesPortId));
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
            	std::string leavesPortId = (treetype == GATHER ? "in" : "out");

            	Component* owner;

		std::string userHost = getParameter<std::string>("Host");
		bool useUserHost = userHost != "";

            	// List of primitives outside of the current composite object connected to the "leaves" port
            	std::list<const Port*> primitivePorts;
            	getPort(leavesPortId)->getPrimitiveSiblingListSkipConnections(primitivePorts);
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
            				// Look for the first primitive (connections are ignored)
            				// with a non empty host list and use the host available to map the "Node" component.
            				owner = (*it)->getOwner();
            				if (!owner->hostList.empty())
            				{
						//if(useUserHost)
						//	getComponent("Node")->hostList.push_back(userHost);
						//else
            						getComponent("Node")->hostList.push_back(owner->hostList.front());
            					getComponent("Node")->hostList.front().appendtoOrigin(owner->getFullId());
            					nodeIsMapped = true;
            					Msg::debug("Host mapping : Id = " + getFullId() + "/Node  mapped on " + getComponent("Node")->hostList.front().getName(), __FUNCTION_NAME__);
            				}
            				++it;
            			}
            			while( (!nodeIsMapped) && it != primitivePorts.end());

            			if (!nodeIsMapped)
            				throw CustomException("Component id=" + getFullId() + "/Node : No host for mapping", __FUNCTION_NAME__);
            		}
            	}

            };
    };


}; };
#endif //__TREE_H_
