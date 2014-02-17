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
 * File: ./include/flowvr/app/components/sync.comp.h             *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__SYNC_H
#define __FLOWVR_APP__SYNC_H

#include "flowvr/app/core/component.h"
#include "flowvr/app/core/data.h"

/**
 * \file sync.comp.h
 * \brief implements base class for  all synchronizers
 */

namespace flowvr { namespace app {

     
    /**
     * \class Sync
     * \brief this class is the parent of all synchronizers
     */
    class Sync : public Primitive
    {
        private:
            SyncClass syncclass; ///<  the sync  class plugin
      
        public:


            /**
             * \brief constructor
             * \param id_ the filter id
             * \param class_ the plugin class 
             */
            Sync(const std::string& id_, const std::string& class_) : Primitive(id_,"synchronizer"), syncclass(class_){};

      
            /**
             * \brief copy constructor
             */
            Sync(const Sync& m) : Primitive(m), syncclass(m.syncclass)
            {
            	// all synchronizers have an advance parameter with a default value set to 1:
            	// A first message is sent before receiving any incoming message (starting token).
            	addParameter<int>("advance",1);
            };

            /**
             * \brief destructor
             */
            virtual ~Sync() {};

            virtual Component* create() const 
            { 
                return new Sync(getId(), syncclass.getValue());
            };
      
            TypeComponent getTypeComponent() const { return OBJECT; };

            bool isPrimitive() const {return true;}

            /**
             * \brief Convert object to XML 
             * \return an XML node 
             */
            virtual flowvr::xml::DOMElement * XMLBuild() const;


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

};}; //end namespace


#endif // __FLOWVR_APP__SYNC_H
