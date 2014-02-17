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
 * File: ./include/flowvr/app/data.h                               *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP_DATA_H
#define __FLOWVR_APP_DATA_H

#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <flowvr/xml.h>
#include "flowvr/app/core/exception.h"
#include "flowvr/app/core/msg.h"
#include "flowvr/app/core/utils.h"


namespace flowvr { namespace app {
    /** \file data.h
     *  \brief Define base classes for handling various data used by flowvr objects and metaobjects (parameters,  trace, ....)
     **/


    /**
     * \class Id
     * \brief this class is used to represent the id of a component (Port, Object, etc ...)
     */
    class Id
    {
        private:
            std::string id; ///< std::string of the id
            std::string delimiter; ///<delimiter between prefixes
      
            /**
             * \brief default constructor
             * \note this constructor is private because you shouldn't use it
             */
            Id() {};
      
        public:
            /**
             * \brief constructor with the name of the id
             * \throw IncorrectValueException if id is empty or equals ""
             */
            Id(const std::string& id_, const std::string& delimiter_ = ".") throw(IncorrectValueException);
      
            /**
             * \brief copy constructor
             */
            Id(const Id& id_): id(id_.id), delimiter(id_.delimiter) {};
      
            /**
             * \brief geter of the id
             * \return a std::string with the id
             */
            const std::string& getId() const { return  id; };
      
            /**
             * \brief set the id
             * \param id_ a std::string representing the id
             * \throw IncorrectValueException if id_ equals "" or empty
             */
            void setId(const std::string& id_) throw(IncorrectValueException);
      
            /**
             * \brief add a prefix to the id
             * \param prefix_ a std::string equals to the prefix
             * \throw IncorrectValueException if prefix_ equals "" or empty
             */
            void addPrefix(const std::string& prefix_) throw(IncorrectValueException);
      
    };
    
	
    /**
     * \enum ValStatus
     * \brief this enum represents the status of a parameter
     * <ul>
     *    <li> NO=0 => no value (value == NULL or value == "") </li>
     *    <li> REQ=1 => mandatory value ( value != NULL and value != "") </li>
     *    <li> OPT=2 => optional  value (value can be empty std::string "" or NULL) </li>
     * </ul>
     */
     
    // on s'ajoute un petit ValStatus pour la route ?  
    // genre ASK=3 : Ask for value at runtime 
     
    enum ValStatus {NO=0,REQ=1,OPT=2};


    /**
     * \class Data
     * \brief A data is a value associated to a name and a list of child data. User can declare if a data is optional or not. Data are used to generate XML.     
     */
    class Data
    {
        private :
            std::list<Data*> children; ///< List of children
            std::string name; ///< Name of the data
            std::string value; ///< Value of the data
            ValStatus valstatus; ///< State of the data

            /**
             * \brief default constructor
             * \note this constructor has no sense. So it's private and you shouldn't use it
             */
            Data() : children(),  name(""),  value(""), valstatus(NO) {};

        public :


            /**
             * \brief a constructor with the name of the data and it's status
             * \param name_ the name of the data
             * \param s_ the state of the data
             */
            Data( const std::string& name_, const ValStatus s_) :  children(), name(name_), value(""),valstatus(s_) {};

            /**
             * \brief a constructor with the name, the value of the status of a data
             * \param n_ the name of the data
             * \param v_ the value of the data
             * \param s_ the state of the data
             */
            Data( const std::string& n_, const std::string& v_, const ValStatus s_) :  children() ,  name(n_), value(v_),valstatus(s_) {};

            /**
             * \brief copy constructor
             */
            Data( const Data& p);


            /**
             * \brief virtual copy constructor
             */
            virtual Data* clone() const  {  return new Data(*this); };


            /**
             * \brief Destructor
             */

            virtual ~Data(){
                std::for_each(children.begin(), children.end(), DeletePtr<Data>());
            };

            /**
             * \brief get the name of the data
             * \return the name of the data
             */
            const std::string& getName() const { return name; };

            /**
             * \brief set the name of the data
             * \param n the new name
             */
            void setName( const std::string& n ) { name = n; };

            /**
             * \brief get the value status
             * \return value status
             */
            ValStatus getValStatus() const { return valstatus; };

            /**
             * \brief set the status
             * \param s_ the new status
             */
            void setValStatus(const ValStatus s_ ) { valstatus = s_; };

            /**
             * \brief set the value of the data
             * \param s_ the new value
             */
            void setValue(const std::string& s_) { value = s_; };

            /**
             * \brief get the Value of the data
             * \return the value of the data
             */
            const std::string& getValue() const { return value; };


            /**
             * \brief test if value empty or not 
             * \return false if value==NULL or "", true otherwise
             */
            bool isValueSet() const 
            { 
                return !value.empty() ; 
            };


            /**
             * \brief get an iterator on the children
             * \return the iterator on the first child
             */
            std::list<Data*>::const_iterator getChildrenBegin() const { return children.begin(); };
            /**
             * \brief get an iterator on the children
             * \return the iterator on the last child
             */
            std::list<Data*>::const_iterator getChildrenEnd() const { return children.end(); };


            /**
             * \brief number of children
             * \return the number of children
             */
            unsigned int size() { return children.size(); };

            /**
             * \brief get first child with  name v
             * \param name_ the name of the child
             * \return NULL if no child with name_ found
             * \throw IncorrectValueException if name_  empty
             */
            Data* getChild( const std::string& name_)     throw(IncorrectValueException);


            /**
             * \brief Look for first descendant (recursive search includes self)
             * \param name_ the  name of the data to look for 
             * \return NULL if not found otherwise pointer to descendant
             * \throw IncorrectValueException if name_  empty
             */
            Data*  getDescendant( const std::string& name_)     throw(IncorrectValueException);


            /**
             * \brief add a new data in child
             * \param data_ the new data 
             * \note a copy of \a data_ is made 
             * \throw IncorrectValueException if data_ name empty 
             * \throw SameIdException if child with same name already exists
             */
            void add(const Data& data_) throw (IncorrectValueException,SameIdException);



            /**
             * \brief add a new simple data with name and value in child
             * \param id_ the new data id
             * \param value_ the new data value
             * \param s_ the status of the value
             * \throw IncorrectValueException if id_ name empty
             * \throw SameIdException if child with same id name already exists
             */
            void add(const std::string& id_, const std::string& value_, const ValStatus s_) throw (IncorrectValueException,SameIdException);

            /**
             * \brief Convert data to XML
             * \note  Only work for Data without value (valstatus==NO)
             * \note  Recursively build xml for all children
             * \return a pointer to a XML DOMElement
             * \throw CustomException  if Data can have a value (valstatus== REQ or OPT) 
             */
            virtual flowvr::xml::DOMElement* XMLBuild() const throw (IncorrectValueException);
    };



    /**
     * \class Info
     * \brief class for storing an info  (a std::string) 
     */   
    class Info: public Data
    {
        public:
      
            /**
             * \brief Constructor
             */   
            Info() : Data("info", OPT){};
      
            /**
             * \brief Constructor
             * \param info the info std::string 
             */   
            Info(const std::string& info): Data("info", info, OPT){};
      
            /**
             * \brief Constructor
             * \param info the info char pointer 
             */   
            Info(char * info): Data("info", info, OPT){};

            /**
             * \brief Copy  Constructor
             */   
            Info(const Info& p) : Data("info", p.getValue(), OPT) {};

            /**
             * \brief Virtual copy  constructor
             */   
            virtual Info* clone() const { return new Info(*this); }; 
      
    };

    /**
     * \class InternalData
     * \brief for storing data that do not require to be printed in the XML 
     * \warning Use carefully.  It is usually better to expose parameters in XML to ease debugging.
     */
    class InternalData : public Data
    {
        public :
            InternalData(const std::string& key) : Data(key, OPT) {};

            InternalData(const std::string& key, const std::string& value) : Data(key, value, OPT) {};

            InternalData(const InternalData& p) : Data(p.getName(), p.getValue(), OPT) {};

            virtual InternalData* clone() const { return new InternalData(*this); };


            flowvr::xml::DOMElement* XMLBuild() const throw (IncorrectValueException){ return NULL; };
    };


    /**
     * \class Plugin
     * \brief base class for plugin parameter of  synchronizers and filters. Should not be used directly
     */
    class Plugin: public Data
    {
        public:

            /**
             * \brief constructor with the object type  the plugin is related to  ("filter" for instance)
             * \param name_  the object type name
             */
            Plugin( const std::string& name_ ) : Data(name_,REQ)  {};


            /**
             * \brief constructor with the object type the plugin is related to  ("filter" for instance) and the plugin class name
             * \param name_  the object type name
             * \param class_  the plugin class name
             */
            Plugin( const std::string& name_, const std::string& class_ ) : Data(name_,class_,REQ) {};

            /**
             * \brief copy constructor
             */
            Plugin(const Plugin& p_) : Data(p_) {};


            /**
             * \brief virtual copy constructor
             */
            virtual Plugin* clone() const {  return new Plugin(*this); };


            /**
             * \brief Destructor
             */
            virtual ~Plugin(){};


            /**
             * \brief Convert to XML
             * \return a pointer to a XML DOMElement
             */

            virtual flowvr::xml::DOMElement*  XMLBuild() const throw (IncorrectValueException);


            virtual flowvr::xml::DOMElement* HierarchyXMLBuild() const throw (IncorrectValueException);

    };


    /**
     * \class FilterClass
     * \brief class for plugin parameter of filters. 
     */   
    class FilterClass: public Plugin 
    {
        public:

            /**
             * \brief constructor with the name of the plugin class
             * \param class_  the plugin class name
             */
            FilterClass( const std::string& class_ ) : Plugin("filterclass",class_) {};

            /**
             * \brief copy constructor
             */
            FilterClass(const FilterClass& p_) : Plugin(p_) {};

            /**
             * \brief virtual copy constructor
             */
            virtual FilterClass* clone() const {  return new FilterClass(*this); };

            /**
             * \brief Destructor
             */
            virtual ~FilterClass(){};


    };


    /**
     * \class SyncClass
     * \brief class for plugin parameter of synchronizers. 
     */   
    class SyncClass: public Plugin 
    {
        public:

            /**
             * \brief constructor with the name of the plugin class
             * \param class_  the plugin class name
             */
            SyncClass( const std::string& class_ ) : Plugin("synchronizerclass",class_) {};

            /**
             * \brief copy constructor
             */
            SyncClass(const SyncClass& p_) : Plugin(p_) {};

            /**
             * \brief Destructor
             */
            virtual ~SyncClass(){};

            /**
             * \brief virtual copy constructor
             */
            virtual SyncClass* clone() const {  return new SyncClass(*this); };
    };


    /**
     * \class SimpleDataType
     * \brief Base class for data types (used for traces for instance)
     */
    class SimpleDataType:  public Data
    {
        public: 

            /**
             * \brief constructor with the type name  ("int" for instance)
             * \param t_ type name
             */
            SimpleDataType(const std::string& t_): Data(t_,NO){};

            /**
             * \brief copy constructor
             */
            SimpleDataType(const SimpleDataType& t_): Data(t_){};

            /**
             * \brief virtual copy constructor
             */
            virtual SimpleDataType* clone() const {  return new SimpleDataType(*this); };


            /**
             * \brief Destructor
             */
            virtual ~SimpleDataType(){};


            /**
             * \brief Convert to XML
             * \return a pointer to a XML DOMElement
             */
            flowvr::xml::DOMElement*  XMLBuild() const throw (IncorrectValueException);

    };


    /**
     * \class IntDataType
     * \brief  class for int data type 
     */
    class IntDataType: public SimpleDataType
    {
        public:
            /**
             * \brief constructor 
             */
            IntDataType(): SimpleDataType("int"){};

            /**
             * \brief copy constructor
             */
            IntDataType( const IntDataType& i_): SimpleDataType(i_){};


            /**
             * \brief Destructor
             */
            virtual ~IntDataType(){};



            /**
             * \brief virtual copy constructor
             */
            virtual IntDataType* clone() const {  return new IntDataType(*this); };



    };




    /**
     * \class FloatDataType
     * \brief  class for float data type 
     */
    class FloatDataType: public SimpleDataType
    {
        public:
            /**
             * \brief constructor 
             */
            FloatDataType(): SimpleDataType("int"){};

            /**
             * \brief copy constructor
             */
            FloatDataType( const FloatDataType& i_): SimpleDataType(i_){};


            /**
             * \brief Destructor
             */
            virtual ~FloatDataType(){};


            /**
             * \brief virtual copy constructor
             */
            virtual FloatDataType* clone() const {  return new FloatDataType(*this); };

    };


    /**
     * \class std::stringDataType
     * \brief  class for std::string  data type 
     */
    class StringDataType: public SimpleDataType
    {
        public:
            /**
             * \brief constructor 
             */
            StringDataType(): SimpleDataType("string"){};

            /**
             * \brief copy constructor
             */
            StringDataType( const StringDataType& i_): SimpleDataType(i_){};


            /**
             * \brief virtual copy constructor
             */
            virtual StringDataType* clone() const {  return new StringDataType(*this); };



            /**
             * \brief Destructor
             */
            virtual ~StringDataType(){};


    };




    /**
     * \class BinaryDataType
     * \brief  class for binary  data type 
     */
    class BinaryDataType: public Data
    {
        public:
            /**
             * \brief constructor with size
             * \param size_  binary data size
             */
            BinaryDataType(int size_): Data("binary",toString<int>(size_),REQ){};

            /**
             * \brief copy constructor
             */
            BinaryDataType( const BinaryDataType& i_): Data(i_){};


            /**
             * \brief virtual copy constructor
             */
            virtual BinaryDataType* clone() const {  return new BinaryDataType(*this); };


            /**
             * \brief Destructor
             */
            virtual ~BinaryDataType(){};

            /**
             * \brief Convert data to XML
             * \return a pointer to a XML DOMElement
             */
            virtual flowvr::xml::DOMElement*  XMLBuild() const throw (IncorrectValueException);


    };


    /**
     * \class ArrayDataType
     * \brief  class for array  data type 
     */
    class ArrayDataType: public Data
    {
        public:

            /**
             * \brief constructor with the array size and base type
             * \param size_ array size
             * \param type_  array element type
             */      
            ArrayDataType(int size_, const SimpleDataType& type_): Data("array",toString<int>(size_),REQ){ add(type_); };

            /**
             * \brief constructor with the array size and base type
             * \param size_ array size
             * \param type_  array element type
             */      
            ArrayDataType(int size_, const BinaryDataType& type_): Data("array",toString<int>(size_),REQ) { add(type_); };

            /**
             * \brief copy constructor
             */
            ArrayDataType(const ArrayDataType& p_): Data(p_){};


            /**
             * \brief virtual copy constructor
             */
            virtual ArrayDataType* clone() const  {  return new ArrayDataType(*this); };

            /**
             * \brief Destructor
             */
            virtual ~ArrayDataType(){};


            /**
             * \brief Convert to XML
             * \return a pointer to a XML DOMElement
             */
            virtual flowvr::xml::DOMElement*  XMLBuild() const throw (IncorrectValueException);

    };


    /**
     * \class Trace
     * \brief class for trace events 
     */
    class Trace: public  Data
    {
        public:

            /**
             * \brief constructor with the trace name and int type
             * \param id_ trace name
             */
            Trace(const std::string& id_);


            /**
             * \brief constructor with the trace name and type
             * \param id_ trace name
             * \param t_ trace type
             */
            Trace(const std::string& id_, const SimpleDataType& t_) : Data(id_,NO) { Data::add(t_); };


            /**
             * \brief constructor with the trace name and type
             * \param id_ trace name
             * \param t_ trace type
             */
            Trace(const std::string& id_, const ArrayDataType& t_) :  Data(id_,NO) { Data::add(t_); };

            /**
             * \brief constructor with the trace name and type
             * \param id_ trace name
             * \param t_ trace type
             */
            Trace(const std::string& id_, const BinaryDataType& t_) : Data(id_,NO) { Data::add(t_); };

            /**
             * \brief copy constructor
             */
            Trace(const Trace& t_) :  Data(t_){};


            /**
             * \brief virtual copy constructor
             */
            virtual Trace* clone() const  {  return new Trace(*this); };


            /**
             * \brief Destructor
             */
            virtual ~Trace(){};


            /**
             * \brief Convert trace to XML
             * \return a pointer to a XML DOMElement
             */

            virtual flowvr::xml::DOMElement*  XMLBuild() const throw (IncorrectValueException);

    };


//    /**
//     * \class TraceList
//     * \brief class for list of trace events
//     */
//    class TraceList: public  Data
//    {
//        public:
//
//            /**
//             * \brief constructor
//             */
//            TraceList() : Data("tracelist",NO) {};
//
//            /**
//             * \brief copy constructor
//             */
//            TraceList(const TraceList& t_) : Data(t_){};
//
//            /**
//             * \brief virtual copy constructor
//             */
//            virtual TraceList* clone() const {  return new TraceList(*this); };
//
//            /**
//             * \brief Destructor
//             */
//            virtual ~TraceList(){};
//
//
//            /**
//             * \brief add a Trace giving its id (int type).
//             *  \param id_ trace id
//             *  \throw IncorrectValueException if id_  empty
//             *  \throw SameIdException if trace with same id already in  the list
//             */
//            void add(const std::string& id_)  throw (IncorrectValueException);
//
//
//            /**
//             * \brief add a new trace
//             * \param trace_ the new trace
//             * \note a copy of \a trace_ is made
//             * \throw IncorrectValueException if trace_ name  empty
//             * \throw SameIdException if child with same name already exists
//             */
//            void add(const Trace& trace_) throw (IncorrectValueException,SameIdException) { Data::add(trace_);};
//
//
//    };



    /**
     * \class ParamList
     * \brief class for list of parameters
     */   
    class ParamList: public Data
    {
        public:

            /**
             * \brief Constructor
             */   
            ParamList() : Data("parameters",NO) {};

            /**
             * \brief copy constructor
             */
            ParamList(const ParamList& t_) : Data(t_){};

            /**
             * \brief virtual copy constructor
             */
            virtual ParamList* clone() const {  return new ParamList(*this); };

            /**
             * \brief Destructor
             */
            virtual ~ParamList(){};

    };




}; };
#endif //__DATA__
