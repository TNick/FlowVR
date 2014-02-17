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
 * File: ./include/flowvr/app/msg.h                                *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/


/**
 * \file msg.h
 * \brief All you need to add debug messages in your code.
 */

#ifndef __FLOWVR_APP_MSG_H
#define __FLOWVR_APP_MSG_H


/**
 * \def __FUNCTION_NAME__
 * \brief give the name of the current function.
 * \warning The detailed name of function is only provided with the GNU g++ compiler
 */
#ifdef __GNUG__
#define __FUNCTION_NAME__ __PRETTY_FUNCTION__
#else
#define __FUNCTION_NAME__ __FUNCTION__
#endif


#include <string>
#include <exception>
#include <iostream>
#include "flowvr/app/core/exception.h" 

namespace flowvr { namespace app {

    /**
     * \def DEBUGLEVEL
     * \brief the number of debug levels
     */
#define DEBUGLEVEL 5

    class Msg {
        private: 
            static std::string levelDebug[DEBUGLEVEL];
  
        public:
            /**
             * \var verboseLevel
             * \brief control verbosity level: print to stderr all messages with a level smaller or equal to verbosity level. 
             */
            static unsigned int verboseLevel; 
   
   
            /**
             * \brief Write an message in the stderr. The message will be "LEVEL : MESSAGE. FUNCTIONNAME";
             * \param msg the message 
             * \param level an integer from 0 to DEBUGLEVEL defining the message level
             * \param functionName the name of the function
             * \warning if level >= DEBUGLEVEL then level = DEBUGLEVEL-1
             */
            static inline void message(const std::string& msg, unsigned int level, const std::string& functionName);
   
   
            /**
             * \brief Write an message in the stderr with the WARNING level.
             * \param msg the message
             * \param functionName the name of the function
             */
            static void warning(const std::string& msg, const std::string& functionName); 
   
   
            /**
             * \brief Write an message in the stderr with the ERROR level.
             * \param msg the message
             * \param functionName the name of the function
             */
            static void error(const std::string& msg, const std::string& functionName); 
   
   
            /**
             * \brief Write exception message  in the stderr with the ERROR level.
             * \param e the exception 
             */
            static void errorException(const CustomException& e);
   
   
            /**
             * \brief Write exception message in the stderr with the WARNING level.
             * \param e  the exception
             */
            static void warningException(const CustomException& e);
   
            /**
             * \brief Write an message in the stderr with the DEBUG level.
             * \param msg the message
             * \param functionName the name of the function
             */
            static void debug(const std::string& msg, const std::string& functionName);

            /**
             * \brief Write exception message in the stderr with the DEBUG level.
             * \param e  the exception
             */
            static void debugException(const CustomException& e);
   
            /**
             * \brief Write an message in the stderr with the INFO level
             * \param msg the message
             * \param functionName the name of the function
             */
            static void info(const std::string& msg, const std::string& functionName);
   
            /**
             * \brief You can test a result with this function. The output will be DEBUG : true/false.
             * \param result a bool result (a test for example)
             */
            static inline void result(bool result) { std::cerr << "DEBUG : " << std::boolalpha << result << std::endl; } ;

    };
   
}; };

#endif //__FLOWVR_APP_MSG_H
