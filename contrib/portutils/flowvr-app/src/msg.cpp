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
 *   File: flowvr/app/src/msg.cpp                                  *
 *                                                                 *
 *   Author: Bruno Raffin                                          *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/msg.h"

#include <iostream>

namespace flowvr { namespace app {


    std::string  Msg::levelDebug[DEBUGLEVEL] = { "CRITICAL", "ERROR", "WARNING", "DEBUG", "INFO" };

    unsigned int Msg::verboseLevel=1;

    void Msg::info(const std::string& msg, const std::string& functionName)
    {
        Msg::message(msg,4,functionName);
    };

    void Msg::debug(const std::string& msg, const std::string& functionName)
    {
        Msg::message(msg,3,functionName);
    };
    
    void Msg::debugException(const CustomException& e)
    {
        Msg::message(e.what(),3, e.functionName); 
    };
    
    
    void Msg::warningException(const CustomException& e)
    {
        Msg::message(e.what(),2, e.functionName); 
    };

    void Msg::warning(const std::string& msg, const std::string& functionName)
    {
        Msg::message(msg, 2, functionName);
    };

    void Msg::error(const std::string& msg, const std::string& functionName)
    {
        Msg::message(msg, 1, functionName);
    };
    
    void Msg::errorException(const CustomException& e)
    {
        Msg::message(e.what(),1, e.functionName);
    };
    
    
    void Msg::message(const std::string& msg, unsigned int level, const std::string& functionName)
    {
    //	std::string out;
    	std::stringstream out;

        if (level <= Msg::verboseLevel )
            {
                out << Msg::levelDebug[level >= DEBUGLEVEL ? DEBUGLEVEL - 1 : level] << " in function " << functionName << std::endl;
                out << "\t" << msg << std::endl<<std::endl;
            }

    	if (level <= 1)
    		std::cerr << out.str();
    	else
    		std::cout << out.str();

    };


}; };
