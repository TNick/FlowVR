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
 * File: ./include/flowvr/app/components/filter.comp.h           *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTER_H
#define __FLOWVR_APP__FILTER_H

#include "flowvr/app/core/component.h"

/**
 * \file filter.comp.h
 * \brief implements base class for  all filters
 */

namespace flowvr { namespace app {

     
    /**
     * \class Filter
     * \brief this class is the parent of all filters
     */
    class Filter : public Primitive
    {
        private:
            FilterClass filterclass; ///<  the filter class plugin
      
        public:


            /**
             * \brief constructor
             * \param id_ the filter id
             * \param class_ the plugin class 
             */
            Filter(const std::string& id_, const std::string& class_) : Primitive(id_,"filter"), filterclass(class_){};

      
            /**
             * \brief copy constructor
             */
            Filter(const Filter& m) : Primitive(m), filterclass(m.filterclass) {};


            TypeComponent getTypeComponent() const { return OBJECT; };
      
            /**
             * \brief virtual copy constructor
             */

            virtual Component* create() const 
            { 
                return new Filter(getId(), getType());
            };

            /**
             * \brief Convert object to XML 
             * \return an XML node 
             */
            virtual xml::DOMElement * XMLBuild() const;


            virtual xml::DOMElement * HierarchyXMLBuild() const;


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




            bool isPrimitive() const {return true;}
    };


};}; //end namespace


#endif // __FLOWVR_APP__FILTER_H
