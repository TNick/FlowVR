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
 *        File: ./src/testlib.cpp                                  *
 *                                                                 *
 *	Contact : Jean-Denis Lesage                                *
 *                                                                 *
 ******************************************************************/

#include <algorithm>
#include <flowvr/utils/filepath.h>
#include <ftl/cmdline.h>
#include "flowvr/app/core/class.h"
#include "flowvr/app/core/genclass.h"
#include "flowvr/app/core/traverse.h"
#include "flowvr/app/core/msg.h"
#include "flowvr/app/core/controller.h"

using namespace flowvr::app;
using namespace flowvr::xml;



int main(int argc, char** argv)
{

    const std::string tmps="flowvr-test [options] [LibName]";
    ftl::CmdLine cmdline("flowvr-test [options] [LibName]");

    // Parse commande line
    // Exit if error or --help option
    if ( ! cmdline.parse(argc, argv) ) return 1;

    // For testing, the verbose level is 3
    Msg::verboseLevel= 3;

    std::string libname;

    // if justListComponents, the name of main component is useless
    if  ( cmdline.args.size() !=  1 || cmdline.args[0].empty() )
        {
            Msg::error("One and only one library file needed as input argument",__FUNCTION_NAME__);
            return 1;
        }
    // library name  (also the component class name)
    libname = cmdline.args[0];

    // compute lower case version  of component  class name
    std::string libnamelower = libname;
    std::transform(libnamelower.begin(), libnamelower.end(),libnamelower.begin(),::tolower);


#ifdef __APPLE__
    static flowvr::utils::FilePath libpath("DYLD_LIBRARY_PATH");
#else
    static flowvr::utils::FilePath libpath("LD_LIBRARY_PATH");
#endif

    // Check we  find  the library
    if (!libpath.findFile(libname)) {
        Msg::error("Cannot find "+libname,__FUNCTION_NAME__);
        return 0;
    }
    Msg::debug("Found "+libname,__FUNCTION_NAME__);



    bool noError = true;

    // Dynamic component loading
    std::vector<Component*> components = Class::getSetComponents(libname);
    for(std::vector<Component*>::iterator itComp = components.begin(); itComp != components.end(); ++itComp)
	{

            Msg::debug("Test component type = " + (*itComp)->getId(), __FUNCTION_NAME__);
            /// Set traverse (fixe point mode)  for calling the execute method

            Controller<void, void> controlExecute(&Component::execute);
            TraversePointFixe treeExecute(*itComp, &controlExecute);

            Controller<void, Component::Host> createHost(&Component::setHost);
            TraversePointFixe traverseLocalHost(*itComp, &createHost);

            Component::Host h("localhost");
            createHost.parameter = h;

            do
                {
                    traverseLocalHost(*itComp);
                    treeExecute(); /// call traverse on execute method
                }while(treeExecute.isModified );

            // if error signal it
            if (!treeExecute.listError.empty())
                {
                    for (std::list<CustomException>::const_iterator it = treeExecute.listError.begin(); it != treeExecute.listError.end(); ++it)
                        {
                            Msg::errorException(*it);
                            noError = false;
                        }
                }
            delete *itComp; // need virtual destructor
	}
    if (noError)
        Msg::debug("Library " + libname + " successfully tested.", __FUNCTION_NAME__);
};
