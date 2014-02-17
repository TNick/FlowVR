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
 * File: ./include/flowvr/app/components/connection.comp.h       *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__CONNECTION_H
#define __FLOWVR_APP__CONNECTION_H

#include "flowvr/app/core/component.h"

/**
 * \file connection.comp.h
 * \brief implements Connection and ConnectionStamps
 */

namespace flowvr { namespace app {


    /**
     * \class ConnectionBase
     * \brief This class define a base connection that can be FULL or STAMPS. 
     * It has two specializations: 
     * <ul>
     *    <li> Connection  for full connections
     *    <li> ConnectionStamps for stamps connections
     * </ul>
     *
     */
    class ConnectionBase : public Primitive
    {
      
        protected:
            bool isFull;             ///< Type (FULL =true/STAMPS=false) of the connection 
     
        public:
            /**
             * \brief constructor
             * \note a connection have only an in and out port
             */
            //            ConnectionBase(const std::string& id_,  const bool isFull_) : Primitive(id_, (isFull_ ? "connection" :"connectionstamps")), isFull(isFull_)
            ConnectionBase(const std::string& id_,  const std::string type_) : Primitive(id_,type_), isFull(true)
            {
            };
      
      
            /**
             * \brief destructor
             */
            ~ConnectionBase() {};
      
      
            TypeComponent getTypeComponent() const { return COMMUNICATION; };

            bool isPrimitive() const { return true; }
      
        private:
            /**
             * \enum VertexType
             * \brief define if vertex is Source or Destinaton
             */
            enum VertexType{SOURCE,DEST};
      
            /**
             * \brief Convert connection vertex (source or destination)  to XML 
             *
             * @pre expect SAMEAS/AUTO ports (@see ConnectionAuto)  be resolved beofre calling this method 
             *
             * \return an  XML node
             */
            xml::DOMElement * XMLBuildVertex(const VertexType vt_) const; 
      
        public:
      
            /**
             * \brief create XML structure of a moduled
             * \return pointer to the XML structure
             */
            xml::DOMElement* XMLBuild() const;
      
            /**
             * \brief Clone 
             */
            virtual Component* create() const { return new ConnectionBase(getId(),getType()); };
      
    };

    /**
     * class Connection  for FULL connection 
     */
    class Connection: public ConnectionBase
    {
        public: 
      
            /**
             * \brief constructor for a connection 
             * \param id_ id of the connection
             */
            Connection(const std::string& id_): ConnectionBase(id_,"connection")
            {
                setInfo("Send messages from a port to an another");
                addPort("in", INPUT,FULL);
                addPort("out", OUTPUT,FULL);
                isFull = true; 
            };
      
   
            virtual Component* create() const { return new Connection(getId());};



            /**
             * \brief Resolve AUTO/SAMEAS ports (controller)
             *
             *  Nothing to do for connection
             */
            virtual void resolveMsgTypes(){};

    };


    /**
     * class Connection  for STAMPS connection 
     */
    class ConnectionStamps: public ConnectionBase
    {
        public: 

            /**
             * \brief constructor for a connection 
             * \param id_ id of the connection
             */
            ConnectionStamps(const std::string& id_): ConnectionBase(id_,"connectionstamps")
            {
                setInfo("Send just stamps from a port to an another");
                addPort("in", INPUT,STAMPS);
                addPort("out", OUTPUT,STAMPS);
                isFull=false;
            };
      
  		
            virtual Component* create() const { return new ConnectionStamps(getId());};

            /**
             * \brief Resolve AUTO/SAMEAS ports (controller)
             *
             *  Nothing to do for connectionstamps
             */
            virtual void resolveMsgTypes(){};
    };



    /**
     * class Connection  for AUTO  connection 
     * @brief FULL or STAMPS state defined during the  @see setConnections() traverse
     */
    class ConnectionAuto: public ConnectionBase
    {
        public: 

            /**
             * \brief constructor for a connection 
             * \param id_ id of the connection
             */
            ConnectionAuto(const std::string& id_): ConnectionBase(id_,"auto")
            {
                setInfo("Automatic type: FULL/STAMPS status resolved during resolveMsgTypes traverse");
                addPort("in", INPUT,SAMEAS,"out");
                addPort("out", OUTPUT,AUTO);
            };
      
  		
            virtual Component* create() const { return new ConnectionAuto(getId());};

           
            /**
             * \brief Resolve AUTO/SAMEAS ports (controller)
             *
             * This is a controller called during a fixed point traverse. This controller has only  implementations for primitive components.
             *
             */
            virtual void resolveMsgTypes() 
            {
                Primitive::resolveMsgTypes();
                // AUTO/SAMEAS resolved now
                // set the type of the connection now that we know it. 
                switch (  getPort("out")->getMsgType() ) 
                    {
                    case FULL:
                        setType("connection");
                        isFull = true;
                        break;
                    case STAMPS:
                        setType("connectionstamps");
                        isFull = false;
                        break;
                    default:
                        throw CustomException("ConnectionAuto "+getFullId()+": unable to define if FULL or STAMPS connection",__FUNCTION_NAME__);
                    };
            };
    };


}; }; // end namespace

#endif //__FLOWVR_APP__CONNECTION_H

