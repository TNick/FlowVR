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
 * File: ./include/flowvr/app/components/routingnode.comp.h      *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__ROUTINGNODE_H
#define __FLOWVR_APP__ROUTINGNODE_H

#include "flowvr/app/core/component.h"

/**
 * \file routingnode.comp.h
 * \brief implements RoutingNode and RoutingNodeStamps classes 
 */

namespace flowvr { namespace app {



    /**
     * \class RoutingNode 
     */
    class RoutingNode : public Primitive
    {

        public:
            /**
             * \brief constructor
             * \note only have an in and out port
             */
            RoutingNode(const std::string& id_) : Primitive(id_,"routingnode") 
            {
                setInfo("A routing node is a low level  primitive  component. It just takes messages on 'in' port (must be  full  messages)  and sends them on 'out' port. Should be used with great care because it can lead to deadlocks if not properly used (related to an internal limitation of routing node implementation). Unless you know what you are doing use instead FilterRoutingNode that provides the same functionality without risk of deadlock");
                addPort("in", INPUT);
                addPort("out", OUTPUT);

				setHostListfromCsv();
            };


            /**
             * \brief virtual copy constructor
             */
            Component* create() const { return new RoutingNode(getId());  }; 

            bool isPrimitive() const {return true;}


            /**
             * \brief destructor
             */
            ~RoutingNode() {};

            TypeComponent getTypeComponent() const { return OBJECT; };

            /**
             * \brief get the XML structure of a routingnode
             * \return a pointer to the XML description 
             */
            virtual xml::DOMElement* XMLBuild() const
            {
                return Primitive::XMLBuild();
            };

            /**
             * \brief Set host list from csv data. Clear existing hostlist only if csv provides a new host.
             */
    		virtual void setHostListfromCsv();


            /**
             * \brief host mapping  (controller)
             * Call the secondary controller setHosts()
             *  Ensure that if a host mapping is given through the CSV file, it  overwrites any previously set host (CSV setting has the highest priority for Primitives)
             *
             */
            void setHostsMainController();

    };


    /**
     * \class RoutingNodeStamps 
     */
    class RoutingNodeStamps : public Primitive
    {
        public:
            /**
             * \brief constructor
             * \note Only have a "in" and "out" port.
             */
            RoutingNodeStamps(const std::string& id_) : Primitive(id_,"routingnodestamps") 
            {
                setInfo("A routing node is a low level  primitive  component. It just takes messages on 'in' port (must be  stamp  messages)  and sends them on 'out' port. Should be used with great care because it can lead to deadlocks if not properly used (related to an internal limitation of routing node implementation). Unless you know what you are doing use instead FilterRoutingNode that provides the same functionality without risk of deadlock");
                addPort("in", INPUT, STAMPS);
                addPort("out", OUTPUT, STAMPS);
            };

            /**
             * \brief virtual copy constructor
             */
            Component* create() const { return new RoutingNodeStamps(getId());  }; 


            bool isPrimitive() const {return true;}


            /**1
             * \brief destructor
             */
            ~RoutingNodeStamps() {};

            TypeComponent getTypeComponent() const { return OBJECT; };

            /**
             * \brief Set host list from csv data. Clear existing hostlist only if csv provides a new host.
             */
            virtual void setHostListfromCsv();


            /**
             * \brief host mapping  (controller)
             * Call the secondary controller setHosts()
             *  Ensure that if a host mapping is given through the CSV file, it  overwrites any previously set host (CSV setting has the highest priority for Primitives)
             *
             */
            void setHostsMainController();
    };


}; }; // end namespace

#endif //__FLOWVR_APP__ROUTINGNODE_H

