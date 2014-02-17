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
 * File: ./src/port.cpp                                            *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/


#include "flowvr/app/core/port.h"
#include "flowvr/app/core/exception.h"
#include "flowvr/app/core/component.h"
#include "flowvr/app/components/connection.comp.h"
#include "flowvr/app/core/msg.h"
#include <algorithm>

namespace flowvr{ namespace app {
                
    Port::~Port()
    {
        // Erase all pointers owned by other components and pointing to the component to be deleted.

        if (clone  != NULL)   clone->clone = NULL;
        
        for(Port::ParentLinksIterator it = getParentLinksBegin(); it != getParentLinksEnd(); ++it)
            {
                if ( *it != NULL)
                    (* it)->delLink(this);
            }

        for(Port::SiblingLinksIterator it = getSiblingLinksBegin(); it != getSiblingLinksEnd(); ++it)
            {
                if ( *it != NULL)
                    (* it)->delLink(this);
            }
        // remove pointer to current port in distant reverseResPrimSib lists.
        for( std::list<const Port*>::iterator it = restrictedPrimitiveSiblings.begin(); it != restrictedPrimitiveSiblings.end(); ++it)
            {
                (*it)->reverseResPrimSib.remove(this);
            }

        // remove pointer to current port in distant restrictedPrimitiveSiblings lists.
        for( std::list<const Port*>::iterator it = reverseResPrimSib.begin(); it != reverseResPrimSib.end(); ++it)
            {
                (*it)->restrictedPrimitiveSiblings.remove(this);
            }
    };

 
    std::string Port::getIsBlockingString()  const
    {
        switch( getBlockingState() )
            {
            case ST_BLOCKING:
                return "blocking";
                break;
            case ST_NONBLOCKING:
                return "nonblocking";
                break;
            case ST_UNKNOWN:
                return "unknown";
                break;
            default:
                return "ERROR";
            }
    }

    std::string Port::getMsgTypeString() const
    {
        switch ( getMsgType() ) 
            {
            case AUTO: 
                return  "AUTO";
                break;
            case SAMEAS:
                return "SAMEAS";
                break;
            case FULL:
                return  "FULL";
                break;
            case STAMPS:
                return  "STAMPS";
                break;
            default:
            	break;
            };
        return  "ERROR";
    }


    flowvr::xml::DOMElement* Port::XMLBuild() 
    {
        flowvr::xml::DOMElement * e = new flowvr::xml::DOMElement("port");
        e->SetAttribute("id",getId());

        if( getType() == INPUT )
            {
                e->SetAttribute("blockstate", getIsBlockingString());
            }
        
        if ( getMsgType() == FULL)
            {
                // note: FULL/STAMP only differanciate by the presence  or absence of the datatype tag
            e->LinkEndChild(new flowvr::xml::DOMElement("datatype"));
            }

        std::list<const Port*> primlist;
        getPrimitiveSiblingList(primlist);
        


        // Check their neither  AUTO nor SAMEAS port left unless some ports are not connected.
        // This piece of code is  useless as AUTO and SAMEAS are removed when applying the @see resolveMsgTypes controller. Keep it for extra safety.

        // AUTO port: test if connected or not

        if ( getMsgType() == AUTO)
            {
                if( !primlist.empty() ) 
                    {
                        // connected port
                        throw CustomException("Port p=("+getOwner()->getFullId()+", "+getId()+"): has the AUTO  message type and his connected to at least one primitive component. Should be resolved to FULL or STAMPS now.",__FUNCTION_NAME__);
                    }
                else
                    {
                        setMsgType(FULL); //Unconnected port. Set it to FULL so the application can be launched.
                    }
            }
                
        //SAMEAS port: test if SAMESAS target is connected or not
        
        if ( getMsgType() == SAMEAS)
            {
                std::list<const Port*> sameasprimlist;
                getOwner()->getPort(getSameAs())->getPrimitiveSiblingList(sameasprimlist);
                if (  !sameasprimlist.empty() )
                    {
                        // SAMEAS target connected: error
                        if (getOwner()->getPort(getSameAs())->getMsgType() == AUTO)
                            throw CustomException("Port ("+getOwner()->getFullId()+", "+getId()+"): has SAMEAS message type and the SAMEAS target port ("
                                                  +getOwner()->getFullId()+", "+ getOwner()->getPort(getSameAs())->getId()
                                                  +") has the AUTO message type. Should be both resolved now",__FUNCTION_NAME__);
                        else
                            throw CustomException("Port p=("+getOwner()->getFullId()+", "+getId()+"): has SAMEAS message type. Should be "
                                                  +getOwner()->getPort(getSameAs())->getMsgTypeString() 
                                                  +" like the target port p=("+getOwner()->getFullId()+", "+ getOwner()->getPort(getSameAs())->getId()+")",__FUNCTION_NAME__);
                    }
                else
                    {
                        // SAMEAS target not connected: set current port to FULL
                        setMsgType(FULL); //Unconnected port. Set it to FULL so the application can be launched.
                    }
            }

 
        // Make sure that if primitive and input it is connected to at most one incoming primitive component
        if ( ! getOwner()->isComposite() && type== INPUT)
            {
                // Test if  more than one incoming primitive component
                if ( primlist.size() > 1)
                    {
                        std::string list("");
                        for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
                            {
                                list += "\t- (";
                                list += (*it)->getOwner()->getFullId();
                                list += ", " + (*it)->getId() + ")\n";
                            }
                        throw CustomLinkException("Port  p=("+ getOwner()->getFullId() + ", " +getId() +"): linked to multiple primitive components (at most one expected):\n"+list, __FUNCTION_NAME__);
                    }
            }
        return e;
    };
		

    std::string Port::getSameAs() const 
    { 
        if ( getMsgType() != SAMEAS)
            throw  CustomException("Port p=("+getOwner()->getFullId()+", "+getId()+"): cannot call this method if the port message type is not 'SAMEAS'",__FUNCTION_NAME__);
        return sameas; 
    };



    void Port::setMsgType(const  PortMessageType m, const std::string& s)  
    {
		msgType = m;

		if (m == SAMEAS)
		{
			if (!s.empty())
				sameas = s;
			else
			{
				throw CustomException(
						"Port p=(" + getOwner()->getFullId() + ", " + getId()
								+ "): no port name given through optional argument. Expected because port as SAMEAS message type",
						__FUNCTION_NAME__);
			}
		}
		else
		{
			// not sameas port. s should be empty
			if (!s.empty())
			{
				throw CustomException(
						"Port p=(" + getOwner()->getFullId() + ", " + getId()
								+ "): provide value '" + s
								+ "' to optional argument while none expected (not a SAMEAS port)",
						__FUNCTION_NAME__);
			}
		}
    };





    bool Port::isSibling(Port* p)  const 
    {
        if (!getOwner()->getParent() || !p->getOwner()->getParent())
            return false;
        return *(p->getOwner()->getParent()) == *(getOwner()->getParent()); 
    };
	
    bool Port::isCompatibleSibling(Port* p)  
    {
        TypeComponent p_Owner_Type = p->getOwner()->getTypeComponent();
        TypeComponent Owner_Type = getOwner()->getTypeComponent();

        p->getOwner()->setCacheTypeComponent(p_Owner_Type);
        getOwner()->setCacheTypeComponent(Owner_Type);

        return p->getType() != getType(); // need to be INPUT/OUTPUT compatibles 
    };
    
    
    bool Port::isCompatibleChildren(Port* p)  
    {
        TypeComponent p_Owner_Type = p->getOwner()->getTypeComponent();
        TypeComponent Owner_Type = getOwner()->getTypeComponent();

        p->getOwner()->setCacheTypeComponent(p_Owner_Type);
        getOwner()->setCacheTypeComponent(Owner_Type);			
			
        if (p_Owner_Type == UNKNOWN || Owner_Type == UNKNOWN)
            return true;
			
        return p->getType() == getType(); // need to be OUTPUT/OUTPUT or INPUT/INPUT  compatibles 
    };


    void Port::addLink(Port* p)
    {
        if (p==NULL)
            throw   NullPointerException("link target port","",__FUNCTION_NAME__);

        if ( getOwner() == NULL || p->getOwner() == NULL)
            throw CustomLinkException("At least one  port ("+getId()+" or "+ p->getId()+") does not belong to any component",__FUNCTION_NAME__);
        
        if ( getOwner() == p->getOwner())
            {
                // Intend to link two ports form the same component: forbidden
                throw CustomLinkException("Port p1=("+getOwner()->getFullId()+ ", " +getId()+") cannot link to port p2=("+p->getOwner()->getFullId()+ ", " + p->getId()+"): ports from same component",
                                          __FUNCTION_NAME__);
            }

        if (isSibling(p))
            {
                // Direct siblings 
                if (!isCompatibleSibling(p))
                    throw CustomLinkException("Port p1=("+getOwner()->getFullId()+ ", " +getId()
                                              +") cannot link to sibling port p2=("+p->getOwner()->getFullId()
                                              + ", " + p->getId()+"): are both INPUT or both OUTPUT instead of a  complementary INPUT/OUTPUT pair",
                                              __FUNCTION_NAME__);
                siblingLinks.push_back(p);
                siblingLinks.unique();
            }
        else if( p->getOwner()->getParent() && *(p->getOwner()->getParent())	== *(getOwner()) )
            {
                
                // Current component is the parent of target component

                // test  if ports can  be connected 
                if (!isCompatibleChildren(p))
                    throw CustomLinkException("Port p1=("+getOwner()->getFullId()+ ", " +getId()
                                              +") cannot link to child port p2=("+p->getOwner()->getFullId()
                                              + ", " + p->getId()+"): are INPUT/OUTPUT instead of both INPUT or both OUTPUT",
                                              __FUNCTION_NAME__);
                childrenLinks.push_back(p);
                childrenLinks.unique();
            }
        else if (getOwner()->getParent() && *(p->getOwner()) == *(getOwner()->getParent()))
            {

                // Current component is the child of the target component
                
                // test  if ports can  be connected 
                if (!isCompatibleChildren(p))
                    throw CustomLinkException("Port p1=("+getOwner()->getFullId()+ ", " +getId()
                                              +") cannot link to parent  port p2=("+p->getOwner()->getFullId()
                                              + ", " + p->getId()+"): are INPUT/OUTPUT instead of both INPUT or both OUTPUT",
                                              __FUNCTION_NAME__);

                parentLinks.push_back(p);
                parentLinks.unique();
            }
        else
            {
                // Something wrong
                if ( getOwner()->getParent() == NULL  ||  p->getOwner()->getParent() == NULL)
                    throw CustomLinkException("Port p1=("+getOwner()->getFullId()+ ", " +getId()+") cannot link to  port p2=("
                                              +p->getOwner()->getFullId()+ ", " + p->getId()
                                              +"): At least one of them does not have a parent component (they both should have one)",__FUNCTION_NAME__);
                else
                    throw CustomLinkException(
                                              "Ports p1=(" + getOwner()->getId() + ", " + getId()
                                              + ") cannot link to port p2=(" + p->getOwner()->getId()
                                              + ", " + p->getId()
                                              + "): it is only possible to link parent/child (one parent of the other) or direct siblings (same direct parent).",
                                              __FUNCTION_NAME__);
            }
    };

    void Port::delLink(Port* p)
    {
        if ( isSibling(p) )
            {
                siblingLinks.remove(p);
            }
        else
            {
                if (getOwner()->getParent() && *(p->getOwner()) == *(getOwner()->getParent()))
                    parentLinks.remove(p);
                else
                    childrenLinks.remove(p);
            }
    };

    Port::YoungestDescendantsIterator Port::getYoungestDescendantsBegin() 
    {
        YoungestDescendantsIterator it(this);
        Port* previousChildren = this;
        while (!previousChildren->childrenLinks.empty())
            {
                ChildrenLinksIterator itChild = previousChildren->childrenLinks.begin();
                it.iterators.push(itChild);
                previousChildren = *itChild;
            }
        return it;
    };

 
	Port::YoungestDescendantsIterator Port::getYoungestDescendantsEnd() 
	{
		YoungestDescendantsIterator it(this);
                it.end = true;
		return it;
	};
 

 
	size_t Port::getYoungestDescendantsSize() const
	{
		if (childrenLinks.empty())
		{
				return 1;
		}
		size_t result = 0;
		for(ConstChildrenLinksIterator it = childrenLinks.begin(); it != childrenLinks.end(); ++it)
		{
			result += (*it)->getYoungestDescendantsSize();
		};
		return result;
	};
 
 
	Port::YoungestDescendantsIterator& operator++ (Port::YoungestDescendantsIterator& it)
	{
		// big hack for primitive object... (different composite object with no children ..... )
		if (dynamic_cast<const Primitive* const>(it.host->getOwner()) && it.iterators.empty())
		{
           it.end = true;
			return it;
		}
        if(it.host->getChildrenLinksSize() == 0)
        {
            it.end = true;
            return it;  
        }
		Port::ChildrenLinksIterator itChildren;
		do 
		{
			itChildren = it.iterators.top();
			it.iterators.pop();
			itChildren++;
		} while ( (!(it.iterators.empty())) && itChildren == (*(it.iterators.top()))->getChildrenLinksEnd());
 


        if (itChildren == it.host->getChildrenLinksEnd())
            {
                it = it.host->getYoungestDescendantsEnd();
                return it;
            }

        Port::ChildrenLinksIterator itChild = itChildren;
        Port* previousChildren = NULL;
        do
            {
                it.iterators.push(itChild);
                previousChildren = *itChild;
                itChild = previousChildren->getChildrenLinksBegin();
            }
        while (itChild != previousChildren->getChildrenLinksEnd());



        return it;

    };

 
    Port* Port::YoungestDescendantsIterator::operator*()
    {
        Port* result;
        if (iterators.empty())
            result = host;
        else
            result = *(iterators.top());
        return result;
    };
    

    void  Port::getPrimitiveSiblingList(std::list<const Port*> &primlist) const
    {
    	primlist.clear();
        getPrimitiveSiblingList(this,primlist,NULL,false); // Don't skip connections

        // For debugging
/*
        std::list<const Port*> *rlist= new  std::list<const Port*>();
        getPrimitiveSiblingList(this,primlist,rlist,false);

        std::string list("");

        if ( primlist.empty() ) 
            {
                Msg::error("Port p1=("+owner->getFullId()+","+getId()+"): no accessible primitive sibbling",__FUNCTION_NAME__);
            }
        else 
            {
                for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
                    {
                        list += "\t- (";
                        list += (*it)->getOwner()->getFullId();
                        list += ", " + (*it)->getId() + ")\n";
                    }
                Msg::error("Port p1=("+owner->getFullId()+","+getId()+") accessible primitive siblings: "+list,__FUNCTION_NAME__);
            }
        
        if (!rlist->empty())
            {
                list ="";
                for( std::list<const Port*>::const_iterator it = rlist->begin(); it != rlist->end(); ++it)
                    {
                        list += "\t- (";
                        list += (*it)->getOwner()->getFullId();
                        list += ", " + (*it)->getId() + ")\n";

                        std::string listp("\t\t this port having restricted access  to  the  following primitives :");
                        for( std::list<const Port*>::const_iterator itp = (*it)->restrictedPrimitiveSiblings.begin(); itp != (*it)->restrictedPrimitiveSiblings.end(); ++itp)
                            {
                                listp += " ((";
                                listp += (*itp)->getOwner()->getFullId();
                                listp += ", " + (*itp)->getId() + "))";
                            }
                        list += listp+"\n";
                    }
                Msg::error("Port p1=("+owner->getFullId()+","+getId()+"): ports on the path to sibling primitives imposing access restrictions: "+list,__FUNCTION_NAME__);
            }
*/

    }

    void  Port::getPrimitiveSiblingListSkipConnections(std::list<const Port*> &primlist) const
        {
        	primlist.clear();
        	getPrimitiveSiblingList(this,primlist,NULL,true);// Don't skip connections
        }


    void  Port::getPrimitiveSiblingList(std::list<const Port*> &primlist, std::list<const Port*>  &reslist, bool skipcon) const
      {
          primlist.clear();
          reslist.clear();
          getPrimitiveSiblingList(this,primlist,&reslist,skipcon);
      }


    void Port::getPrimitiveSiblingList(const Port * src, std::list<const Port*> &primlist, std::list<const Port*> * reslist, bool skipcon) const
    {

        // We first look for all  primitives (without taking account accessibility restrictions)
        // We want to make sure that the primitive contained in the list are still linked (indirectly)  to the current component.

        // case of parent ports
        for (ConstParentLinksIterator it = getParentLinksBegin(); it != getParentLinksEnd(); ++it)
            {
                (*it)->getPrimitiveSiblingList(src,primlist,reslist,skipcon);
            }
        
        // Case of sibling components
        for (ConstSiblingLinksIterator it = getSiblingLinksBegin(); it != getSiblingLinksEnd(); ++it)
            {
                (*it)->getPrimitiveDescendantList(src,primlist,reslist,skipcon);
            }

        // If the current component imposes accessibility restrictions, remove unaccessible primitives from primlist
        if ( !restrictedPrimitiveSiblings.empty() )
            {
                bool res = false;
                for( std::list<const Port*>::iterator it = primlist.begin(); it != primlist.end();)
                    {
                        if (restrictedPrimitiveSiblings.end() == std::find( restrictedPrimitiveSiblings.begin(),  restrictedPrimitiveSiblings.end(),*it) )
                            {
                                // not in restricted list -> remove.
                                it=primlist.erase(it);//it increased by one here.
                                res = true;
                            }
                        else
                            {
                                it++;
                            }
                    }
                // record that this component restricts the list of primitive siblings.
                if (reslist!=NULL && res == true) reslist->push_back(this); 
            }
    }



    void Port::getPrimitiveDescendantList(const Port * src, std::list<const Port*> &primlist, std::list<const Port*> * reslist, bool skipcon) const
     {   

        //The sibling list is restricted to a given set of primitives
        // Check if src is included in this list (only if src is a primitive)
        // if src excluded from this list, src cannot access  any of the primitive included in the current component  in its list of primitive siblings
         if ( 
             !src->getOwner()->isComposite() 
             && !restrictedPrimitiveSiblings.empty() 
             && restrictedPrimitiveSiblings.end() == std::find( restrictedPrimitiveSiblings.begin(),  restrictedPrimitiveSiblings.end(),src) )
            {
                // src is not in the restricted primitive list of this component
                if( reslist != NULL) reslist->push_back(this); // Store this port has it restricts our primitive sibling list
            }
         else 
             {
                // src is accessible  from the current component (or src is composite).  Dig further into the component to find the  primitives.
                if ( getOwner()->isComposite() )
                    {
                        if ( !getOwner()->isControllerApplied("execute") )
                            {
                                throw CustomException("Port p=("+getOwner()->getFullId()+","+getId()+") error: execute controller not yet applied. Component visited while  looking for primitive siblings of p=("+src->getOwner()->getFullId()+","+src->getId()+")",__FUNCTION_NAME__);
                            }

                        if ( isChildrenLinksEmpty() )
                            {
                                // if no  child and execute already applied: empty component. 
                                // Nothing to do.
                            }
                        else
                            {
                                // Recurse on children
                                for (ConstChildrenLinksIterator it = getChildrenLinksBegin(); it != getChildrenLinksEnd(); ++it)
                                    {
                                        (*it)->getPrimitiveDescendantList(src,primlist,reslist,skipcon);
                                    }
                            }
                    }
                else
                    {
                        // This  is a primitive. 

                		// This is a connection: skip it if requested
                        if (skipcon && dynamic_cast<ConnectionBase*>(getOwner()) )
                            	{
                            	// Go to the next neighbor
                            	Port* otherSide;
                            	if ( getId() == "in")
                            		{
                            			otherSide = owner->getPort("out");
                            		}
                            	else
                            		{
                            			otherSide = owner->getPort("in");
                            		}
                            	otherSide->getPrimitiveSiblingList(src,primlist,reslist,skipcon);
                            	}
                        else
                        {
                        // This is a primitive: store it in the primitive sibling list.
                         primlist.push_back(this);
                        }
                    }
            }
    }
                
    void Port::addRestrictionToPrimitiveSibling(const Port* p) const 
    {
        // check  not null
        if (!p) throw NullPointerException("port","", __FUNCTION_NAME__);

        // check primitive
        if ( p->getOwner()->isComposite() )
            {
                throw CustomException("Port p1=("+owner->getFullId()+","+getId()+") intend to add port p2=("+p->getOwner()->getFullId()+","+p->getId()+") to its restricted list of primitive components, but p2 is not primitive",__FUNCTION_NAME__);
            }

        // check  that p is an external primitive and accessible, computing the list of primitive siblings.

        // We save the actual primitive list and set it to empty (otherwise getPrimitiveSiblingList simply limits its primitive sibling search to the one in restrictedPrimitiveSiblings)
        std::list<const Port*> primlist;
        std::list<const Port*> reslist;
        std::list<const Port*> saveprimlist;


        saveprimlist.swap(restrictedPrimitiveSiblings);



        getPrimitiveSiblingList(primlist,reslist,false);//Don't skip connections


        if (   primlist.end() == std::find(primlist.begin(),primlist.end(),p)  ) 
            {
                // p not accessible from here 
              
                std::string list("");
                list +=" Ports with accessibility restrictions:";
                for( std::list<const Port*>::const_iterator it = reslist.begin(); it != reslist.end(); ++it)
                    {
                        list += "\n\t- (";
                        list += (*it)->getOwner()->getFullId();
                        list += ", " + (*it)->getId() + ")";

                        std::string listp(" with accessible primitives restricted to:");
                        for( std::list<const Port*>::const_iterator itp = (*it)->restrictedPrimitiveSiblings.begin(); itp != (*it)->restrictedPrimitiveSiblings.end(); ++itp)
                            {
                                listp += " ((";
                                listp += (*itp)->getOwner()->getFullId();
                                listp += ", " + (*itp)->getId() + "))";
                            }
                        list += listp+"\n";
                    }

                throw CustomException("Port p1=("+owner->getFullId()+","+getId()+") intend to add port p2=("+p->getOwner()->getFullId()+","+p->getId()+") to its restricted list of accessible primitive components, but p2 is not accessible because not connected or its access is restricted  by some of the following  ports:\n"+list,__FUNCTION_NAME__);
            }
        //restore list and add p
        restrictedPrimitiveSiblings.swap(saveprimlist);


        restrictedPrimitiveSiblings.push_back(p);
        p->reverseResPrimSib.push_back(this); // also include it in the reverse list of p
        
    };
                

    void Port::delRestrictionToPrimitiveSibling(const Port* p)const
    {
        restrictedPrimitiveSiblings.remove(p); 
        // Remove me from the list of ports having p in their @see restrictedPrimitiveSiblings list
        p->reverseResPrimSib.remove(this); 
    };







   void Port::cloneLinks() 
   {
       // clone links to ports of parent component
       clone->parentLinks.clear();
       for(Port::ConstParentLinksIterator it = getParentLinksBegin(); it != getParentLinksEnd(); ++it)
           {
               if ( (*it)->clone == NULL )
                   {
                   // no clone. We point to the original (outside of the component being cloned)
                   clone->parentLinks.push_back(*it);
                   }
               else
                   {
                       // we point to the clone
                   clone->parentLinks.push_back((*it)->clone);
                   }
           }
       
       
       // Clone sibling links
       clone->siblingLinks.clear();
       for(Port::ConstSiblingLinksIterator it = getSiblingLinksBegin(); it != getSiblingLinksEnd(); ++it)
           {
               if ( (*it)->clone == NULL)
                   {
                   clone->siblingLinks.push_back(*it);
                   }
               else
                   {
                   clone->siblingLinks.push_back((*it)->clone);
                   }
           }
       

       
       // Clone the restricted list of accessible primitives (and keep the corresponding reverseResPrimSib list up to date
       clone->restrictedPrimitiveSiblings.clear();
       for( std::list<const Port*>::iterator it = restrictedPrimitiveSiblings.begin(); it != restrictedPrimitiveSiblings.end(); ++it)
           {
               if ( (*it)->clone  == NULL)
                   {
                       // no clone. We point to the original (outside of the component being cloned)
                       clone->restrictedPrimitiveSiblings.push_back(*it);
                       (*it)->reverseResPrimSib.push_back(clone); 
                   }
               else
                   {
                       // we address the clone 
                       clone->restrictedPrimitiveSiblings.push_back((*it)->clone);
                       (*it)->clone->reverseResPrimSib.push_back(clone); 
                   }
               
           }
       
       // We also have this port added to the list of  restricted acccessible primitives
       // of target having the  current port in their restrictedPrimitiveSiblings list.
       clone->reverseResPrimSib.clear();
       for( std::list<const Port*>::iterator it = reverseResPrimSib.begin(); it != reverseResPrimSib.end(); ++it)
           {
               if ( (*it)->clone  == NULL)
                   {   
                       (*it)->restrictedPrimitiveSiblings.push_back(clone);
                       clone->reverseResPrimSib.push_back(*it); 
                   }
               else
                   {
                       (*it)->clone->restrictedPrimitiveSiblings.push_back(clone);
                       clone->reverseResPrimSib.push_back((*it)->clone); 
                   }
           }
   }
    

    Port* Port::clonePort()
    {
        Port * pclone = new Port(id,owner->getClone(),type,msgType,sameas,m_nBlocking);
        // don't forget  pointers between clones
        pclone->clone = this;
        this ->clone = pclone;

        return pclone;
    }

   void link(Port*  src_, Port*  dest_) throw(CustomException,NullPointerException)
    {
        if (!src_ || !dest_) throw NullPointerException("port", "", __FUNCTION_NAME__);
        src_->addLink(dest_);	
        dest_->addLink(src_);
    }

    void unlink(Port*  src_, Port*  dest_) throw(CustomException,NullPointerException)
    {
        if (!src_ || !dest_) throw NullPointerException("port", "", __FUNCTION_NAME__);
        src_->delLink(dest_);	
        dest_->delLink(src_);
    }



}; }; // end namespace
