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
 * File: ./include/flowvr/app/exception.h                          *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP_EXCEPTION_H
#define __FLOWVR_APP_EXCEPTION_H

#include <exception>
#include <string>
#include <sstream>
#include<iostream>
#include <ftl/convert.h>

using namespace ftl;

enum ExceptionType { CUSTOM, NULL_POINTER, PARAM, INCORRECTVALUE };	

namespace flowvr { namespace app {

    class Component; ///< // Forward declaration for cyclic dependency. Class defined into \sa objectInterface.h. 

    /**
     * \exception CustomException
     * \brief This exception give an output like this :
     * Error in function FUNCTIONNAME : ERROR MESSAGE
     */
    class CustomException : public std::exception
    {
        private :
            std::string msg; ///< Message given by the user
        public:
            std::string functionName; 
         
          
            ExceptionType type;
    
            ExceptionType getType() const
            {
                return type;
            }
    
            void setType(ExceptionType type_)
            {
                type =type_;
            }
     
     
            /**
             * \brief the constructor.
             * \param msg the error message
             * \param functionName_ the name of the function where the exception was thrown
             */
            CustomException( const std::string&  msg, const std::string& functionName_ ):functionName(functionName_), type(CUSTOM)
            {
                std::ostringstream oss;
                oss << msg;
                this->msg = oss.str();
            };

            CustomException(const CustomException& e):msg(e.msg),functionName(e.functionName), type(e.type){};

            /**
             * \brief the destructor
             */
            virtual ~CustomException() throw()
            {
            };

            /**
             * \brief this function return the error message
             */
            virtual const char * what() const throw()
            {
                return this->msg.c_str();
            };

    };


    /**
     * \exception NullPointerException
     * \brief To use when unexpected null pointer detected 
     * Error in function FUNCTIONNAME : Null pointer
     */
    class NullPointerException : public CustomException
    {
      
        public :
            /**
             * \brief the constructor.
             * \param type the type of the object (usually a component) the exception is related to
             * \param id the id  of the object (usually a component) the exception is related to
             * \param functionName_ the name of the function throwing the exception  
             */
            NullPointerException(const std::string& type, const std::string& id, const std::string& functionName_) : CustomException("Pointer to " + type + (id != "" ? " named " + id : "") + " is null",functionName_) 
            {
                this->setType(NULL_POINTER);
            };         
    };

    /**
     * \exception SameIdException
     * \brief this exception should be used when duplicate ids or names are found in lists. The message given by this exception is
     * Error in function FUNCTIONNAME : id=IDNAME already exists
     */
    class SameIdException : public CustomException
    {
      
        public :
            /**
             * \brief the constructor.
             * \param type_ type of object where the exception is thrown
             * \param id_ the name of the duplicate id
             * \param typeContainer_ type of container object
             * \param idContainer_ id of the container 
             * \param functionName_ the name of the function where the exception was thrown
             */
            SameIdException( const std::string& type_, const std::string& id_, const std::string& typeContainer_, const std::string& idContainer_, const std::string& functionName_ ) : CustomException( type_ + " id= '" + id_ + "' already exists in " + typeContainer_ + " " + idContainer_, functionName_) {};
    };


    /**
     * \exception IdNotFoundException
     * \brief this exception should be used when looking for an  id or name not found . The message given by this exception is
     * Error in function FUNCTIONNAME : id=IDNAME not found
     */

    class IdNotFoundException : public CustomException
    {
      
        public :
            /**
             * \brief the constructor.
             * \param typeExpected  The type of the id not found
             * \param idExpected  The id not found
             * \param typeContainer The type of the container 
             * \param idContainer The id of the container
             * \param functionName_ The name of the function where the exception was thrown
             */
            IdNotFoundException(const std::string& typeExpected, const std::string& idExpected, const std::string& typeContainer, const std::string& idContainer, const std::string& functionName_) : CustomException(typeContainer + " " + idContainer +": No " + typeExpected + " with id = '" + idExpected+"'", functionName_) {};
    };




    /**
     * \exception XMLAttributeInvalid
     * \brief this exception should be used when an attribute in the XML file is required and the parser doesn't find it. The message given by this exception is
     * 'Error in function FUNCTIONNAME : The attribute NAME doesn't exist in the node NODENAME. Error in (ROW, COLUMN)'
     */
    class XMLAttributeInvalid : public CustomException
    {
        public :
            /**
             * \brief the constructor
             * \param attrName the name of the attribute
             * \param nodeName the name of the node
             * \param row the number of the row in the XML file
             * \param column the number of the column in the XML file
             * \param functionName_ the name of the function where the exception was thrown
             */
            XMLAttributeInvalid( const std::string& attrName, const std::string& nodeName, const unsigned int row, const unsigned int column, const std::string& functionName_ ) : CustomException("The attribute " + attrName + " doesn't exist in the node " + nodeName + ". Error in (" + toString<unsigned int>(row) + ", " + toString<unsigned int>(column) + ")", functionName_) {};
    };

    /**
     * \exception XMLChildIncorrect
     * \brief this exception should be used when node child has an incorrect type. The message given by this exception is 
     * 'Error in function FUNCTIONNAME : The NAMEPARENT's child can't have the type TYPE. Error in (ROW, COLUMN)'
     */
    class XMLChildIncorrect : public CustomException
    {
        public :
            /**
             * \brief the constructor
             * \param parentName the name of the parent
             * \param typeName the type incorrect
             * \param row the number of the row in the XML file
             * \param column the number of the column in the XML file
             * \param functionName_ the name of the function where the exception was thrown
             */
            XMLChildIncorrect( const std::string& parentName, const std::string& typeName, const unsigned int row, const unsigned int column, const std::string& functionName_ ) : CustomException(parentName + " child can't have  type " + typeName + ". Error in (" + toString<unsigned int>(row) + ", " + toString<unsigned int>(column) + ")", functionName_) {};
    };


    /**
     * \exception BadConversion 
     * \brief this exception should be used when a conversion has failed. The message given by the exception is
     * Error in function FUNCTIONNAME : The conversion of the variable VARIABLENAME to TYPEDEST failed.
     */
    class TypeIncompatible : public CustomException
    {
        public :
            /**
             * \brief the constructor
             * \param varName the name of the variable 
             * \param typeDest the conversion would transform the variable to the type typeDest
             * \param functionName_ the name of the function where the exception was thrown
             */
            TypeIncompatible( const std::string& varName, const std::string& typeDest, const std::string functionName_) : CustomException("Conversion of " + varName + " to " + typeDest + " failed.", functionName_) {};
            TypeIncompatible( const std::string& portName, const std::string& componentName, const std::string& typeDest, const std::string functionName_) : CustomException( "The port p=(" + componentName + ", " + portName + ") must be an " + typeDest + " port", functionName_) {};
    };

    /**
     * \exception FileException
     * \brief this exception should be used when an issue occurs when working with file streams. The message given by the exception is
     * Error in function FUNCTIONNAME : MESSAGE FILENAME
     */
    class FileException : public CustomException
    {
        public :
            /**
             * \brief the constructor
             * \param message the message of the error
             * \param fileName the file name where the exception occurs
             * \param functionName_ the name of the function where the exception was thrown
             */
            FileException( const std::string& message, const std::string& fileName, const std::string& functionName_) : CustomException(message + " " + fileName, functionName_) {};
    };

    /**
     * \exception ImpossibleMapping
     * \brief this exception should be used when there are no host where the object could be mapped
     * Error in function FUNCTIONNAME : id=IDNAME cannot be mapped
     */
    class ImpossibleMapping: public CustomException
    {
      
        public :
            /**
             * \brief the constructor.
             * \param id_ the id of the object
             * \param functionName_ the name of the function where the exception was thrown
             */
            ImpossibleMapping( const std::string& id_, const std::string& functionName_ ) : CustomException( "id= '" + id_ + "' cannot be mapped", functionName_) {};
    };

    /**
     * \exception NotSameNumberLinks
     * \brief this exception should be used when a MultipleLinkObjectdo  not have the same number of SIBLING links in in and out ports
     * Error in function FUNCTIONNAME : id=IDNAME not have the same number of SIBLING links ( in = NBIN; out = NBOUT)
     */
    class NotSameNumberLinks: public CustomException
    {
      
        public :
            /**
             * \brief the constructor.
             * \param id_ the id of the object
             * \param nbin_ the number of sibling links in in port
             * \param nbout_ the number of sibling links in out port
             * \param functionName_ the name of the function where the exception was thrown
             */
            NotSameNumberLinks( const std::string& id_, unsigned int nbin_, unsigned int nbout_, const std::string& functionName_ ) : CustomException( "id= '" + id_ + "':  number of SIBLING links differ ( in = " + toString<unsigned int>(nbin_) + "; out = " + toString<unsigned int>(nbout_) +")", functionName_) {};
    };

    /**
     * \exception MoreThanOneCandidate
     * \brief this exception should be used when an object has more than one candidate host 
     * Error in function FUNCTIONNAME : id=IDNAME has more than one candidate host
     */
    class MoreThanOneCandidate: public CustomException
    {
      
        public :
            /**
             * \brief the constructor.
             * \param id_ the id of the object
             * \param functionName_ the name of the function where the exception was thrown
             */
            MoreThanOneCandidate( const std::string& id_, const std::string& functionName_ ) : CustomException( "id= '" + id_ + "':  more than one  host", functionName_) {};
    };


    class CustomLinkException : public CustomException
    {	
        public :
            CustomLinkException(const std::string& msg, const std::string& functionName_) : CustomException("LINK ERROR: " + msg, functionName_) {};
    };

    class LinkTypeException : public CustomLinkException
    {
        public :
            LinkTypeException(const std::string& p1Id, const std::string& p1OwnerId,  const std::string& p2Id, const std::string& p2OwnerId, const std::string& typeLink, const std::string& functionName_) : CustomLinkException("Ports p1=("+ p1OwnerId +", " + p1Id + ") and p2=(" +p2OwnerId+", " + p2Id + ") have incompatible types for a " + typeLink + " link", functionName_) {};

            LinkTypeException(const std::string& pId, const std::string& pOwnerId, const std::string& typeOwnerExpected, const std::string& functionName_) : CustomLinkException("Port p=("+ pOwnerId + ", " + pId +")  needs that type of  " + pOwnerId + " be " + typeOwnerExpected, functionName_) {};
    };

    class LinkCardinalityException : public CustomLinkException
    {
        public :
            LinkCardinalityException(const std::string& pId, const std::string& pOwnerId, const std::string& functionName_) : CustomLinkException("Port p=("+pOwnerId+", " + pId + ") must have at least one link", functionName_) {};

            LinkCardinalityException(const std::string& pId, const std::string& pOwnerId, unsigned int nbConnect, const std::string& p2Id, const std::string& p2OwnerId, unsigned int nbConnect2, const std::string& functionName_) : CustomLinkException("Port p1=("+pOwnerId+", "+pId+") and port p2=("+p2OwnerId+", "+p2Id+") must have the same number of links. ("+toString<unsigned int>(nbConnect)+" != " + toString<unsigned int>(nbConnect2) +")", functionName_) {}
    };

    class NoPrimitiveConnectedException : public CustomLinkException
    {
        public :
            NoPrimitiveConnectedException(const std::string& pId, const std::string& pOwnerId, const std::string& p2Id, const std::string& p2OwnerId, const std::string& functionName_) : CustomLinkException("Port p=(" + pOwnerId + ", " + pId + ") linked to port of composite p=(" + p2OwnerId + ", " + p2Id + ").\n\tExpecting  a  primitive component following this path but could not go further.  Primitive not found", functionName_){};
            NoPrimitiveConnectedException(const std::string& pId, const std::string& pOwnerId, const std::string& functionName_) : CustomLinkException("Port p=(" + pOwnerId + ", " + pId + "): No primitive component linked", functionName_){};
    };

    

    /**
     * \exception IncorrectValueException
     * \brief used in data.h (most methods of this class are deprecated now)
     */
    class IncorrectValueException : public CustomException
    {
            
        public :
            IncorrectValueException( const std::string& parameterName, const std::string& functionName_ ) : CustomException( "Parameter " + parameterName + " is empty", functionName_) 
            {
                setType(INCORRECTVALUE);
            };
            IncorrectValueException( const std::string& parameterName, const std::string& typeContainer, const std::string& idContainer, const std::string& functionName_ ) : CustomException( "In "+typeContainer+ " " + idContainer + ", the parameter " + parameterName + " is empty", functionName_) 
            {
                setType(INCORRECTVALUE);
            };
            
            IncorrectValueException( const std::string& parameterName, const std::string& typeContainer, const std::string& idContainer, const std::string& defaultValue, const std::string& functionName_ ) : CustomException( "In "+typeContainer+ " " + idContainer + ", the parameter " + parameterName + " is empty. Use default value (" + defaultValue + ")", functionName_) 
            {
                setType(INCORRECTVALUE);
            };
    };
    



    class ParameterException : public CustomException
    {
        public :
            ParameterException(const std::string moduleName, const std::string parameterName, const std::string& functionName_): CustomException("Component id='"+moduleName + "'. No value found for  parameter '"+ parameterName+"'", functionName_)
            {
                this->setType(PARAM);
            };
    
            ParameterException(const ParameterException& e) : CustomException(e)
            {}
            ;
    
            virtual ~ParameterException() throw()
            {}
            ;
    
    };

}; };

#endif // __EXCEPTION__APP__
