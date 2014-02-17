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
 * File: include/flowvr/app/core/class.h                           *
 *                                                                 *
 * Contacts:                                                       *
 *  12/10/2003 Jeremie Allard <Jeremie.Allard@imag.fr>             *
 *                                                                 *
 ******************************************************************/
#ifndef __FLOWVR_APP_CLASS_H
#define __FLOWVR_APP_CLASS_H


#include <vector>
#include "flowvr/app/core/component.h"

/**
 * \file genclass.h
 * \brief class for dynamic component loading
 */

namespace flowvr
{

  namespace app
  {

    /** \class Class
     * \brief  Dynamic Component loader
     *  Each class is designated by a (unique) name (case-insensitive)
     *  When a class is created it must be registered using the registerClass  method.
     *
     * To obtain the reference to a Class given its name call the static find
     * method.
     *
     * A Class must implement:
     *  the name method which retrieves this class's name.
     *  the construct method which creates a instance of this class given an object ID.
     */
    class Class
    {
        public:

	    /**
	     * \brief Virtual destructor.
	     */
	    virtual ~Class() {}

	    /**
	     * \brief Check if a  classname is valid. A class name should only contain letters, digits, dots and underscores 
	     * \param name the class name (case-insensitive)
	     */
	    static bool isValidName(const std::string &name);

	    /**
	     * \brief Register this class (must be called after construction of a new class).
	     * \return  false if a Class already registered with the same name.
	     */
            virtual bool registerClass();

	    /**
	     * \brief Name of the registered class. 
	     */
            virtual std::string name()=0;

	    /**
	     * \brief Construct a new composite component from this class with the given object ID.
	     * \param objID the id of the create composite component
	     */
	    virtual Component* create(std::string objID)=0;

	    /**
	     * \brief Find a class with the given name. If not found try to load a component corresponding to this name.
	     * \param classname the composite class name (case-insensitive)
	     * \param libname  the library name where to find the composite class
	     */
            static Class* find(std::string classname, std::string libname);


	    /**
	     * \brief list all loadable   components in a library. Useful to get the content of a library.
	     */
	    static void listComponents(std::string& libname);

	    /**
	     * \brief Get a vector with an instance of each loadable component found in a given library file
	     * \param libname the library name
	     */
	    static std::vector<Component*> getSetComponents(const std::string& libname);

	    static void listRegistered();
    };

} // namespace app

} // namespace flowvr

#endif
