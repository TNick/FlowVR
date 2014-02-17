/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
 * INRIA                                                           *
 * ALL RIGHTS RESERVED.	                                           *
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
 * File: ./src/component.cpp                                       *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/connection.comp.h"
#include "flowvr/app/core/utils.h"
#include <algorithm>
#include <queue>
#include <list>

namespace flowvr
{
	namespace app
	{

	///////////////////////////////////////////////////////////////////////////
	// FUNCTORS USED IN THIS CODE
	///////////////////////////////////////////////////////////////////////////


    //Composite useless("useless","useless");

    struct FunctorCopyPort
    {
            Component* c;
            FunctorCopyPort(Component* c_) : c(c_)
            {}

            void operator() (const Port* p)
            {
                if (!c->hasPort(p->getId()))
				{
					c->addPort(p->getId(), p->getType(), p->getMsgType());
				}
            }

    };

    struct FunctorNotEmpty
    {
            bool operator() (const Port* p)
            {
            	return !(p->isChildrenLinksEmpty());
            }
    };


    /**
     * \brief add an XML node to a root
     */
    struct FunctorAddSon
    {
            xml::DOMElement* root; ///< XML root

            /**
             * \brief pass root through constructor
             * \param root_ the XML root
             */
            FunctorAddSon(xml::DOMElement* root_)
            : root(root_)
            {}

            /**
             * \brief the link method
             */
            void operator() (xml::DOMElement* son)
            {
                root->LinkEndChild(son);
            }
    };


    struct FunctorMakeLink
    {
		struct FunctorCopyAllPortLinks
		{
				Port* operator() (const Port* pInit, Port* pDest)
				{
					const Component* const host =  pDest->getOwner();
					Composite* const parent = (Composite* const)host->getParent();
					for (Port::ConstSiblingLinksIterator it = pInit->getSiblingLinksBegin();
                                             it != pInit->getSiblingLinksEnd();
                                             ++it)
					{
						Component* sibling = parent->getComponent((*it)->getOwner()->getId());
						link(pDest, sibling->getPort((*it)->getId()));
					}


                                        for (Port::ConstParentLinksIterator it = pInit->getParentLinksBegin();
                                             it != pInit->getParentLinksEnd();
                                             ++it)
					{
                                            link(pDest, parent->getPort((*it)->getId()) ); 
					}


					return pDest;
				}

		};

		Component* operator() (const Component* const cInit, Component* const cDest)
		{
			FunctorCopyAllPortLinks cAllPort;
			std::transform(cInit->getPortBegin(),
						   cInit->getPortEnd(),
						   cDest->getPortBegin(),
						   cDest->getPortBegin(),
						   cAllPort);
			return cDest;
		}
    };


    struct FunctorXMLResultPort
    {
            xml::DOMNode* result;
            PortType t;

            FunctorXMLResultPort(const PortType& t_)
            : result(new xml::DOMElement( t_==INPUT ? "input" : "output" ))
            , t(t_)
            {}

            void operator() (Port* elt)
            {
                if (elt->getType() == t)
                    result->LinkEndChild(elt->XMLBuild());
            }
    };

    class BlockingStateCheck : public std::unary_function<bool, Port*>
    {
    public:
    	bool operator()( const Port *o ) const
    	{
    		if( ( o->getIsBlocking() == Port::ST_BLOCKING ) or (o->getIsBlocking() == Port::ST_UNKNOWN ) )
    		{
    			return !(o->getSiblingLinksSize() == 0); // connected ports will block, unconnected not
//				std::list<const Port*> primlist;
//				o->getPrimitiveSiblingList(primlist);
//				return ( !primlist.empty() );
    		}

    		return false;
    	}
    };

    ////////////////////////////////////////////////////////////////////////////
    // IMPLEMENTATION
    ////////////////////////////////////////////////////////////////////////////



    // Need declaration of static data member of Component here
    HostMap * Component::csvHostMap;



    std::string Component::getFullId() const {
        if(parent)
            return parent->getFullId()+idsep+id;
        return id;
    }



    Port* Component::getPort(const std::string& id) const
    {
        SameIdPtr<Port> sameId(id);
        Component::ConstpPortIterator it = std::find_if(ports.begin(), ports.end(), sameId);
        if (it == ports.end())
            throw IdNotFoundException("port", id, "Component", getFullId(), __FUNCTION_NAME__);
        return *it ;
    }


    bool Component::hasPort(const std::string& id) const
    {
        SameIdPtr<Port> sameId(id);
        return (std::find_if(ports.begin(), ports.end(), sameId) != ports.end());
    }


    bool Component::getIsBoundByInports() const
    {
    	ConstpPortIterator it = std::find_if( ports.begin(), ports.end(), BlockingStateCheck() );
    	if( it == ports.end() )
    		return false;

    	return true;
    }

    void Component::setInfo(const std::string& value)
    {
        setParameter("info",value);
    }



    void Component::setParameterT(const std::string& name, const std::string& value, const std::string& targetcomp)
    {
        Parameter param(name,  value, Parameter::CODE, getFullId(),targetcomp);
        //Check if same parameter already in the list
        //if (  paramListSet.eraseDuplicates(param) )
        //            {
                //                Msg::warning(this->getType()+" id='"+this->getFullId()+" setParameter "+ param.getTargetComp()+":"+param.getName()+"="+param.getValue()+" erases previous parameter value", __FUNCTION_NAME__);
        //            }
        paramListSet.eraseDuplicates(param);
        paramListSet.push_front(param);
    }
    void Component::addParameterT(const std::string &name, const bool isdefault, const std::string &defaultVal)
    {

        ParameterList::iterator it;

        //Check if same parameter already declared
        if( ! paramListAdd.empty() )
            {
                for(it=paramListAdd.begin(); it!=paramListAdd.end(); ++it){
                    if(it->getName() == name )
                        {
                            throw CustomException(getType()+" id=" +getFullId()+": parameter :" + name + " already declared (should be declared once only)", __FUNCTION_NAME__);
                        }
                }
            }

        Parameter tmp(name,isdefault,defaultVal);

        // look for duplicates.
        paramListAdd.push_back(tmp);
    }



    std::string Component::getParameterT(const std::string& name) const
    {
        return  computeParameterValue(name,this)->getValue();
    }


    Parameter * Component::computeParameterValue(const std::string& name, const Component* comp) const
    {

        ParameterList::iterator itadd = comp->paramListAdd.getParam(name); // get  parameter to compute

        // Parameter does not exists (yet)
        if ( itadd  == comp->paramListAdd.end() )
			throw CustomException(comp->getType()
					              + " id='"
					              + comp->getFullId()
					              + "' parameter '"
					              + name
					              + "' not declared.",__FUNCTION_NAME__);


        Parameter::TypeFrom search[3]= {Parameter::CMDLINE,
                                        Parameter::PARAMFILE,
                                        Parameter::ADL}; // order meaningfully: correspond to priority order
        ParameterList::const_iterator itset;
        std::string targetcomp;
        std::string fullid= getFullId();


        if (  paramListSetExternal != NULL && ! paramListSetExternal->empty() )
		{
			// Look if  parameter set from  CMDLINE, PARAMFILE and ADL (order meaningful)
			for (int i = 0; i < 3; i++)
			{
				// Look for parameter with target corresponding to the current parameter full id.
				targetcomp = fullid;

				// get set parameter
				itset = paramListSetExternal->getParam(name, targetcomp, search[i]);

				if (itset != paramListSetExternal->end())
				{
					// We found a value for the parameter.
					// update added parameter state (mainly value)  from set parameter state
					itadd->setValue(*itset, itset->getSetFromComp()); // getSetFromComp() stores name of adl file or parameter file
					return &(*itadd);
				}
			}
		} // end if( ! paramListSetExternal.empty())


		if (!paramListSet.empty())
		{
			// Look for parameter set from the code.
			// Start with  the  target component and next remove on level up to the current comp id.

			targetcomp = comp->getFullId();
			if (targetcomp == fullid)
			{
				targetcomp = "";
			}
			else
			{
				if (targetcomp.compare(0, fullid.size(), fullid) != 0)
				{
					throw CustomException(getType() + " id='" + fullid
							+ "': Does not seem to be parent of " + comp->getType()
							+ " id='" + comp->getFullId() + "'", __FUNCTION_NAME__);
				}
				targetcomp.erase(0, fullid.size() + 1);// remove from compid the fullid of the current component + '/'
			}

			while (true)
			{
				// get parameter
				itset = paramListSet.getParam(name, targetcomp, Parameter::CODE);

				if (itset != paramListSet.end())
				{
					// We found a value for the parameter.

					// update added parameter state (mainly value)  from set parameter state
					itadd->setValue(*itset, fullid);
					return &(*itadd);
				}

				// exit loop if target empty.
				if (targetcomp.empty())
					break;

				// Parameter not found. Try with a more general target: use the  parent name as new target
				std::string::size_type lastsep = targetcomp.rfind(getIdSep());
				if (lastsep == std::string::npos)
				{
					// no separator in target name: test empty name
					targetcomp = "";
				}
				else
				{
					// separator found: remove the last '/' and all subsequent characters
					targetcomp.erase(lastsep);
				}
			}

		}

		// We are at the root node. Look for default value
		if (parent == NULL)
		{
			if (itadd->isDefaultSet())
			{
				// found default value
				itadd->setValuetoDefault(comp->getFullId());
				return &(*itadd);
			}
			else
			{
				// nothing found. Throw exception
				throw CustomException(comp->getType()
						             + " id='"
						             + comp->getFullId()
						             + "': No value found for parameter '"
						             + name
						             + "'", __FUNCTION_NAME__);
			}
		}

		// Now call the same function from the parent component
		return parent->computeParameterValue(name, comp);
    }

      
   Component* Component::cloneForTraverse()
   { 
       //        Msg::error("Cloning "+getFullId(),__FUNCTION_NAME__);
       
       Component* myclone = create();
       
       //  Make original and clone point to each other
       myclone->setClone(this);
       
       // Set parent for clone
       myclone->setParent(parent);
       
       // clone the component and sub components (everything except pointers between components)
       cloneInternals();
       
       // Now take care of pointers between  components.        
       //  Pointers related to links between components 
       cloneLinks();
       
       return myclone;
   }
          
  void  Component::cloneInternals() 
  {
      // Must be called from the original component and not the clone

      // clone parameters
      clone->paramListSetExternal = paramListSetExternal;
      // duplicate parameter list for traverse.  Required not to lose parameters that would be set from outside of the component
      clone->paramListSet.push_fromParameterList(paramListSet);
      clone->paramListAdd.push_fromParameterList(paramListAdd);
      // clone host list
      clone->hostList.clear();// clean hostlist from element that may have been added by constructor
      clone->hostList = hostList;

      // clone trace list
       clone->myTraceList.insert(clone->myTraceList.begin(),myTraceList.begin(),myTraceList.end());
       
       // clone controller log
       clone->cloneControllerLog(controllerLog);

       // cache component type
       clone->setCacheTypeComponent(_cache_type);   
       
       clone->adlDescr = adlDescr;
       
       //clone ports
       // First remove ports that may have been added before (constructor)
       for(std::list<Port*>::iterator it = clone->ports.begin(); it != clone->ports.end(); ++it)
           {
               delete (*it);
           }
       clone->ports.clear();
       // Now clone ports
       for(std::list<Port*>::iterator it = ports.begin(); it != ports.end(); ++it)
           {
               clone->ports.push_back((*it)->clonePort());
           }

       // clone pointer to port used for  mapping (pointer to one of its own ports)
       if( relativeMappingPort != NULL)  
           clone->setRelativePortMapping(relativeMappingPort->getClone());
       else
           clone->setRelativePortMapping(NULL);
       
       // if composite clone children
       Composite * thiscomposite = dynamic_cast<Composite *>(this);
       if ( thiscomposite )
           {
               thiscomposite->cloneChildren();
           }
       
  }
          

  void Composite::cloneChildren() 
  {
      // Reproduce all child components (recursive)
      Component* mychildclone;
      Composite * compositeclone = static_cast<Composite *>(clone);
      
      // First clean children that may have been added before (constructor)
      for(std::list<Component*>::iterator it = compositeclone->children.begin(); it != compositeclone->children.end(); ++it)
          {
              delete (*it);
          }
      compositeclone->children.clear();
      
      // Now add cloned children.
      for(std::list<Component*>::iterator it = children.begin(); it != children.end(); ++it)
          {
              mychildclone = (*it)->create();
               (*it)->setClone(mychildclone);
               mychildclone->setParent(compositeclone);
               compositeclone->children.push_back(mychildclone);
               (*it)->cloneInternals();
          }
  }
          

  void Component::cloneLinks() 
  {
      //  clone all internal links 
      for(std::list<Port*>::iterator it = ports.begin(); it != ports.end(); ++it)
          {
              (*it)->cloneLinks();
          }
      
      // Recurse on descendants if composite
      Composite * thiscomposite = dynamic_cast<Composite *>(this);
      if ( thiscomposite) thiscomposite->cloneChildLinks();
      
  }
          
          
  void Composite::cloneChildLinks()
  {
      // Recurse on descendants. 
      for(std::list<Component*>::iterator it = children.begin(); it != children.end(); ++it)
          {
              (*it)->cloneLinks();
          }
      
  }

 void Component::cloneControllerLog(const ControllerLog& contlog)
   {
       controllerLog.copyControllersFrom(contlog);
   }


 Port* Component::addPort(const std::string& id, const PortType& p, const PortMessageType& m, const std::string& s, Port::eBlckState blockState )
    {

        // check  id is unique
        SameIdPtr<Port> sameId(id);
        Component::ConstpPortIterator it = std::find_if(ports.begin(), ports.end(), sameId);
        if (  it != ports.end() )
            throw SameIdException("Port",id, "component", getFullId(), __FUNCTION_NAME__);
        

        // Only input ports can be blocking 
        if ( p == OUTPUT && ( (blockState == Port::ST_BLOCKING || blockState == Port::ST_NONBLOCKING)  )  )
            throw CustomException("OUTPUT Port p=(" + getFullId() + "," + id
                                  + "): OUTPUT port cannot be ST_BLOCKING or ST_NONBLOCKING", __FUNCTION_NAME__);


        if (m != SAMEAS && !s.empty())
            throw CustomException("Port p=(" + getFullId() + "," + id
                                  + "): set last optional argument of addPort to '" + s
                                  + "'. Should not be set (not a SAMEAS port)", __FUNCTION_NAME__);

        // Only output  ports can be AUTO
        if ( m == AUTO && p == INPUT )
            throw CustomException("INPUT Port p=(" + getFullId() + "," + id
                                  + "): INPUT port cannot be AUTO", __FUNCTION_NAME__);

        if (m == SAMEAS)
            {
                // Only input ports can be SAMEAS
                if ( p  == OUTPUT)
                    throw CustomException("OUTPUT Port p=(" + getFullId() + "," + id
                                          + "): OUTPUT port cannot be SAMEAS", __FUNCTION_NAME__);
                if( s.empty() )
                    {
                        throw CustomException("Port p=(" + getFullId() + ","
                                              + id
                                              + "): type set to SAMEAS, but SAMEAS is EMPTY '", __FUNCTION_NAME__);
                    }
                if (id == s)
                    {
                        throw CustomException("Port p=(" + getFullId() + "," + id
                                              + "): cannot have last argument of addPort set to '" + s
                                              + "'. This is my port id.", __FUNCTION_NAME__);
                    }

                // check that the target SAMEAS port is AUTO (test can only be done if target port already declared)
                Port * p = NULL;
                try
                    {
                        p = getPort(s);
                    }
                catch (CustomException &e)
                    {
                        // Nothing.  port 's' is probably not  declared yet.
                    }

                if (p != NULL && p->getMsgType() != AUTO)
                    {
                        throw CustomException("Port p1=(" + getFullId() + "," + id
                                              + "): message type supposed to be SAMEAS p2=("
                                              + getFullId() + "," + s
                                              + "). But p2 has not the AUTO message type",
                                              __FUNCTION_NAME__);
                    }
            }

        Port *port = new Port(id, this, p, m, s, blockState);
        ports.push_back(port);

        myTraceList.push_back(port->getId());
        return port;
    }


     void    Component::setHostsMainController()
     {
                if (this->getParent() != NULL &&  ! this->getParent()->isControllerApplied("setHosts") )
                    throw CustomException("Component "+ getFullId()+": setHosts controller not yet applied on parent component "+ this->getParent()->getFullId()+". Cannot apply setHosts to current component now", __FUNCTION_NAME__);
     }

    void Component::applyController(InterfaceController* c, Component* cloned)
    {
		if (!cloned)
			c->setComponent(this);
		else
		{
			c->setComponent(cloned);
			c->setComponentClone(this);
		}

		InterfaceController* controllerClone = c->clone();
		try
		{
			(*c)();
		}
		catch (CustomException &e)
		{
			delete controllerClone;
			throw e;
		}
                controllerLog.registerController(controllerClone);
    }



    void Primitive::setLocalHost()
        {
    		hostList.clear();
        	hostList.push_back(Host("localhost","-L option"));
        }




    void Composite::setLocalHost()
      {
      		// only propagate mapping informations from parent to children.
      		Host localhost("localhost","-L option");
  			for(std::list<Component*>::iterator i = getComponentBegin(); i != getComponentEnd(); i ++)
  			{
  				(*i)->hostList.clear();
  				(*i)->hostList.push_back(localhost);
  			}
  		}


    Component * Composite::getComponent(const std::string& id)  const
    {
        SameIdPtr<Component> sameId(id);
        Composite::ConstpComponentIterator it = std::find_if(children.begin(), children.end(), sameId);
        if (it == children.end())
			throw IdNotFoundException("component", id, "component", getFullId(), __FUNCTION_NAME__);
        return *it;
    }


    bool Composite::isComponentExist(const std::string& id) const
    {
        SameIdPtr<Component> sameId(id);
        return (std::find_if(children.begin(), children.end(), sameId) != children.end());
    }


    TypeComponent Composite::getTypeComponent() const
    {
        if (getCacheTypeComponent() != UNKNOWN)
            return getCacheTypeComponent();

        ConstpPortIterator it = std::find_if(ports.begin(), ports.end(), FunctorNotEmpty());

        if (it == ports.end())
            return UNKNOWN; // default value

        TypeComponent result;
        try
		{
			result = (*((*it)->getYoungestDescendantsBegin()))->getOwner()->getTypeComponent();
		}
        catch(const CustomException& e)
		{
			result= UNKNOWN;
		}
        return result;
    }


    void  Composite::insertObject(Component * child)
    {
        // check  id unique
        for(std::list<Component*>::const_iterator it = children.begin(); it != children.end(); ++it)
		{
			if ((*it)->getId() == child->getId())
				throw SameIdException("Component", child->getId(), "component", getFullId(), __FUNCTION_NAME__);
		}

        // add the new component to its parent
        child->setParent(this);
        children.push_back(child);

        // Provide pointer to parameters set externally
        child->paramListSetExternal = paramListSetExternal;

        // init hostlist from csv (virtual method: behavior depends on component type)
        child->setHostListfromCsv();

       // if (child->csvHostMap == NULL)
        //	Msg::warning(child->getFullId()+" csvHostMap Null",__FUNCTION_NAME__);
        //else
        //Msg::warning(child->getFullId()+" csvHostMap not Null",__FUNCTION_NAME__);


        //Update child adlDescr
        child->adlDescr = NULL;
        if(adlDescr)
		{
			//Try to find my child in ADL
			xml::DOMElement* itChild = adlDescr->FirstChildElement("component");
			bool not_found = true;
			while(itChild && not_found)
			{
				if(itChild->Attribute("id")  == child->getFullId())
				{
					not_found = false;
					child->adlDescr = itChild;
				}
				itChild = itChild->NextSiblingElement();
			}
		}
    }

    Component* Composite::addObject(const Component& c)
    {
		Component * child = c.create();
		try
		{
			insertObject(child);
		}
		catch (CustomException& e)
		{
			delete child;
			throw e;
		}
		return child;
    }

    void Component::setRelativePortMapping(const Port* p)
    {
        if ( p != NULL && p->getOwner() != this)
            {
                throw IdNotFoundException("port", p->getId(), "component", getFullId(), __FUNCTION_NAME__);
            }
        relativeMappingPort = p;
    }

    void Component::setRelativePortMapping(const std::string& portId)
    {
        SameIdPtr<Port> sameId(portId);
        pPortIterator p = std::find_if(ports.begin(), ports.end(), sameId);
        if (p == ports.end())
            throw IdNotFoundException("port", portId, "component", getFullId(), __FUNCTION_NAME__);
        relativeMappingPort = *p;
    }

    xml::DOMElement* Component::HierarchyXMLBuild() const
    {
        xml::DOMElement* interface = this->HierarchyXMLBuildInterface();
        std::list<xml::DOMElement*> addToInterface = HierarchyXMLBuildAdd();
        std::for_each(addToInterface.begin(), addToInterface.end(), FunctorAddSon(interface));
        return interface;
    }

    xml::DOMElement* Component::HierarchyXMLBuildInterface() const
    {
        flowvr::xml::DOMElement * interface = new flowvr::xml::DOMElement(getType());
        interface->SetAttribute("id",getFullId());

        /*
        // do not create the node 'hosts' if we have no host.
        if( hostList.size() > 0 ) 
            {
                xml::DOMElement* hostlist = new xml::DOMElement("hosts");

                for( HostList::const_iterator it = hostList.begin(); it != hostList.end(); it++ )
                    {
                        // use text nodes and not attribute to allow complex strings in origin (strong restriction applied on attributes' values)
                        xml::DOMElement* host = new xml::DOMElement("host");
                        xml::DOMElement* name = new xml::DOMElement("name");                
                        xml::DOMElement* origin = new xml::DOMElement("from");

                        name->LinkEndChild(new xml::DOMText( (*it).getName()));
                        host->LinkEndChild(name);
                        origin->LinkEndChild(new xml::DOMText( (*it).getOrigin()));
                        host->LinkEndChild(origin);
                        hostlist->LinkEndChild(host);
                    }
                interface->LinkEndChild(hostlist);
            }
        */

        // Ports
        for(ConstpPortIterator it = getPortBegin(); it != getPortEnd(); it++ )
		{
			xml::DOMElement* port = new xml::DOMElement("port");
			port->SetAttribute("id", (*it)->getId());
			port->SetAttribute("type", ((*it)->getType() == INPUT ? "INPUT" : "OUTPUT" ));
			port->SetAttribute("msgtype",  (*it)->getMsgTypeString());
                        if ((*it)->getType() == INPUT)
                            port->SetAttribute("blockstate",  (*it)->getIsBlockingString());
			interface->LinkEndChild(port);
		}

        // Host List
        if( hostList.size() > 0 ) 
            {
                xml::DOMElement* hostlist = new xml::DOMElement("hostlist");
                for( HostList::const_iterator it = hostList.begin(); it != hostList.end(); it++ )
                    {
                        xml::DOMElement* host = new xml::DOMElement("host");
                        host->SetAttribute("name", (*it).getName());
                        host->SetAttribute("from", (*it).getOrigin());
                        hostlist->LinkEndChild(host);
                    }
                interface->LinkEndChild(hostlist);
            }



        // Fill xml node parameter and at the same time print debug messages about parameters.
        xml::DOMElement* paramlist = new xml::DOMElement("parameters");
		if (!paramListAdd.empty())
		{
			for (ParameterList::const_iterator it = paramListAdd.begin(); it != paramListAdd.end(); ++it)
			{
				try
				{
					computeParameterValue(it->getName(), this); // compute the parameter value
					paramlist->LinkEndChild(it->XMLBuild(true));
					if (it->getName() != "info")
					{
						Msg::debug(
                                                           "Parameter " + getFullId() + ":" + it->getName()
                                                           + "='" + it->getValue() + "' set from "
                                                           + (it->getSetFrom() != Parameter::CODE ? it->getStringSetFrom() : "component id=" + it->getSetFromComp())
                                                           + (!it->getTargetComp().empty() && it->getSetFrom() == Parameter::CODE ? " (target component='"
                                                           + it->getTargetComp() + "')" : ""), __FUNCTION_NAME__);
					}
				}
				catch (const CustomException& e)
				{
					// We do not do anything. Just produce the xml.
				}
			}
			interface->LinkEndChild(paramlist);
		}
        return interface;
    }


    void Primitive::resolveMsgTypes()
    {
        PortMessageType msgtype;
        bool connected;
          
        // RESOLVE AUTO PORTS
        for (pPortIterator p = getPortBegin(); p != getPortEnd(); p++)
            {

                //Msg::info("SetConnections: working with  p=("+ getFullId() + ", " + (*p)->getId()+ ")", __FUNCTION_NAME__);

                // Define the type of messages carried by the port if something connected to it
                // We assume only output ports can be AUTO (checked when the port was built)
                if ( (*p)->getMsgType() == AUTO)
                    {
                                
                        // Take the most  general type  of the primitives port linked  to it (FULL>STAMPS>SAMEAS/AUTO)
                        msgtype = SAMEAS;
                        connected = false;

                        std::list<const Port*> primlist;
                        (*p)->getPrimitiveSiblingList(primlist);

                        for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
                            {
                                connected = true;
                                if (  msgtype != FULL &&  (*it)->getMsgType() == STAMPS)
                                    msgtype = STAMPS;
                                else if ( (*it)->getMsgType() == FULL )
                                    msgtype = FULL;
                            }
                        if ( connected )
                            {
                                if (msgtype == SAMEAS)
                                    {
                                        // Throw exception to request execution of this controller later (SAMEAS as port need to be resolved first)
                                        throw CustomException("Output port p=(" + getFullId() + ", " + (*p)->getId()
                                                              + "):  its AUTO type cannot be resolved to FULL or STAMPS: "
                                                              +" All connected  primitive components have SAMEAS ports",
                                                              __FUNCTION_NAME__);
                                    }
                                else 
                                    {
                                        (*p)->setMsgType(msgtype);
                                    }
                            }
                        else // Not connected to any primitive
                            {
                                // Throw exception : port not connected. Cannot resolve AUTO status).
                                throw CustomException("Output port p=(" + getFullId() + ", " + (*p)->getId()
                                                      + "): its AUTO type  cannot be resolved to FULL or STAMPS: "
                                                      +" Not connected to any primitive component",
                                                      __FUNCTION_NAME__);
                            }
                    }
            }
                    
        // RESOLVE SAMEAS  PORTS (we assume all AUTO ports the component  have been resolved to FULL or STAMPS)
        for (pPortIterator p = getPortBegin(); p != getPortEnd(); p++)
            {
                if ((*p)->getMsgType() == SAMEAS)
                    {
                        try
                            {
                                msgtype = getPort((*p)->getSameAs())->getMsgType();
                                if (msgtype != AUTO)
                                    (*p)->setMsgType(msgtype);
                                else
                                    //should never happen
                                    throw CustomException("Port p=(" + getFullId() + ", " + (*p)->getId()
                                                          + "):  Message type expected to be SAMEAS port p=("
                                                          + getFullId() + ", " + (*p)->getSameAs()
                                                          +") but this one is still "
                                                          + getPort((*p)->getSameAs())->getMsgTypeString(),
                                                          __FUNCTION_NAME__);

                            }
                        catch (const CustomException& e)
                            {
                                throw CustomException(
                                                      "Port p=(" + getFullId() + ", " + (*p)->getId()
                                                      + "): unable   to know  if FULL or STAMPS:  Message type expected to be SAMEAS port p=("
                                                      + getFullId() + ", " + (*p)->getSameAs()
                                                      + ") but error when trying to know if its a  FULL or STAMP port. Exception raised is:"
                                                      + e.what(), __FUNCTION_NAME__);
                            }
                    }
            }
    }


    void Composite::setConnections()
    {
        for ( pComponentIterator child = getComponentBegin(); child != getComponentEnd(); ++child )
            {       
                // Only work on non connection primitive object
                if ( !(*child)->isComposite()  &&  !dynamic_cast<const ConnectionBase* const>(*child) )
                    {
                        // ADD MISSING CONNECTIONS (we assume there is no AUTO or SAMEAS ports left here)
                        ConnectionBase * c;
                        for ( pPortIterator p = (*child)->getPortBegin(); p != (*child)->getPortEnd(); ++p )
                            {
                                // Look at connected input ports
                                std::list<const Port*> primlist;
                                (*p)->getPrimitiveSiblingList(primlist);
        
                                if (  (*p)->getType() == INPUT &&  !primlist.empty() )
                                    {
                                        // Only one primitive components can be connected to an input port 
                                        // (use an appropriate  filter just before the port if this is the kind of behavior you expect)
                                        if ( primlist.size() > 1 )
										{
											// build a string with list of primitive components.
											std::string list("");
											for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
											{
												list += "\t- (";
												list += (*it)->getOwner()->getFullId();
												list += ", " + (*it)->getId() + ")\n";
											}
											throw CustomLinkException("Input Port  p=("+ (*child)->getFullId() + ", " + (*p)->getId() +"): linked to multiple primitive components (at most one expected). These primitives are:\n"+list, __FUNCTION_NAME__);
										}
                                
                                        // get the port of the primitive component the current component is linked  to 
                                        const Port * sibp = *(primlist.begin());
                                        // if not a connection need to add one
                                        const ConnectionBase * const dconn = dynamic_cast<const ConnectionBase* const>(sibp->getOwner());
                                        if ( !dconn)
                                            {
                                                if( (*p)->getMsgType() == FULL)
                                                    {
                                                        // Input port is FULL: add a FULL connection
                                                
                                                        if (sibp->getMsgType() == STAMPS)
                                                            throw CustomException(
                                                                                  "FULL input port p=(" + (*child)->getFullId() + ", " + (*p)->getId()
                                                                                  + ")  connected to  STAMPS output port"
                                                                                  + sibp->getOwner()->getFullId()
                                                                                  + ", "
                                                                                  + sibp->getId()
                                                                                  + "): not allowed. Automatic connection insertion failed",
                                                                                  __FUNCTION_NAME__);

                                                        c = new Connection((*child)->getId()+"."+(*p)->getId());
                                                    }
                                                else if ((*p)->getMsgType() == STAMPS)
                                                    {
                                                        // Input port is STAMPS: add a STAMPS connection
                                                        c= new ConnectionStamps((*child)->getId()+"."+(*p)->getId());
                                                    }   
                                                else    
                                                    {
                                                        // SHOULD NEVER HAPPEN: AUTO ports are resolved at this point
                                                        throw CustomException(
                                                                              "Input port p=(" + (*child)->getFullId() + ", " + (*p)->getId()
                                                                              + ")  connected to  AUTO  output port p=("
                                                                              + sibp->getOwner()->getFullId()
                                                                              + ", "
                                                                              + sibp->getId()
                                                                              + "): Something wrong! AUTO status should have been resolved before. Automatic connection insertion failed",
                                                                              __FUNCTION_NAME__);
                                                
                                                    }
                                        
                                        
                                                // Find the target port to connect the new connection to. We have the port of the current component (p)
                                                // and the port of the target primitive component (sibp) , but now we have to find the first port on the path to the primitive component (tp).
                                                Port * tp;
                                                bool foundport = false;
                                                std::list<const Port*> tpprimlist;
                                                        
                                        
                                                // first look if the target primitive component is outside the parent component
                                                for (Port::ConstParentLinksIterator itpl = (*p)->getParentLinksBegin(); itpl != (*p)->getParentLinksEnd(); ++itpl)
                                                    {
                                                        // Target primitive component is outside the parent component. 
                                                        tp = (*itpl); 
                                                        tp->getPrimitiveSiblingList(tpprimlist);

                                                        // Check that the target  primitive port is connected to the parent port tp (outside of the parent)  
                                                        for(std::list<const Port*>::const_iterator it = tpprimlist.begin(); it != tpprimlist.end(); ++it)
                                                            {
                                                                if ( (*it) == sibp)
                                                                    {
                                                                        foundport = true;
                                                                        break;
                                                                    }
                                                            }
                                                        if(foundport) break;
                                                    }
                                                // look if target primitive component is a  sibling or inside a sibling component
                                                if ( !foundport) 
                                                    {
                                                        // Check that the target  primitive port is acutally connected to tp
                                                
                                                        for(Port::ConstSiblingLinksIterator it = (*p)->getSiblingLinksBegin();   it != (*p)->getSiblingLinksEnd(); it++)
                                                            {
                                                                for(Port::YoungestDescendantsIterator itYoung = (*it)->getYoungestDescendantsBegin(); itYoung != (*it)->getYoungestDescendantsEnd(); ++itYoung)
                                                                    {
                                                                        if ( (*itYoung) == sibp)
                                                                            {
                                                                                foundport = true;
                                                                                break;
                                                                            }         
                                                                    }
                                                                if (foundport)
                                                                    {
                                                                        // Target primitive component is inside a  sibling component
                                                                        tp = *it;
                                                                        break;
                                                                    }
                                                            }
                                                    }
                                                if (!foundport)  // we did not find the target primitive component (should never happen)
                                                    {
                                                        // SHOULD NEVER HAPPEN
                                                        throw CustomException(
                                                                              "Input port p=(" + (*child)->getFullId() + ", " + (*p)->getId()
                                                                              + ")  supposed to be connected to port p=("
                                                                              + sibp->getOwner()->getFullId()
                                                                              + ", "
                                                                              + sibp->getId()
                                                                              + "): But intermediate port not found. Something wrong!  Automatic connection insertion failed",
                                                                              __FUNCTION_NAME__);
                                                    }
                                        
                                                //Add a new connection
                                                Component * compc = addObject(*c);
                                                // Unlink first and next link the  new connection.
                                                unlink(*p, tp);
                                                link(tp,compc->getPort("in"));
                                                link(compc->getPort("out"),*p);

                                                // Update the restricted primitive lists:
                                                
                                                // For performance purpose: restrict current port accessibility to added connection
                                                (*p)->addRestrictionToPrimitiveSibling(compc->getPort("out"));
                                             
                                                // For correctness: this is now the new connection  that has restricted access
                                                // to the target  primitive  and not anymore the current component

                                                (*p)->delRestrictionToPrimitiveSibling(sibp);

                                                // Also inspect the reverse case: *p can be in the restricted lists of other components
                                                // Change  to the new connection instead:
                                                
                                                // Work on a copy of reverseResPrimSib as delRestrictedPrimitiveSibling remove elements from this list.
                                                std::list<const Port*> copyreverse( (*p)->reverseResPrimSib);
                                                for( std::list<const Port*>::iterator it = copyreverse.begin(); it != copyreverse.end(); ++it)
                                                    {
                                                        (*it)->delRestrictionToPrimitiveSibling(*p);
                                                        (*it)->addRestrictionToPrimitiveSibling(compc->getPort("in"));
                                                    }
                                                
                                                // Add the target primitive in the restricted list of the connection 
                                                // (cannot be performed soonner as  sibp could be made non accessible by other restrictions from other components. 
                                                //  These restrictions are suppressed when processing (*p)->reverseResPrimSib - see code just above )
                                                compc->getPort("in")->addRestrictionToPrimitiveSibling(sibp);


                                                Msg::info("SetConnections: new connection "+ compc->getFullId()+ " linked to  port p=("
                                                          + tp->getOwner()->getFullId() + ", " + tp->getId()
                                                          + ") and port p=("
                                                          + (*p)->getOwner()->getFullId() + ", " + (*p)->getId()
                                                          + ") with target primitive p=("
                                                          + sibp->getOwner()->getFullId() + ", " + sibp->getId()+")",
                                                          __FUNCTION_NAME__);
                                            }
                                        else
                                            Msg::info("SetConnections: Input port p=(" +(*p)->getOwner()->getFullId() + ", " + (*p)->getId()
                                                      + ")  already linked to  connection  p=("
                                                      + sibp->getOwner()->getFullId()
                                                      + ", "
                                                      + sibp->getId()
                                                      + ")",
                                                      __FUNCTION_NAME__);
                                    }  
                            }
                    }
            }
    }

      
    void Composite::getPrimitiveChildren(std::list<Component*> &list_) {

    	for(std::list<Component*>::iterator it = children.begin(); it != children.end(); ++it)
		{
			if((*it)->isComposite()){
				dynamic_cast<Composite*>(*it)->getPrimitiveChildren(list_);
			}

			else if((*it)->isPrimitive()){
				list_.push_back(*it);
			}

		}
    }

    void Composite::setHosts()
    {
    	// Default low priority behavior for composite: propagate hostList to children
    	// This method can be overwritten by subclasses lile PatternParallelFromHosts()
        if(!hostList.empty())
            {
    		//Copy hostlist in tmp list and add info that hostlist propagated from current component
    		HostList h = hostList;
    		for (HostList::iterator it =h.begin(); it!=h.end();it++)
                    {
    			(*it).appendtoOrigin(getId());
                    }
    		for(std::list<Component*>::iterator i = getComponentBegin(); i != getComponentEnd(); ++i)
                    {
    			// Propagate hostlist to child components only if their host list is empty
                        if ( (*i)->hostList.empty() )
    			{
                            (*i)->hostList=h;
    			}
                        
                    }
            }
    }

     void Composite::setHostsMainController()
    { 
        Component::setHostsMainController();
        setHosts(); 
    }

    Component* Composite::create() const
    {
        return new Composite(getId(), getType());
    }

    Composite::~Composite()
    {
        for(std::list<Port*>::iterator it = ports.begin(); it != ports.end(); ++it)
            (*it)->flushChildrenLinks();

        std::for_each(children.begin(), children.end(), DeletePtr<Component>());
    }

    Component::~Component()
    {
        //delete paramlist;
        std::for_each(ports.begin(), ports.end(), DeletePtr<Port>());
        // delete clone pointer
        if (clone != NULL) 
			clone->clone = NULL;
    }


    std::list<xml::DOMElement*> Composite::HierarchyXMLBuildAdd() const
    {
    	std::list<xml::DOMElement*> linkList;
    	for(ConstpComponentIterator  it = getComponentBegin(); it != getComponentEnd(); ++it)
    	{
    		for (ConstpPortIterator itPort = (*it)->getPortBegin(); itPort != (*it)->getPortEnd(); ++itPort)
    		{
    			// Links are  oriented from source to destination (output->input)
    			if ((*itPort)->getType() == OUTPUT)
    			{
    				// Sibling Links
    				// avoid to count 2 times sibling links. So we assume source is an output port.
    				// This method works because encapsulation is strict.
    				for (Port::ConstSiblingLinksIterator itLink = (*itPort)->getSiblingLinksBegin();
    						itLink != (*itPort)->getSiblingLinksEnd(); ++itLink)
    				{
    					xml::DOMElement* link = new xml::DOMElement("link");
    					link->SetAttribute("source", (*itPort)->getOwner()->getFullId());
    					link->SetAttribute("outport", (*itPort)->getId());
    					link->SetAttribute("dest", (*itLink)->getOwner()->getFullId());
    					link->SetAttribute("inport", (*itLink)->getId());
    					linkList.push_back(link);
    				}
    				// Parent/child link
    				for (Port::ConstParentLinksIterator itpl =  (*itPort)->getParentLinksBegin(); itpl !=  (*itPort)->getParentLinksEnd(); ++itpl)
    				{
    					xml::DOMElement* link = new xml::DOMElement("link");
    					link->SetAttribute("source", (*itPort)->getOwner()->getFullId());
    					link->SetAttribute("outport", (*itPort)->getId());
    					link->SetAttribute("dest", (*itpl)->getOwner()->getFullId());
    					link->SetAttribute("inport", (*itpl)->getId());
    					linkList.push_back(link);
    				}
    			}else {
    				// INPUT PORT
    				// Parent/child link
    				for (Port::ConstParentLinksIterator itpl =  (*itPort)->getParentLinksBegin(); itpl !=  (*itPort)->getParentLinksEnd(); ++itpl)
    				{
    					xml::DOMElement* link = new xml::DOMElement("link");
    					link->SetAttribute("source", (*itpl)->getOwner()->getFullId());
    					link->SetAttribute("outport", (*itpl)->getId());
    					link->SetAttribute("dest", (*itPort)->getOwner()->getFullId());
    					link->SetAttribute("inport", (*itPort)->getId());
    					linkList.push_back(link);
    				}
    			}
    		}
    	}
    return linkList;
	}


    flowvr::xml::DOMElement * Primitive::XMLBuild() const
    {
        flowvr::xml::DOMElement * e = new flowvr::xml::DOMElement(getType());
        e->SetAttribute("id",getFullId());
        
        // Check primitive has one and only one host
        if(hostList.empty())
        	throw CustomException("Primitive "+ getFullId()+" : no host for mapping (one expected) " , __FUNCTION_NAME__);
        else if(hostList.size() > 1)
        	throw CustomException("Primitive "+ getFullId()+" : "+toString<int>(hostList.size())+" hosts for mapping (one expected)", __FUNCTION_NAME__);
        else
        	e->SetAttribute("host",hostList.front().getName());
        
        FunctorXMLResultPort portXMLInput(INPUT);
        portXMLInput = for_each(ports.begin(), ports.end(), portXMLInput);
        e->LinkEndChild(portXMLInput.result);
        FunctorXMLResultPort portXMLOutput(OUTPUT);
        portXMLOutput = for_each(ports.begin(), ports.end(), portXMLOutput);
        e->LinkEndChild(portXMLOutput.result);
        // add parameters in .net.xml here
        xml::DOMElement* paramlist = new xml::DOMElement("parameters");
        if (!paramListAdd.empty())
            {
                for (ParameterList::iterator it = paramListAdd.begin(); it != paramListAdd.end(); ++it)
                    {
                        computeParameterValue(it->getName(), this); // compute the parameter value
                        paramlist->LinkEndChild(it->XMLBuild(false));
                    }
                e->LinkEndChild(paramlist);
            }
        
        if (getParameter<int> ("trace"))
            {
                xml::DOMElement* tracelist = new xml::DOMElement("tracelist");
                for (std::list<flowvr::app::Trace>::const_iterator i = myTraceList.begin(); i != myTraceList.end(); i++)
                    {
                        tracelist->LinkEndChild((*i).XMLBuild());
                    }
                e->LinkEndChild(tracelist);
            }
        return e;
    }


    HostMap Primitive::getHostMap() const
    {
    	HostMap result;
       if (getTypeComponent() != COMMUNICATION){
    		result[getFullId()].insert(result[getFullId()].begin(), hostList.begin(), hostList.end());
        }
        return result;
    }

    HostMap Composite::getHostMap() const
    {
    	HostMap result;
    	if (! hostList.empty() ){
    		result[getFullId()].insert(result[getFullId()].begin(), hostList.begin(),hostList.end());
    	}
    	return result;
    }

 } // namespace app
} // namespace flowvr
