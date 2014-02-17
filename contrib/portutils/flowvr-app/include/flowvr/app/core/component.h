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
 *       File : flowvr/app/core/component.h                        *
 *                                                                 *
 *                                                                 *
 *       Contact : Jean-Denis Lesage                               *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP_COMPONENT__
#define __FLOWVR_APP_COMPONENT__

#include <string>
#include <list>
#include <map>
#include <flowvr/xml.h>
#include "flowvr/app/core/controllerlog.h"
#include "flowvr/app/core/port.h"
#include "flowvr/app/core/interfacecontroller.h"
#include "flowvr/app/core/data.h"
#include "flowvr/app/core/parameter.h"
#include "flowvr/app/core/parameterlist.h"
#include "flowvr/app/core/host.h"


/**
 * \file component.h
 * \brief implements base classes for components, primitives and composites
 */



namespace flowvr { namespace app {


    class Composite; ///<  Forward declaration for cyclic dependency. Class Composite used in Component and defined after



    /**
     * \brief A component has a type object or communication. If cannot be determined then UNKNOWN. See documentation for connecting rules
     */
    enum TypeComponent {OBJECT,COMMUNICATION,UNKNOWN};




	/**
	 * \brief  List of hosts for each component
	 */
	typedef std::vector<Host> HostList;

	/**
	 * \brief  Map for storing host data extracted from CSV file
	 */
	typedef  std::map<std::string, HostList > HostMap;


    /**
     * \class Component
     * \brief the interface for all components
     */
    class Component
    {
        private:
            std::string id; ///< id of the object
            std::string type; ///< type of the object
            static const char idsep='/'; ///< id separator
            ControllerLog controllerLog; ///<The log of all controllers
            TypeComponent _cache_type; ///< cache for type component to solve performance issue

        protected:
            std::list<Port*> ports; ///< list of ports
            Composite* parent; ///< father of the object (necessarily a composite component)
            Component* clone; ///< Pointer to clone. Used only when cloning a component. Should be symmetric, i.e clone == clone->clone. Should be set to NULL when clone deleted. 


            /// INFORMATIONS FOR HOST MAPPING
            const Port* relativeMappingPort; ///< Map thanks to this port

        public:
            xml::DOMElement* adlDescr; ///< Pointer in adlFile to the description of this object


            // Component mapping to hosts
            static HostMap * csvHostMap; ///< pointer to the maps extracted from the CSV file
            HostList hostList; ///< vector of hosts

            // Parameter lists
            ParameterList* paramListSetExternal; ///<  pointer to the list  of parameters set from command line, parameter file or adl.
            mutable ParameterList paramListAdd; ///< List of parameters added to this component (i.e. addParameter called for this component). Modified to store the value of the parameter each time  it is computed.
            ParameterList paramListSet; ///< List of parameters set from this component (i.e. setParameter called from this component)


            // Traces
            std::list<Trace> myTraceList;


            /**
             * \brief constructor
             * \param id_ id of the component
             * \param type_ information about the component type (module, filter, etc ...)
             */
            Component(const std::string& id_, const std::string& type_) : id(id_),
                                                                          type(type_),
                                                                          ports(),
                                                                          parent(NULL),
                                                                          relativeMappingPort(NULL),
                                                                          adlDescr(NULL),
                                                                          paramListSetExternal(NULL),
                                                                          paramListAdd(),
                                                                          paramListSet(),
                                                                          _cache_type(UNKNOWN),
                                                                          clone(NULL),
                                                                          //csvHostMap(NULL),
                                                                          hostList()
            {
                addParameter("info",""); // default value: empty string

                addParameter<int>("trace",0); // Enable to activate a trace capture for that component
                                              // and all its descendant (if trace is different from 0
											  // then trace this component)

            }

            /**
             * \brief destructor
             */
            virtual ~Component();


            /**
             * \brief get the type string
             * \return the type string
             */
            std::string getType() const { return type; }

            /**
             * \brief set the type string (use with great care !)
             * \return 
             */
            void  setType(const std::string type_)  { type = type_; }

            /**
             * \brief get the component type
             * \note this method is pure virtual
             */
            virtual TypeComponent getTypeComponent() const = 0;


            /**
             * \brief this method tells if a component is composite ( avoid a dynamic_cast ...)
             */
            virtual bool isComposite() const = 0;

            /**
             * \brief this method tells if a component is primitive ( avoid a dynamic_cast ...)
             */
            virtual bool isPrimitive()const = 0;

            /**
             * \return id string
             */
            std::string getId() const { return id; }

            /**
             * \brief Change the  component id.
             */
            void setId(const std::string &strId) { id = strId; }

            /**
             * \brief get the id separator
             * \return char
             */
            static char  getIdSep()   { return idsep; }


            /**
             * \brief get  the full id (concatenation of all ids from  oldest ancestor to current component)
             * \return fullid
             */
            std::string getFullId() const;

            typedef std::list<Port*>::iterator pPortIterator;        ///< definition of a port iterator
            typedef std::list<Port*>::const_iterator ConstpPortIterator; ///< definition of a port const_iterator

            /**
             * \brief get the first port
             * \return iterator to the first port
             */
            ConstpPortIterator getPortBegin() const{ return ports.begin(); }

            /**
             * \brief get the last port
             * \return iterator to the last port
             */
            ConstpPortIterator getPortEnd() const{ return ports.end(); }

            /**
             * \brief get the first port
             * \return iterator to the first port
             */
            pPortIterator  getPortBegin() { return ports.begin(); }


            /**
             * \brief get the last port
             * \return iterator to the last port
             */
            pPortIterator  getPortEnd() { return ports.end(); }

            /**
             * \brief get the first port by the given id
             *
             * Retrieves a port by name/id. Note that in case you registered in id two or more times, this method will only
             * give you the first found.
             * In case the port is not found, this method will throw an exception.
             *
             * @todo searching a port which is not there is not an exception, no? Could return NULL instead?
             *
             * \param id the id of the port
             * \return pointer to the port
             *
             * @see hasPort()
             */
            Port *  getPort(const std::string& id) const;

            /**
             * transience API, to be removed
             */

            bool isPortExist( const std::string &id ) const { return hasPort(id); }

            /**
             * \brief Check the existence of a port with a given id
             *
             * This method checks if a port with a given id was added to this component.
             * @return true if a port with the give id exists. In that case, getPort(id) returns a non-NULL port.
             *         false else.
             */
            bool hasPort(const std::string& id) const;


            bool getIsBoundByInports() const;

            /**
             * \brief return the number of ports
             */
            size_t getPortSize() const { return ports.size(); }


            /**
             * \brief get the parent of the component
             * \return pointer to the parent
             */
            const Composite* getParent() const {return parent;}
            Composite *getParent() { return parent; }


            /**
             * \brief change parent
             * \param parent_ pointer to the parent
             */
            void setParent(Composite* parent_) {parent = parent_;}

            /**
             * \brief equality operator
             */
            friend bool operator==( const Component& c1, const Component& c2)
            {
                return (c1.id == c2.id
                	&& ( ( c1.parent && c2.parent && c1.parent == c2.parent )
                	|| ((!c1.parent) && (!c2.parent) ) ) );

            }

            /**
             * \brief difference operator
             */
            friend bool operator!=( const Component& c1, const Component& c2) { return !(c1 == c2); }

            /**
             * \brief Create a new instance of an object of this type.
             *
             * This is a key method that is required to clone components during  fixed point traverses
             * Each component inheriting from this class  must implement its own version of this method.

             * We need this method because constructors  cannot be virtual. 
             * It enables to create a component of a derived type even if this method is called  from 
             * a <i>Component</i>. For instance p is of type <i>Component *</i> but point to a <i>Module</i>  component, 
             * calling create from p will return a new instance of <i>Module</i> (with same id).
             * 
             */
            virtual Component*  create() const = 0;

            /**
             * \brief this method is call from traverse method to create a "partial clone" of the current component
             * @deprecated This method was used by the cloning method reexecuting traverses (very slow)
             */
            //Component* createUnfoldsInstance();

 
            /**
            * @brief Make original component and its clone point to each other
             * @arg clone: the current component clone
             *
             */
            void setClone(Component* myclone) {this->clone = myclone; myclone->clone = this;};


            /** @brief  Return pointer to component  clone
             */
            Component* getClone()const  { return clone;};


            /**
             * \brief clone the current component 
             *
             * It includes cloning all sub components, ports and parameters and inserting this clone in the current application
             * (set correctly all pointers  to external - not cloned - components) 
             *
             * @return a pointer to the newly created clone
             */
            Component* cloneForTraverse();

      
            /**
             * @brief clone the current component except the  pointers between components.
             *
             * @warning Must be called from the orginal clone (modify  the clone)
             *
             * Method used by @see cloneForTraverse()
             *
             *
             */
            void cloneInternals();

            /**
             * \brief  Complete the clone creation by reproducing all pointers of the original components.
             *
             * @warning Must be called from the orginal clone (modify  the clone)
             *
             */
            void cloneLinks();


            /**
             * \brief  Clone the list of applied controllers
             *
             * @warning Must be called from the orginal clone (modify  the clone)
             *
             */
            void cloneControllerLog(const ControllerLog& contlog);

            
            /**
             * \brief Add a new port on the component
             *
             * General rules for adding a new port:
             * - port id must be unique
             * - port must be INPUT or OUTPUT
             * -  Message types (FULL/STAMPS/AUTO/SAMEAS) and blocking state (ST_UNKNOWN,ST_BLOCKING,ST_NONBLOCKING) only 
             *    make sense for  primitive components. They can be set anyway but will be ignored.
             * - port must be FULL, STAMPS, AUTO or SAMEAS. 
             * - only INPUT ports can be ST_BLOCKING or ST_NONBLOCKING
             * - SAMEAS/AUTO are for components that define the type of message carried depending on what the receiving component expect. 
             *   An AUTO OUPUT port takes the type (FULL/STAMPS) of the INPUT port of  primitive component it is linked to.
             *   A SAMEAS INPUT port  takes the type of the AUTO port (on the same component) it is related to (parameter s). 
             *   The SAMEAS/AUTO type are resolved when processing the application to FULL or STAMPS.
             *
             *
             *  Rules for setting SAMEAS and AUTO ports:
             * - a SAMEAS port must give a non empty string coresponding to a target port of the current component (parameter s). 
             * - a SAMEAS port can reference a non/not yet existing port
             * - a SAMEAS port must  reference an AUTO port
             * - a SAMEAS port can only be an INPUT port
             * - an AUTO port can only be an OUTPUT port
             * Tests are performed to enforce these conditions (exception thrown). Some tests cannot be performed in this method, in particular as the 
             * referenced port may not be created yet. Further tests are performed latter when processing the application.
             *
             * Other properties:
             * - a trace is added to any added port
             * - exceptions thrown are of type CustomException
             *
             * A few things are not enforced:
             * - 
             *
             * \param id id of the port (has to be unique among all ports, regardless of type)
             * \param p the type (i.e.: direction) of the port (INPUT/OUTPUT)
             * \param m the type of the message the  port carries (FULL/STAMPS/AUTO/SAMEAS)
             * \param s the name  of the port the message type should be copied from (only set for SAMEAS port, otherwise an exception is thrown)
             * \param blockState the blocking state of the port (ST_UNKNOWN,ST_BLOCKING,ST_NONBLOCKING). Default: ST_UNKNOWN
             *
             * \return a pointer to the newly created port.
             * \todo throw error when id is already used on the component.
             */
            Port * addPort(const std::string& id,
            		       const PortType& p,
            		       const PortMessageType& m = FULL,
            		       const std::string& s = "",
            		       Port::eBlckState blockState = Port::ST_UNKNOWN );


            /**
             * \brief clone all links (for traverse)
             */
            void cloneLinks(const Component* c);

            /**
             * \brief get the XML structure of the component (controller)
             * \return a pointer to the XML description
             * \note only primitive components return a non-nul pointer
             */
            virtual xml::DOMElement* XMLBuild() const {return NULL;}

            /**
             * \brief get the XML structure of run-file (controller)
             * \return a pointer to the XML description
             * \note only metamodules return a non-null pointer
             */
            virtual xml::DOMElement* XMLRunBuild() const {return NULL;}


            /**
             * \brief set host for a relative mapping
             * \param p pointer to the port
             * \throw CustomException if p is not a port of the component
             */
            void setRelativePortMapping(const Port* p);

            /**
             * \brief set host for a relative mapping
             * \param portId the id of the port
             * \throw CustomException if there is not port with this id
             */
            void setRelativePortMapping(const std::string& portId);

		    /**
             * \brief return a map with a pair (id, vector<host>) from informations taken from component hostlist.
             */
            virtual HostMap getHostMap() const { return HostMap(); }

            /**
             * \brief set info of the component
             */
            void setInfo(const std::string& value);


            /**
             * \brief  Put in this method all code necessary to build the component content that depends on connection to other external components (controller)
             */
            virtual void execute() {}


            /**
             * \brief host mapping  (controller)
             *  Set the hosts associated to the current component
             *  Method only implemented for Component and overwritten for  base primitive objects (Filter,  Sync, RoutingNode)  and Composite
             *  This method calls the secondary setHosts() controller that any component can implement.
             *  We introduced this intermediary controller instead of calling directly the setHosts() controller
             *  to control when to call setHosts(): not called if host sets from CSV file (host setting from
             *  CSV file must have the highest priority for Filter,  Sync, RoutingNode and MetaModules)
             * 
             * This mehtod is also used to enforce that setHosts is called first on the paretn before to be applied on the children (otherwise may causes errors when the parent change the child hostList)
             */
            virtual void setHostsMainController();


            /**
             * \brief host mapping (secondary controller)
             *  Set the hosts associated to the current component. This controller is called by the setHostsMainController() method and not directly.
             *  Any component can implement its specific setHosts() method (in opposite to setHostsMainController() )
             */
            virtual void setHosts() {}

            /*
             * brief host mapping dedicated to -L option (all components on localhost)
             */
            virtual void setLocalHost(){}


            /**
             * \brief Automatic insertion of connections (controller).
             *
             * This is a controller called during a fixed point traverse. This controller has only one implementation for composite components.
             */
            virtual void setConnections() {}


            /**
             * \brief Resolve AUTO/SAMEAS ports (controller)
             *
             * This is a controller called during a fixed point traverse. This controller has only  implementations for primitive components.
             */
            virtual void resolveMsgTypes() {}


            

            /**
             * \brief apply controller and add it on controller log
             */
            void applyController(InterfaceController* c, Component* cloned=NULL);

            /**
             * \brief this method sets the cache of type component.
             * \param t the new type for the cache
             * \warning Optimization issue in isSiblingCompatible. Don't use it.
             * \todo protect it better
             */
            void setCacheTypeComponent(TypeComponent t) { _cache_type = t; }
            /**
             * \brief get value of a parameter.
             * \param name : name of the parameter to look for
             * \template T type of the exected retrun type
             * \throw exception if no value found
             * \warning recursive search on ascendant components (local component first, next  parent component, etc.)
             */
            template<class T>
            T getParameter(const std::string& name) const
            {
                try{
                    return convertTo<T>(getParameterT(name));
                } catch (const BadConversion& e)
                    {
                        throw CustomException(getType()
                        		              +" id='"
                        		              +getFullId()
                        		              +"': Fail to convert parameter "
                        		              +name
                        		              +"='"
                        		              +getParameterT(name)
                        		              +"'",__FUNCTION_NAME__);
                    }
            }



            /**
             * \brief Virtual method. Look for a parameter (called recursive because always used with its composite  version.
             * \param name  the name of the parameter we are looking for
             * \return pointer to the parameter (NULL if param not found)
             */
            //            virtual Parameter *  getParameterRecursive(const std::string& name) const = 0;


            /**
             * \brief create the hierarchy XML description (as fractal ADL)
             * \return the hierarchy description
             */
            virtual xml::DOMElement* HierarchyXMLBuild() const;



            /**
             * \brief set value of a parameter, according to the templated type
             * \param name the name of the parameter
             * \param value the value of the parameter
             * \param targetcomp the optional target component
             * \template T the type of the default value
             */
            template<class T>
                void setParameter(const std::string& name, const T& value, const std::string& targetcomp="")
            {
                setParameterT(name, toString<T>(value),targetcomp);
            }

            /**
             * \brief declare a new parameter without default value
             * \param name the name of the parameter
             */

            void addParameter(const std::string& name)
            {
            	addParameterT(name);
            }


            /**
             * \brief declare a new parameter with a  default value
             * \param name the name of the parameter
             * \param value the default value you want to set
             * \template T the type of the default value
             */
            template<class T>
                void addParameter(const std::string& name, const T& value)
            {
                addParameterT(name,true,toString<T>(value));
            }


            /**
             * \brief test if a given  controller has been applied on current component
             *
             * @param name the name of the controller (based on the registered controller name)
             * @return true if controler applied
             */
            const bool isControllerApplied(const std::string name) const
            {
                return controllerLog.isControllerApplied(name);
            }


            /**
             * \brief set the component host list from the list found in the csv file
             * The current hostlist is cleared only if a new host list is found in the csv.
             *  Virtual method implemented for filter, sync, routingnode and metamodule.
             *  Called when inserting a new component in the application  or when cloning for traverse.
             */
            virtual void setHostListfromCsv(){};


            /**
             * \brief set value of a parameter
             * \not to be called explicitly, use template method : setParameter<Type T>()
             */
            void setParameterT(const std::string& name, const std::string& value, const std::string& componentId_="");

            /**
             * \brief declare a new parameter
             * \param name parameter name
             * \param isdefault true if parameter has default value
             * \param defaultVal default value of parameter
             * not to be called explicitly, use instead the template method : addParameter<Type T>()
             */
            void addParameterT(const std::string &name,const bool isdefault = false, const std::string &defaultVal ="");

            /**
             * \brief get a parameter value
             * not to be called explicitly, use template method : getParameter<Type T>()
             */
            std::string getParameterT(const std::string& name) const;

        protected:
            /**
             * \brief customizable method to add something to the description
             * \return a list of node to add to root XML node. default empty list
             */
            virtual std::list<xml::DOMElement*> HierarchyXMLBuildAdd() const
            {
                std::list<xml::DOMElement*> result;
                return result;
            }


            /**
             * \brief this method gets the cache of type component.
             * \return type the type for the cache
             * \warning Optimization issue in isSiblingCompatible. Don't use it.
             */
            TypeComponent getCacheTypeComponent() const { return _cache_type ; }

            /**
             * \brief compute a value for local parameter. Result stored in paramSetAdd. Recursive method
             * \param name of the parameter to compute a value for
             * \param comp the id of the component that initially called this function (required because recursive function)
             * \return pointer to parameter set in the paramSetAdd list
             * first return value set by command line ( -P moduleId:parameterName=value )
             * then value set by parameterFile (-Q paramFile.txt)
             * then value given in file adl.xml if exists
             * then value set by a  'setParameter()' call in code
             * then look if a value has been set for the parameter in component's parent
             * and finally return defaultValue if it has been set in declaration
             */
            Parameter * computeParameterValue(const std::string& name,const Component* comp) const;



        private :


            /**
             * \brief create hierarchy xml file with just the interface and parameter map
             * \return XML hierarchy description of the component
             */
            xml::DOMElement* HierarchyXMLBuildInterface() const;




    };



    /**
     * \class Composite
     * \brief a component with children
     */
    class Composite : public Component
    {

        protected:

            std::list<Component*> children; ///< list of all children

        public :


            /**
             * \brief constructor
             * \param id_ the id of the component
             * \param type_ the type of the component (optional)
             */
            Composite(const std::string& id_, const std::string& type_="composite")
            : Component(id_, type_), children()
            {}

            /**
             * \brief destructor.
             * \warning destroy all children
             */
            virtual ~Composite();

            typedef std::list<Component*>::iterator pComponentIterator; ///< iterator to children pointers

            /**
             * \brief get the number of children
             * \return the number of children
             */
            size_t getComponentSize() const { return children.size(); }

            /**
             * \brief get the first child
             * \return iterator to the first child pointer
             */
            pComponentIterator getComponentBegin() { return children.begin(); }
            /**
             * \brief get the last child
             * \return iterator to the last child pointer
             */
            pComponentIterator getComponentEnd() { return children.end(); }

            /**
             * \brief component id is a child of this component ?
             */
            bool isComponentExist(const std::string& id) const;


            typedef std::list<Component*>::const_iterator ConstpComponentIterator; ///< iterator to children pointers
            /**
             * \brief get the first child
             * \return iterator to the first child pointer
             */
            ConstpComponentIterator getComponentBegin() const { return children.begin(); }
            /**
             * \brief get the last child
             * \return iterator to the last child pointer
             */
            ConstpComponentIterator getComponentEnd() const { return children.end(); }

            /**
             * \brief get the child with an id
             * \param id the id of the wanted component
             * \return pointer to the child
             */
            Component *  getComponent(const std::string& id) const;


            /**
             * \brief the type of component. Follow a parent link from a port and get the type of child.
             * \return the type.
             * \warning default value (if no children for example) is UNKNOWN
             */
            TypeComponent getTypeComponent() const;


            /**
             * \brief test if composite
             * \return true if composite
             */
            bool isComposite() const { return true; }

            bool isPrimitive() const { return false; }


            /**
             * \brief virtual copy constructor
             */
            virtual Component* create() const ;

            /**
             * \brief populate the list with all the primitive children of a composite.
             */
            void getPrimitiveChildren(std::list<Component*> &list_);


            /**
             * \brief  add a new child component to the current component
             * \tparam Object_ template giving the type of the child to add.
             * \param id_  id of the child added
             * \return pointer to the child
             * \throw CustomException if the id is already used by a children
             */
            template<class Object_>
                Object_ * addObject(const std::string& id_)
            {
                Object_ *  child = new Object_(id_);
                try
                {
                    insertObject(child);
                }
                catch (CustomException& e)
                {
                	delete child;
                	throw(e);
                }
                return child;
            }


            /**
             * \brief add a new child component to the current component
             * \param c pointer to the  component the new child is copied from  (a new instance is created using the create virtual copy constructor)
             * \return pointer to the new child
             * \throw  CustomException if the id is already used by a children
             * \warning This method should be used with care as it is very permissive:  Modifications made on the c component are not reproduced  on the child  inserted (see create virtual  method)
             */
            Component* addObject(const Component& c);


            /**
             * \brief add a component and link it to two ports.
             * \param Object_  the type of the component to add
             * \param id_  the id of the component to add
             * \param  outport a pointer to the source output port
             * \param  inportid  the id of the component  input port outport is linked to.
             * \param  outportid the id of the component output port
             * \param  inport  a pointer to the destination input port the outportid is linked to.
             * \return  a pointer to the newly added component.
             */
            template<class Object_>
                Object_ *  addObjectandLink(const std::string& id_,  Port * outport,  const std::string& inportid, const std::string& outportid,  Port * inport)
            {
                Object_ * c = addObject<Object_>(id_);
                link(outport,c->getPort(inportid));
                link(inport,c->getPort(outportid));
                return c;
            }


            /**
             * \brief add a component and link its "in" and "out" ports to two ports.
             * \param Object_  the type of the component to add
             * \param id_  the id of the component to add
             * \param  outport a pointer to the source output port  to be linked to the component "in" port
             * \param  inport  a pointer to the destination input port the "out" port is linked to.
             * \return  a pointer to the newly added component.
             */
            template<class Object_>
                Object_ *  addObjectandLink(const std::string& id, Port * outport, Port * inport)
                {
                	return  addObjectandLink<Object_>(id, outport,"in","out",inport);
                }



            /**
             * \brief Put in this method all code necessary to build the component
             *        content that depends on connection to other external components (controller)
             */
            virtual void execute() {}

            
            /**
             * \brief Automatic insertion of connections (controller)
             *
             * This is a controller called during a simple traverse. This controller has only an implementation for composite components.
             * No other implementation is expected.
             * For each of its primitive (and non connection) child:
             *  -#  add connections on input ports if missing. Type (full/stamps) depend on input port type.
             *
             * This controller is designed to be called once the @see execute and @see resolveMsgType controllers have been applied
             * (all component create and no more SAMEAS/AUTO ports)
             * We expect  input ports to be connected  only to a single primitive component (enforced in @see Port::XMLBuild() )
             */
            virtual void setConnections();

            /**
             *  brief return a map with a pair (id, vector<host>) from component hostlist
             */
            virtual HostMap getHostMap() const;

            /**
             * \brief host mapping (secondary controller)
             *
             * Composite propagates its host list to all children.
             * This is a low priority behavior only performed if the child host list is empty.
             * The host list propagation can be overwritten when the  execute() or setHosts()  controllers are  called on the children.
             */
            virtual void setHosts();

            /**
             * \brief host mapping  (controller)
             *  Simply  call the setHosts() controller
             */
            void setHostsMainController();

            /**
             * \brief clone current component and all sub components with their ports and parameters  (but does not take care of pointers between ports)
             *
             * Indirect recursive method working with @see  Component::cloneInternals()
             * @warning Must be called from the original clone (modify  the clone)
             *
             */
            void  cloneChildren();
            

            /**
             * \brief  Complete the clone creation by reproducing all pointers of the original components.
             *
             * @warning Must be called from the original clone (modify  the clone)
             *
             * Indirect recursive method working with @see  Component::cloneLinks()
             *
             */
            void cloneChildLinks();
            

            /**
             * \brief Insert a component as a new child of the current component.
             *  Low level method (private). Use \see addObject() to add components
             * \param child pointer to the component to add
             * \throw CustomException if the id is already used by a children
             */
             void  insertObject(Component * child);



             /*
              * brief controller called by traverse in case of -L option use.
              */
             void setLocalHost();


        protected:
            /**
             * \brief add links to the hierarchy description
             * \return the list of links description
             */
            virtual std::list<xml::DOMElement*> HierarchyXMLBuildAdd() const;


    };

    /**
     * \class Primitive
     * \brief a component without children
     */
    class Primitive : public Component
    {
		private:


        public :


            /**
             * \brief constructor
             * \param id_ id of the component
             * \param type_ type of the component
             */
            Primitive(const std::string id_, const std::string& type_) : Component(id_, type_){
            }

            /**
             * \brief virtual destructor
             */
            virtual ~Primitive(){}


            bool isComposite() const { return false; }

            bool isPrimitive() const { return true; }

            /**
             * \note this method MUST be implemented by all inherited classes
             */
            virtual TypeComponent getTypeComponent()const  = 0;
            

       
            /**
             * \brief Resolve AUTO/SAMEAS ports (controller)
             *
             * This controller acts in two steps:
             *  -# resolve AUTO ports to FULL or STAMPS (only output ports can be auto). In case this is not possible because the target port of the connected  primitive object as the SAMEAS type an exception is thrown, catch by the traverse, so this component can be processed latter, hopefully when the SAMEAS port of the partget componenent has been resolved.
             *  -# resolve SAMEAS ports to FULL or STAMPS (only input ports can be auto). 
             *
             *  Connections have their own version of this controller that overwrite this one (@see connection.comp.h)
             * 
             *  We expect  the @see execute controller be applied before (all components are created)
             */
            virtual void resolveMsgTypes();


            /**
             * \brief get the XML structure of the component (controller)
             * \return a pointer to the XML description
             */
            virtual xml::DOMElement* XMLBuild() const;

            /**
             * \brief host mapping (secondary controller).
             */
            virtual void setHosts(){};


            /*
             * brief controller called by traverse in case of -L option use.
             */
            void setLocalHost();
            
            /**
             *  brief return a map with a pair (id, vector<host>) from component hostlist
             */
            virtual HostMap getHostMap() const;
            


    };


    /**
     * \brief Specialized version of  getParameter for string
     */
    template<>
        inline    std::string Component::getParameter(const std::string& name) const
    {
        return getParameterT(name);
    }



 }

}
#endif //__COMPONENT__
