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
 *     File : flowvr/app/core/utils.h                              *
 *                                                                 *
 *       Contact : Jean-Denis Lesage                               *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP_UTILSFLOWVRAPP__
#define __FLOWVR_APP_UTILSFLOWVRAPP__

#include <string>

namespace flowvr { namespace app {

    template <class T>
        struct DeletePtr
    {
            void operator() (T* pValue) const
            {
                delete pValue;
                pValue=NULL;
            };
    };

    template <class T>
        struct SameIdRef
    {
            std::string id;
            SameIdRef(const std::string& id_) : id(id_) {};
            bool operator() (const T& t) { return t.getId() == id; };
    };

    template <class T>
        struct SameIdPtr
    {
            std::string id;
            SameIdPtr(const std::string &id_) : id(id_) {};
            bool operator() (const T* t) { return (t->getId() == id); };
    };
}; };

#endif //__UTILSFLOWVRAPP__
