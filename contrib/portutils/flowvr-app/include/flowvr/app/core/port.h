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
 * File: ./include/app/core/port.h                                 *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/


/**
 * \file port.h
 * In this file, port concept is defined. Class implemented is
 * <ul>
 *    <li> \a Port :  ports for \a Component 
 * </ul>
 */

#ifndef __FLOWVR_APP_FLOWVR_PORT_H
#define __FLOWVR_APP_FLOWVR_PORT_H

#include <cstddef>
#include <iterator>
#include <list>
#include <stack>
#include <string>
#include <flowvr/xml.h>
#include "flowvr/app/core/exception.h"
#include "flowvr/app/core/msg.h"



namespace flowvr { namespace app {

    
    class Component; ///<  Forward declaration for cyclic dependency. Class defined into @see component.h. 

    /**
     * \enum PortType
     * \brief This type is used to declare if a  port is an input or an output port.
     */
    enum PortType {INPUT=0, OUTPUT=1};
    

    /**
     * \enum PortMessageType
     * Used to declare if a  port expect to receive/send
     * full messages (stamps+payload),
     * stamps messages (stamps only),
     * auto (full or stamps state unkown at the time of the port declaration, but computed latter according to context)
     * sameas (full or stamps state identical to one of the auto port of the component. Defined latter once auto ports resolved)
     */
    enum PortMessageType {STAMPS=0, FULL=1,AUTO=2,SAMEAS=3};

    /**
     * \class Port
     * \brief a port sends or receives dataflow. 
     */
    class Port
    {
        public:

        enum eBlckState
        {
        	ST_UNKNOWN = 0,
        	ST_BLOCKING,
        	ST_NONBLOCKING
        };


        private :
            std::string id; ///< id of the port	
            PortType type; ///< Type Input/Output
            PortMessageType msgType; ///< Type Full/Stamps/Auto/Sameas
            std::string sameas; ///< port should take the message type  as this sameas port
            Component* owner; ///< Component owning the port 
            std::list<Port*> siblingLinks; ///<Sibling links
            std::list<Port*> childrenLinks; ///< Children links
            std::list<Port*> parentLinks; ///< parent links (a port can be linked to several ports of the parent component)
            eBlckState m_nBlocking; ///< reflect blocking state (when known)
            Port* clone; ///< Pointer to clone. Used only when cloning a component. Should be symmetric, i.e clone == clone->clone. Should be set to NULL when clone deleted. 	

            /**
             * \brief  Restricted list of accessible   primitive siblings  connected to the current component
             *
             * Store pointers on a restricted set of ports of primitive sibling components (outside from the current component) that are linked to the current port. 
             * Even if other primitive ports are connected to the current one, they won't be accessible through @see getPrimitiveSiblingList() if they are not included in this list.
             *
              * If this list is empty, no restricion is applied and all connected primitive components are acccessible (except if on the path to this primitive, other
             * components hide some of them from the current source with their own restricted primitive list).
             *
              * This list can be used for composite as well as primitive components. If applied to a composite it restricts the list of connected sibling
             *  primitives (i.e. primitives ouside of the current component) for all primitive that may be included in the current component.
             *
             * This list can contain ports that are not linked (directly or indirectly) to  the current port (linkage checked at port insertion, but can be broken when removing links)
             *
             * This is usefull to force one primitive component to exclusively connect to a specific target primitive component whithout having to  directly link them 
             * (which is often impaired by the hierarchy of components). For instance this is used in the @see Tree class. 
             *
             * Make it mutable so that a deleted port can remove it presence in the restrictedPrimitiveSiblings of distant components. 
             * 
             */
            mutable std::list<const Port*> restrictedPrimitiveSiblings;

        public: std::list<const Port*> getRestrictedPrimitiveSiblings(){
            	return restrictedPrimitiveSiblings;
            }

            /**
             * \brief  Reversed restricted primitive sibling list: Store the ports that points to the current port in their restrictedPrimitiveSiblings list. 
             *
             * This data structure enables to maintain  correct  restrictedPrimitiveSiblings for other ports (avoid pointers to free memory addresses)
             *  when a port is deleted 
             * 
             * Make it mutable as this is one of the very few data structure that can be modified by a distant component.
             * 
             */
            mutable std::list<const Port*> reverseResPrimSib;

        public: std::list<const Port*> getReverseResPrimSib(){
        	return reverseResPrimSib;
        }

            /**
             * private default constructor.
             */
            Port() {}

        public :


            /**
             * \brief constructor
             * \param id_ id of the port
             * \param host_ pointer to the component where the port is
             * \param type_ the type of the port (INPUT or OUTPUT)
             * \param msg_  the message type the port carry (FULL, STAMPS OR AUTO)
             * \param sameas_  the name of the other port of the component the message type is equal to. 
             */
            Port(const std::string& id_,
                 Component* host_,
                 const PortType& type_,
                 PortMessageType  msg_=FULL,
                 const std::string& sameas_ = "",
                 eBlckState blocking = ST_UNKNOWN)
            : id(id_)
            , owner(host_)
            , type(type_)
            , msgType(msg_)
            , sameas(sameas_)
            , m_nBlocking(blocking)
            , siblingLinks()
            , childrenLinks()
            , parentLinks()
            , restrictedPrimitiveSiblings()
            , clone(NULL)
            {}



            /**
             * \brief Destructor
             */
            ~Port();
       
            /**
             * \brief get id
             * \return id
             */
            std::string getId() const {return id; };

            /**
             * \brief get type (input or output)
             * \return type
             */
            PortType getType() const { return type; };

            /**
             * \brief return the blocking (or non blocking) state of the port 
             */
            bool getIsBlocking() const { return (m_nBlocking == ST_BLOCKING or m_nBlocking == ST_UNKNOWN); }

            eBlckState getBlockingState() const { return m_nBlocking; }

            /**
             * \brief return the blocking (or non blocking) state of the port 
             * @return a string (to be used for user messages and xml files mainly)
             */
            std::string getIsBlockingString() const;

            typedef std::list<Port*>::iterator SiblingLinksIterator; ///< Sibling link iterator (a link is a pointer to a port)


            /**
             * \brief get the first sibling link
             * \return the first sibling link
             */

            SiblingLinksIterator getSiblingLinksBegin() { return siblingLinks.begin(); };

            /**
             * \brief get the last sibling link
             * \return the last sibling link
             */
            SiblingLinksIterator getSiblingLinksEnd() { return siblingLinks.end(); };


            typedef std::list<Port*>::const_iterator ConstSiblingLinksIterator; ///< Sibling link const_iterator (a link is a pointer to a port)
            /**
             * \brief get the first sibling link
             * \return the first sibling link
             */
            ConstSiblingLinksIterator getSiblingLinksBegin() const { return siblingLinks.begin(); };
            /**
             * \brief get the last sibling link
             * \return the last sibling link
             */
            ConstSiblingLinksIterator getSiblingLinksEnd() const { return siblingLinks.end(); };

            /**
             * \brief sibling links set is empty?
             * \return true if empty
             */
            bool isSiblingLinksEmpty() const { return siblingLinks.empty(); };
            /**
             * \return the number of sibling link
             */
            size_t getSiblingLinksSize() const { return siblingLinks.size(); };
	
            typedef std::list<Port*>::iterator ChildrenLinksIterator; ///< iterator on  children linked to current compoent  (a link is a pointer to a port)
            /**
             * \brief get the first children link
             * \return the first children link
             */
            ChildrenLinksIterator getChildrenLinksBegin() { return childrenLinks.begin(); };
            /**
             * \brief get the last children link
             * \return the last children link
             */
            ChildrenLinksIterator getChildrenLinksEnd() { return childrenLinks.end(); };

            typedef std::list<Port*>::const_iterator ConstChildrenLinksIterator; ///< const_iterator on  children linked to current compoent  (a link is a pointer to a port)
            /**
             * \brief get the first children link
             * \return the first children link
             */
            ConstChildrenLinksIterator getChildrenLinksBegin() const { return childrenLinks.begin(); };
            /**
             * \brief get the last children link
             * \return the last children link
             */
            ConstChildrenLinksIterator getChildrenLinksEnd() const { return childrenLinks.end(); };

            /**
             * \brief children links set is empty?
             * \return true if empty
             */
            bool isChildrenLinksEmpty() const { return childrenLinks.empty(); };
            /**
             * \return the number of children links
             */
            size_t getChildrenLinksSize() const { return childrenLinks.size(); };


            typedef std::list<Port*>::iterator ParentLinksIterator; ///< iterator on  parent ports  linked to current component  (a link is a pointer to a port)
            /**
             * \brief get the first parent link
             * \return the first parent  link
             */
            ParentLinksIterator getParentLinksBegin() { return parentLinks.begin(); };
            /**
             * \brief get the last parent link
             * \return the last parent link
             */
            ParentLinksIterator getParentLinksEnd() { return parentLinks.end(); };

            typedef std::list<Port*>::const_iterator ConstParentLinksIterator; ///< const_iterator on  parent ports  linked to current compoent  (a link is a pointer to a port)
            /**
             * \brief get the first parent link
             * \return the first parent  link
             */
            ConstParentLinksIterator getParentLinksBegin() const { return parentLinks.begin(); };
            /**
             * \brief get the last parent  link
             * \return the last parent  link
             */
            ConstParentLinksIterator getParentLinksEnd() const { return parentLinks.end(); };

            /**
             * \brief parent links set is empty?
             * \return true if empty
             */
            bool isParentLinksEmpty() const { return parentLinks.empty(); };
            /**
             * \return the number of parent links
             */
            size_t getParentLinksSize() const { return parentLinks.size(); };

            /**
             * \brief get the component where the port is
             * \return pointer to the component
             */
            Component* getOwner() const { return owner; };

            /**
             * \brief get the type of message a  port carry (full, stamps, auto or sameas)
             */
            PortMessageType getMsgType() const { return msgType; };

            /**
             * \brief get the type of message a  port carry (full, stamps, auto or sameas)
             * @return a string (to be used for user messages and xml files mainly)
             */
            std::string getMsgTypeString() const;


            /**
             * \brief get the name of the port the message type should be copied from (in case of a SAMEAS port)
             *
             * @warning throw an exception if current port is not of SAMEAS type. 
             *
             * @return the name of the target SAMEAS port
             */
            std::string getSameAs() const;

            /**
             * \brief set the type of message a  port carry (full, stamps,  auto or sameas)
             * \param m the type of the message (FULL, STAMPS, AUTO, SAMEAS)
             * \param s optional argument that should only be used with the SAMEAS type. In the case gives the the id of the port the message type should be copied from
             */
            void setMsgType(const  PortMessageType m, const std::string& s ="");

            /**
             * \brief add link method
             * \param p pointer to the port
             */
            void addLink(Port* p);

              /**
             * \brief remove link 
             * \param p : designate the link between this and p
             */
            void delLink(Port* p);

            /**
             * \brief flush all children links
             * \warning should be used only in ~Composite
             */
            void flushChildrenLinks() { childrenLinks.clear();}
	
            /**
             * \param p the other port for the test
             * \return if two ports are direct  siblings (same parent component)
             */
            bool isSibling(Port* p) const; 
            /**
             * \param p the other port for the test
             * \return the type of port (INPUT/OUTPUT OBJECT/CONNECTION) are compatible for a sibling link
             * \note if one component is UNKNOWN then method return true
             */
            bool isCompatibleSibling(Port* p);
            /**
             * \param p the other port for the test
             * \return the type of port (INPUT/OUTPUT OBJECT/CONNECTION) are compatible for a children link 
             * \note if one component is UNKNOWN then method return true
             */
            bool isCompatibleChildren(Port* p);

            /**
             * \return XML description of the port
             */
            xml::DOMElement* XMLBuild();

            /**
             * \class YoungestDescendantsIterator
             * \brief iterator for the leafs of the children tree.
             */
            class YoungestDescendantsIterator 
            {
                public :
                    //iterator traits
                    typedef std::forward_iterator_tag iterator_category ;
                    typedef Port* value_type;
                    typedef ptrdiff_t difference_type;
                    typedef Port** pointer;
                    typedef Port*& reference;
                private :
                    std::stack<ChildrenLinksIterator> iterators; ///< Stack for the iteration on the port tree
                    Port* host; ///< The port root
                bool end; ///< is the end of the list ?


				/**
				 * \brief constructor
				 * \param host_ the port where the iterator is created
				 */
				YoungestDescendantsIterator(Port* host_) : host(host_), end(false) {};


                    friend class Port; ///< Only port methods can use this iterator
 
				/**
				 * Classic methods for an iterator
				 */
				friend YoungestDescendantsIterator& operator++ (YoungestDescendantsIterator& it);
				friend bool operator== (const YoungestDescendantsIterator& it1, const YoungestDescendantsIterator& it2) {return (it1.host == it2.host && it1.end == it2.end && ((it1.iterators.empty() && it2.iterators.empty()) || ( (!it1.iterators.empty()) && (!it2.iterators.empty()) && it1.iterators.top() == it2.iterators.top())));};
				friend bool operator!= (const YoungestDescendantsIterator& it1, const YoungestDescendantsIterator& it2) {return !(it1 == it2);};
 

                public :
                    Port* operator*(); 

            };
            /**
             * \brief get the first leaf
             */
            YoungestDescendantsIterator getYoungestDescendantsBegin();
            /**
             * \brief get the end of the tree
             */
            YoungestDescendantsIterator getYoungestDescendantsEnd();

            /**
             * \brief get the size of grandChildren
             * \return the size
             */
            size_t getYoungestDescendantsSize() const;

                
            /** @brief Copy parent and sibling links
             *
             * This method also takes care of maintaining consistent restricted accessible primitive lists. 
             * - copy the restrictedPrimitiveSiblings of p and update the   reverseResPrimSib list of all target ports
             * - add the current the current port  port to each  restrictedPrimitiveSiblings  of the target ports of  reverseResPrimSib of p.
             * - copy the reverseResPrimSib of p
             *
             * @param p the port to copy links forms 

             * @deprecated This method was used by the cloning method reexecuting traverses (very slow)
             *
             */
            //            void copyLinks(Port* p);


            /** @brief Clone links of current port
             *
             *
             *  @warning this  must be be called from  the original  port and not its  clone (modify the clone).
             *
             * Use the clone pointers between clone/original ports  to access original and cloned ports.
             * Assume that if the clone pointer is set to NULL, the pointer must be set to an original port 
             *
             * This method also takes care of maintaining consistent restricted accessible primitive lists. 
             * - copy the restrictedPrimitiveSiblings of p and update the   reverseResPrimSib list of all target ports
             * - add the current the current port  port to each  restrictedPrimitiveSiblings  of the target ports of  reverseResPrimSib of p.
             * - copy the reverseResPrimSib of p
             *
             *
             */
            void cloneLinks() ;

            
          /** @brief Clone current port (but not the links)
           *
           *
           *  @warning this method must be   called from  the original port and not its  clone (modify the clone).

           * @return a pointer to the cloned port
           */
            Port* clonePort() ;


          /** @brief  Return pointer to port clone
           */
            Port* getClone() const{ return clone;};


            /** @brief Return the list of primitive components accessible from the current component and optionally the list of components that imposed restrictions on this list
             *
             *  If called from a primitive component, it returns the list of accessible sibling primitives taking into account
             *  - the  @see restrictedPrimitiveSiblings  list of the current components and of its parents.
             *  - the  @see restrictedPrimitiveSiblings  list  of siblings components (i.e. all external components): if such sibling component p has a non empty  @see restrictedPrimitiveSiblings  list that does not contain the  current primitive, then all primitive contained in p are excluded from the returned list.
             *
             * If called from a composite components, it only takes into account:
             * - the  @see restrictedPrimitiveSiblings  list of the current components and of its parents.
             *
             * If @param skipcon set to true we skip connections encountered along the way and  we look at primitives connected on the other side of this connection
             *
             *  The algorithm ensures all these primitives are connected (directly or indirectly) to the current port.
             *
             * @param primlist Return the list of primitive components accessible  from the current component (list is cleared before being filled). T
             * @param reslit  Return the  list of components that restricted the list of target primitives through their own @see restrictedPrimitiveSiblings list. Usefull when  debugging. If the pointer given is null (default value) this list is not computed (list is cleared before being filled).
             * @param skipcon  If set to true skip connections encountered along the way.
             *
             */
            void  getPrimitiveSiblingList(std::list<const Port*> &primlist, std::list<const Port*>  &reslist, bool skipcon) const;


            /** @brief Return the list of primitive components accessible from the current component and optionally the list of components that imposed restrictions on this list.
             *
             *  Warp of previously defined getPrimitiveSiblingList() method for commodity
             *
             * @param  primlist Return the list of primitive components accessible  from the current component (list is cleared before being filled).
             */
            void  getPrimitiveSiblingList(std::list<const Port*> &primlist) const;


            /** @brief Same as  getPrimitiveSiblingList() except that it skips connections encountered along the way and looks at primitives connected on the other side of these connections
               *
               *  Warp of previously defined getPrimitiveSiblingList()  method for commodity
               *  To use with care. Usually used in setHosts() methods to identify the hosts of connected primitives (connections are useless as not ammped on hosts)
               *
               * @param  primlist Return the list of primitive components accessible  from the current component  (connection skipped)
               */
            void  getPrimitiveSiblingListSkipConnections(std::list<const Port*> &primlist) const;


            /**
             * \brief add a  port to the restricted list  of accessible  primitive sibling ports.
             *
             * @pre the port to add must be
             * - connected to the current port
             * - accessible from the current port (no restriction applied by some @see restrictedPrimitiveSiblings list).
             * - direct or indirect sibling (external to the current component)
             * - primitive
             *
             * @param p pointer to the target port
             */
            void addRestrictionToPrimitiveSibling(const Port* p) const;
            
            
            /**   
             * \brief remove port from the restricted list of accessible primitive siblings.
             *
             * Also clean the reverse list (@see  reverseResPrimSib) of p.
             *
             * \param p pointer to the primitive port to remove
             */
            void delRestrictionToPrimitiveSibling(const Port* p) const;

        private:

            /**
             * \brief Accumulate the  primitive direct and indirect siblings  accessible from @ src 
             *
             *  Recusrsive  method called from @see getPrimitiveSiblingList
             *
             *  Take into account the @see restrictedPrimitiveSiblings of  traversed components
             *
             *  The algorithm ensures all returned primitives are connected (directly or indirectly) to the current port.
             *
             * @param src Look for the primitive components accessible from  this source port. Should be a descendant component  of the current component (condition not enforced). Can be  port form a primitive or composite. 
             * @param  primlist Accumulate in this list the  primitive components accessible  from the current component. 
             * @param reslit  Accumulate in  this  list  the  components that restricted the list of target primitives through their own @see restrictedPrimitiveSiblings list. If the pointer given is null (default value) this list is not computed.
             * @param skipcon  If set to true, skip connections encountered along the way.
             * 
             */
            void getPrimitiveSiblingList(const Port * src, std::list<const Port*> &primlist, std::list<const Port*> * reslist, bool skipcon) const;


            /**
             * \brief Accumulate  the  primitive components  accessible from @ src  that are contained in the current component
             *
             *  Recusrsive  method called from @see getPrimitiveSiblingList
             *
             *  Take into account the @see restrictedPrimitiveSiblings of  traversed components
             *
             *  The algorithm ensures all retunred primitives are connected (directly or indirectly) to the current port.
             *
             * @param src Look for the primitive components accessible from  this source port.  Can be  port from a primitive or composite. 
             * @param  primlist Accumulate in this list the  primitive components contained and accessible from the current component. 
             * @param reslit  Accumulate in this  list  the  components that restricted the list of target primitives through their own @see restrictedPrimitiveSiblings list. If the pointer given is null (default value) this list is not computed.
             * @param skipcon  If set to true, skip connections encountered along the way.
             *
             *
             */
            void getPrimitiveDescendantList(const Port * src, std::list<const Port*> &primlist, std::list<const Port*> * reslist, bool skipcon) const;

                
  

    };

    /**
     * \brief Links two ports.  
     * \param src_  one link extremity
     * \param dest_  the other link extremity
     * \throw CustomException if  intend to build an incorrect link
     * \throw NullPointerException if \a src_ or  \a dest_  Null
     */
    void link(Port*  src_, Port*  dest_) throw(CustomException,NullPointerException);

    /**
     * \brief Unlinks two ports. 
     * \param src_  one link extremity
     * \param dest_  the other link extremity
     * \throw NullPointerException if \a src_ or  \a dest_  Null
     */
    void unlink(Port*  src_, Port*  dest_) throw(CustomException, NullPointerException);

};}; //end namespace

#endif // __FLOWVR_PORT_H
