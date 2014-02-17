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
 * File: ./src/connection.comp.cpp                                 *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/



#include "flowvr/app/core/port.h"
#include "flowvr/app/core/exception.h"
#include "flowvr/app/core/msg.h"
#include "flowvr/app/components/connection.comp.h"


namespace flowvr { namespace app {


    flowvr::xml::DOMElement * ConnectionBase::XMLBuildVertex(const VertexType vt_) const
    {
        std::string v;
        std::string namePort;
        const Port* p = NULL;
      
        if ( vt_ == SOURCE)
            {
                v = "source";
                namePort = "in";
            }
        else
            {
                v = "destination";
                namePort = "out";
            }
        std::string list("");
        int portcount =0;
        std::list<const Port*> primlist;
        getPort(namePort)->getPrimitiveSiblingList(primlist);


        if (primlist.size() >  1 ) 
            {
                for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
                    {
                        list += "\t- (";
                        list += (*it)->getOwner()->getFullId();
                        list += ", " + (*it)->getId() + ")\n";
                    }
                throw CustomLinkException("Port of Connection p=("+ getFullId() + ", " + namePort +"): linked to multiple primitive components  (one expected). These primitives are:\n"+list, __FUNCTION_NAME__);
            }
        else if ( primlist.empty() )
            {
                throw  NoPrimitiveConnectedException(namePort,getFullId(),__FUNCTION_NAME__);
            }
        else 
            {
                p = * (primlist.begin()); 
            }


        // remove useless connection if 2 connections are connected directly.
        // If connections are of different types (FULL and STAMPS) throw exception

        const ConnectionBase * const compconn = dynamic_cast<const ConnectionBase* const>(p->getOwner());
        
        if (compconn  &&  isFull != compconn->isFull )  
            {
                
                throw CustomLinkException(static_cast<std::string const>(isFull?"FULL":"STAMPS")+" connection  port p1=("+ getFullId() +", " + namePort+ ")  directly linked  to the "+(!isFull?"FULL":"STAMPS")+" connection  port p2=(" + p->getOwner()->getFullId()+", " + p->getId() + "): forbidden (cannot directly connect a  FULL and STAMPS connection)", __FUNCTION_NAME__);
            }


        if (vt_ == DEST && compconn)
            {
                // Connection collapse algorithm 1/ => don't create a description of this connection
                return NULL;
            }
        if (vt_ == SOURCE && compconn)
            {
                // Connection collapse algorithm 2/ => use the previous connection in port as in port...
                const Connection* const previous = (const Connection* const) p->getOwner();
                return (previous->XMLBuildVertex(SOURCE));
            }

        // Check connection is connected to compatible FULL/STAMPS ports

        // Chek if FULL  connection linked to STAMPS output  port
        if (vt_ == SOURCE && isFull && p->getMsgType() == STAMPS )
            {
                throw CustomLinkException("FULL connection input port p1=("+ getFullId() +", " + namePort + ")  linked  to STAMPS output port p2=(" + p->getOwner()->getFullId()+", " + p->getId() + "): forbidden (only STAMPS connections can be linked to a STAMPS output port)", __FUNCTION_NAME__);
            }
        // Chek if FULL  connection linked to STAMPS input port
        else if (vt_ == DEST && isFull && p->getMsgType() == STAMPS )
            {
                throw CustomLinkException("FULL connection output port p1=("+ getFullId() +", " + namePort + ")  linked  to STAMPS input  port p2=(" + p->getOwner()->getFullId()+", " + p->getId() + "): forbidden (only STAMPS connections can be linked to a STAMPS input  port)", __FUNCTION_NAME__);
            }
        // Chek if STAMPS  connection linked to FULL  input port
        else if (vt_ == DEST && !isFull && p->getMsgType() == FULL )
            {
                throw CustomLinkException("STAMPS connection output port p1=("+ getFullId() +", " + namePort + ")  linked  to FULL  input  port p2=(" + p->getOwner()->getFullId()+", " + p->getId() + "): forbidden (only FULL connections can be linked to a FULL input  port)", __FUNCTION_NAME__);
            }



        if (!dynamic_cast<const Primitive* const>(p->getOwner()))
            throw LinkTypeException(p->getId(), p->getOwner()->getFullId(), "primitive", __FUNCTION_NAME__);

        if ( !isFull) v = v+"stamps";

        flowvr::xml::DOMElement * vertex = new flowvr::xml::DOMElement(v);

        flowvr::xml::DOMElement * port=new flowvr::xml::DOMElement(((const Primitive* const)(p->getOwner()))->getType()+"id");
        port->SetAttribute("id",p->getOwner()->getFullId()); 
        port->SetAttribute("port",p->getId());
        vertex->LinkEndChild(port);

        return vertex;
    };
    

    flowvr::xml::DOMElement *  ConnectionBase::XMLBuild() const
    {

        // Check AUTO/SAMEAS (@see ConnectionAuto ) resolved now
        if ( getPort("out")->getMsgType() == AUTO)
            {
                throw CustomException("Port of connection p=("+getFullId()+", "+getPort("out")->getId()+"): has the AUTO  message type. Should be resolved to FULL or STAMPS now.",__FUNCTION_NAME__);
            }
        if ( getPort("in")->getMsgType() == SAMEAS)
            {
                throw CustomException("Port of connection p=("+getFullId()+", "+getPort("in")->getId()+"): has the SAMEAS  message type. Should be resolved to FULL or STAMPS now.",__FUNCTION_NAME__);
            }

        flowvr::xml::DOMElement *  e = new flowvr::xml::DOMElement(getType()); 
        e->SetAttribute("id",getFullId());
      
        // connection collapse algorithm :
        // 1/ if out is linked to a connection, then don't create XML description of the connection
        // 2/ if in is linked to a connection, go along the previous connection to find sibling link (see XMLBuildVertex)
      
        //Source
        flowvr::xml::DOMElement * src= XMLBuildVertex(SOURCE);
        e->LinkEndChild(src);
      
        //destination
        flowvr::xml::DOMElement * dest= XMLBuildVertex(DEST);
        if (dest == NULL)
            {
                // Don't create a description (connection collapse algorithm)
                delete e;
                return NULL;
            }
        e->LinkEndChild(dest);
      
        return e;
    };
    
}; }; // end namespace
