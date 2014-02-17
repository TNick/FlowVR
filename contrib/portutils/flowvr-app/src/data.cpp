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
 * File: ./src/data.cpp                                            *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/msg.h"
#include "flowvr/app/core/data.h"


namespace flowvr { namespace app {

    Id::Id(const std::string& id_, const std::string& delimiter_) throw (IncorrectValueException) : delimiter(delimiter_)
    {
        if (id_.empty())
            throw IncorrectValueException("id_", __FUNCTION_NAME__);
        id = id_;
    };

    void Id::setId(const std::string& id_) throw(IncorrectValueException)
    {
        if (id_.empty())
            throw IncorrectValueException("id_", __FUNCTION_NAME__);
        id = id_;
    };

    void Id::addPrefix(const std::string& prefix_) throw(IncorrectValueException)
    {
        if (prefix_.empty())
            throw IncorrectValueException("prefix_", __FUNCTION_NAME__);
        if (id.empty())
            id = prefix_;
        else
            id = prefix_ + delimiter + id;
    };

    Data::Data(const Data& p) :   name(p.name), value(p.value), valstatus(p.valstatus)
    {
        children.clear();
        for( std::list<Data*>::const_iterator i = p.children.begin(); i != p.children.end(); ++i)
            {
                //	children.push_back(new Data(*(*i)));
                children.push_back((*i)->clone());
            }
    };

    Data* Data::getChild( const std::string& name_)   throw(IncorrectValueException)
    {

        if (name_.empty()) 
            throw IncorrectValueException("name_", __FUNCTION_NAME__);

        for (std::list<Data*>::iterator i = children.begin(); i != children.end(); ++i)
            {
                if ((*i)->getName() == name_)
                    return *i;
            }
        return NULL;

    };

    Data* Data::getDescendant( const std::string& name_) throw(IncorrectValueException)
    {
        if (name_.empty()) 
            throw IncorrectValueException("name_", __FUNCTION_NAME__);


        if ( name  == name_ ) return this;

        for (std::list<Data*>::iterator i = children.begin(); i != children.end(); ++i)
            {
                if ((*i)->getName() == name_)
                    return *i;
            }
        return NULL;

    };

    void Data::add(const Data& data_) throw (IncorrectValueException,SameIdException)
    {
        if ( data_.name.empty() )
            throw IncorrectValueException("data_", __FUNCTION_NAME__);
        if ( getChild(data_.getName()) != NULL) 
            throw SameIdException("data", data_.getName(), "data set", getName(), __FUNCTION_NAME__);

        // call the virtual constructor through clone to make sure we copy the full object  even if it's from a  Data sub-class
        children.push_back(data_.clone());
    };

    void Data::add(const std::string& id_, const std::string& value_, const ValStatus s_) throw (IncorrectValueException,SameIdException)
    {
        Data data(id_,value_,s_);
        add(data);
    };



    flowvr::xml::DOMElement*  Data::XMLBuild() const throw (IncorrectValueException)
    {
        flowvr::xml::DOMElement *  e = new flowvr::xml::DOMElement(getName()); 

        if (e)
            {
                if (valstatus != NO)
                    {
                        e->LinkEndChild(new flowvr::xml::DOMText(getValue()));
                    }

                for (std::list<Data*>::const_iterator i=getChildrenBegin(); i !=  getChildrenEnd(); i++)
                    {
                        flowvr::xml::DOMElement* childXML = (*i)->XMLBuild();
                        if (childXML)
                            e->LinkEndChild(childXML);
                    }
            }
        return e;
    };

    flowvr::xml::DOMElement*  Plugin::XMLBuild() const throw (IncorrectValueException)
    {
        flowvr::xml::DOMElement *  e = new flowvr::xml::DOMElement(getName()); 
        e->LinkEndChild(new flowvr::xml::DOMText("flowvr.plugins." + getValue()));
        return e;
    };

    flowvr::xml::DOMElement* Plugin::HierarchyXMLBuild() const throw (IncorrectValueException)
	{
        flowvr::xml::DOMElement *  e = new flowvr::xml::DOMElement(getName());
        e->LinkEndChild(new flowvr::xml::DOMText("flowvr.plugins." + getValue()));
        return e;
	}

    flowvr::xml::DOMElement*  SimpleDataType::XMLBuild() const throw (IncorrectValueException)
    {
        flowvr::xml::DOMElement * e= new flowvr::xml::DOMElement(getName()); 
        return e;
    };


    flowvr::xml::DOMElement*  BinaryDataType::XMLBuild() const throw (IncorrectValueException)
    {
        flowvr::xml::DOMElement *  e= new flowvr::xml::DOMElement(getName()); 
        e->SetAttribute("size",getValue());
        return e;
    };



    flowvr::xml::DOMElement*  ArrayDataType::XMLBuild() const throw (IncorrectValueException)
    {

        flowvr::xml::DOMElement*  e= new flowvr::xml::DOMElement(getName()); 
        e->SetAttribute("size",getValue());
        e->LinkEndChild((*getChildrenBegin())->XMLBuild());
        return e;
    };


    Trace::Trace(const std::string& id_) : Data(id_,NO)
    { 
        IntDataType i;
        Data::add(i);
    };


    flowvr::xml::DOMElement*  Trace::XMLBuild() const throw (IncorrectValueException)
    {

        flowvr::xml::DOMElement * e = new flowvr::xml::DOMElement("trace"); 
        e->SetAttribute("id",getName());
        flowvr::xml::DOMElement * d = new flowvr::xml::DOMElement("data");

        // Build XML for data type of trace
        d->LinkEndChild((* getChildrenBegin())->XMLBuild());
        e->LinkEndChild(d);
        return e;
    };

//    void TraceList::add(const std::string& id_)  throw (IncorrectValueException)
//    {
//        if(getChild(id_) != NULL)
//            throw SameIdException("trace", id_, "trace list", getName(), __FUNCTION_NAME__);
//
//        Trace  t(id_);
//        Data::add(t);
//    };



}; }; // end namespace
