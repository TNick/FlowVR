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
 * File: include/flowvr/app/genclass.h                              *
 *                                                                 *
 * Contacts:                                                       *
 *  12/10/2003 Jeremie Allard <Jeremie.Allard@imag.fr>             *
 *                                                                 *
 ******************************************************************/
#ifndef __FLOWVR_APP_GENCLASS_H
#define __FLOWVR_APP_GENCLASS_H

#include "flowvr/app/core/class.h"
#include "flowvr/app/core/component.h"

/**
 * \file genclass.h 
 * \brief class for dynamic component loading
 */ 

namespace flowvr
{
  namespace app
  {



    /** \def GENCLASS
     * \brief  Utility macro to define the genclass for dynamicaly loadable components. It correpsonds to flowvr::app::GenClass<X> XClass("X");
     * \warning  You need one declaration per component. You cannot use templates here
     *  Need to be static to avoid creating  an instance  each time a comp.h including this macro is included in a file.
     */
#define GENCLASS(X)  flowvr::app::GenClass<X> X ## Class(#X);


    /** \class GenClass
     * \brief Template utility class to easily register component classes
     */
    template<class Obj>
        class GenClass : public Class
    {

        public:
	    std::string compname; ///< Registered component name
            
            
	    /**
	     * \brief Constructor
	     * \param _name the  reference registered for the class Obj 
	     */
	    GenClass(std::string _name):compname(_name)
	    {
                registerClass();
	    }

            /**
	     * \brief virtual create a new instance of the registered Class
	     * \param objID the id given to the created component.
	     */
	    virtual flowvr::app::Component* create(std::string objID)
	    {
                return new Obj(objID);
	    }

	    /**
	     * \brief Return the registered component name
	     */
	    virtual std::string name()
	    {
                return compname;
	    }

    };


} // namespace app

} // namespace flowvr

#endif



