/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
 * COPYRIGHT (C) 2003-2011                by                       *
 * Laboratoire d'Informatique Fondamentale d'Orleans               *
 * (EA 4022) ALL RIGHTS RESERVED.                                  *
 *                                                                 *
 * This source is covered by the GNU LGPL, please refer to the     *
 * COPYING-LIB file for further information.                       *
 *                                                                 *
 *-----------------------------------------------------------------*
 *                                                                 *
 *  Original Contributors:                                         *
 *    Sebastien Limet                                              *
 *    Sophie Robert                                                *
 * 	  Yoann Kernoa												   *
 *                                                                 *
 *******************************************************************
 *                                                                 *
 * File: ./include/flowvr/app/components/filteritsimple.comp.h     *
 *                                                                 *
 * Contacts:  Yoann Kernoa  <yoann.kernoa@etu.univ-orleans.fr      *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTERITSIMPLE_H
#define __FLOWVR_APP__FILTERITSIMPLE_H

#include "flowvr/app/core/data.h"
#include "flowvr/app/components/filter.comp.h"


      
/**
 * \file filteritsimple.comp.h
 */

namespace flowvr { namespace app {


    /**
     * \class FilterItSimple
     * \brief Forward on its  output  the messages received on its input having the same it numbers than the ones received on the order port. Discard other messages (filter usually used with a synchronizer).
     **/

    class FilterItSimple : public Filter
    {
        public :
      
            FilterItSimple(const std::string& id_) : Filter(id_, "FilterItSimple") 
            {
	
                setInfo("Forward on its  output  the messages received on its input having the same it numbers than the ones received on the order port. Discard other messages  (filter usually used with a synchronizer).");
	
                addPort("in", INPUT,FULL);
                addPort("order", INPUT, STAMPS); // stamps
                addPort("out", OUTPUT,FULL);
            };
      
            virtual Component* create() const {return new FilterItSimple(getId());};  
    };
    
};}; // end namespace 

#endif // __FLOWVR_APP__FILTERITSIMPLE_H


