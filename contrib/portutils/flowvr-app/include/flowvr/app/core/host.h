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
 * File: ./include/flowvr/app/data.h                               *
 *                                                                 *
 * Contacts:  Antoine Vanel                                        *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP_HOSTS_H
#define __FLOWVR_APP_HOSTS_H

#include <vector>
#include <string>
#include <algorithm>
#include "flowvr/app/core/exception.h"
#include "flowvr/app/core/msg.h"
#include "flowvr/app/core/utils.h"


namespace flowvr { namespace app {

   	  /** \file host.h
     *  \brief Class used to handle component hosts (for mapping)
     **/


/**
 * \brief  single host
 */
	class Host
	{
	private:
		std::string	name;///< host name
		std::string origin; ///< where the host name comes from
	public:

		Host(const std::string& _name, const std::string& _origin ): name(_name),origin(_origin){};

		Host(): name(),origin(){};

		const std::string getName() const {return name;};

		const std::string getOrigin() const {return origin;};

		void setName(const std::string& _name)
			{
				if( _name.empty()) throw CustomException("Host: Intend to set an empty host name",__FUNCTION_NAME__);
				name= _name;
			};

		void setOrigin(const std::string& _origin){origin= _origin; };

		void appendtoOrigin(const std::string& extra){ origin+=','+extra; };
	};



}} // end namespace
#endif //__HOSTS__
