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
 * File: src/class.cpp                                             *
 *                                                                 *
 * Contacts:                                                       *
 *  12/10/2003 Jeremie Allard <Jeremie.Allard@imag.fr>             *
 *                                                                 *
 ******************************************************************/
#include "flowvr/app/core/class.h"
#include "flowvr/app/core/msg.h"
#include "flowvr/utils/filepath.h"

#include <map>
#include <iostream>
#include <dlfcn.h>
#include <cctype>

/**
 * \file class.cpp
 */

namespace flowvr
{

namespace app
{

/** \typedef ClassRegistry
 * \brief name->Class associative container
 */
typedef std::map<std::string, Class*> ClassRegistry;

namespace
{
	ClassRegistry classRegistry;
}

/**
 * \brief return the class registry.
 */
static ClassRegistry* getClassRegistry()
{
	return &classRegistry;
}

void Class::listRegistered()
{
	for( ClassRegistry::const_iterator it = classRegistry.begin();
	     it != classRegistry.end(); ++it )
	{
		std::cout << "["
		          << (*it).first
		          << "]\n";
	}
}

bool Class::isValidName(const std::string &name)
{
	if (name.empty())
		return false; // a name can't be empty

	for (unsigned int i = 0; i < name.size(); i++)
	{
		char c = name[i];
		if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z') && !(c >= '0'
				&& c <= '9') && c != '_' && c != '.')
			return false; // bad char
	}
	return true;
}

bool Class::registerClass()
{
	std::string classname = name();
        // always register a lower name (we are case-insensitive)
        std::transform(classname.begin(), classname.end(), classname.begin(),::tolower);

	Msg::debug("Registering Class " + classname, __FUNCTION_NAME__);
	if (!isValidName(classname))
	{
		throw CustomException("Invalid class name " + classname,
				__FUNCTION_NAME__);
		return false;
	}

	if (!(getClassRegistry()->insert(make_pair(classname, this)).second))
	{
		throw CustomException(
				"Class name " + classname + " already registered",
				__FUNCTION_NAME__);
		return false;
	}
	return true;
}

Class* Class::find(std::string classname, std::string libname)
{
	if (!isValidName(classname))
		throw CustomException("Invalid class name " + classname,
				__FUNCTION_NAME__);

        // always use a lower name (we are case-insensitive)
        std::transform(classname.begin(), classname.end(), classname.begin(),::tolower);


	ClassRegistry::const_iterator it = getClassRegistry()->find(classname);
	if (it != getClassRegistry()->end())
		return it->second; // class found

	if (libname.empty())
		return NULL;

	void *handle;

	/// When loading the  library, all components having a GenClass (usually defined using the  GENCLASS macro)
	//  will create an instance of GenClass that will directly register in the registry.
	handle = dlopen(libname.c_str(), RTLD_LAZY); ///< open the library

	if (handle == NULL)
		throw CustomException("Cannot load " + libname + ": " + dlerror(),
				__FUNCTION_NAME__);

	Msg::debug("Library " + libname + " successfully loaded", __FUNCTION_NAME__);

	it = getClassRegistry()->find(classname);
	if (it != getClassRegistry()->end())
	{
		Msg::debug("Component class " + classname + " found", __FUNCTION_NAME__);
		return it->second; // class found
	}
	throw CustomException("Component class " + classname
			+ " not found. Double check the class name is correct (case-insensitive)"
			+ "and  an  instance of  GenClass<" + classname
			+ "> (case-sensitive) is declared in library " + libname, __FUNCTION_NAME__);
	// no return here... line should not be reached.
}

void Class::listComponents(std::string& libname)
{
	if (getClassRegistry()->empty())
	{
		void *handle;
		handle = dlopen(libname.c_str(), RTLD_LAZY);
		if (!handle)
		{
			throw CustomException("Cannot load " + libname + ": " + dlerror(),
					__FUNCTION_NAME__);
		}

		Msg::debug("Library " + libname + " successfully loaded",
				__FUNCTION_NAME__);

	}
	std::cout << "Components available in " << libname << std::endl;
	for (ClassRegistry::const_iterator it = getClassRegistry()->begin(); it
			!= getClassRegistry()->end(); ++it)
	{
		std::cout << "\t- " << it->first << std::endl;
	}

}

std::vector<Component*> Class::getSetComponents(const std::string& libname)
{
	std::vector<Component*> result;
	if (getClassRegistry()->empty())
	{
		void *handle;
		handle = dlopen(libname.c_str(), RTLD_LAZY);
		if (!handle)
		{
			throw CustomException("Cannot load " + libname + ": " + dlerror(),
					__FUNCTION_NAME__);
		}
		Msg::debug("Library " + libname + " successfully loaded",
				__FUNCTION_NAME__);

	}
	for (ClassRegistry::iterator it = getClassRegistry()->begin(); it
			!= getClassRegistry()->end(); ++it)
	{
		Component* comp = (it->second)->create(it->first);
		result.push_back(comp);
	}

	return result;
}

} // namespace app

} // namespace flowvr
