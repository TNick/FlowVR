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
 * File: ./include/flowvr/app/src/routingnode.comp.cpp             *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/components/routingnode.comp.h"

namespace flowvr { namespace app {

	void RoutingNode::setHostListfromCsv()
		{
		    if (csvHostMap != NULL)
		    {
		    		const  HostMap::iterator it = csvHostMap->find(getFullId());
		    		if (  it != csvHostMap->end() )
		    		{
		    			if( (*it).second.size() > 1 )
		    				throw CustomException(getType()+" "+ getFullId() + ": routing node but "+toString<int>((*it).second.size())+" hosts provided through CSV (one expected)", __FUNCTION_NAME__);

		    			if ( (*it).second.size() == 1 )
		    				{
		    				hostList.clear();
		    				hostList.push_back(it->second.front());
		    				}
		    		}
		    }
		};


	 void RoutingNode::setHostsMainController()
	 {
		 // If hostList set from csv (always performed when creating the component)
		 // the hostList is still unchanged at this point
		 // (propagation from a parent composite only performed if hostList was empty)
		 // Thus if the hostList is still empty we can safely call the setHost controller
		 if (hostList.empty())
		 {
			 setHosts();
		 }

		 // Classic error check
		 if(hostList.size() > 1)
			 throw CustomException("RoutingNode "+ getFullId()+" : "+toString<int>(hostList.size())+" hosts for mapping (one expected).", __FUNCTION_NAME__);
		 else if(hostList.empty())
			 throw CustomException("RoutingNode "+ getFullId()+" : no host for mapping (one expected).", __FUNCTION_NAME__);
	 };



	void RoutingNodeStamps::setHostListfromCsv()
			{
			    if (csvHostMap != NULL)
			    {
			    		const  HostMap::iterator it = csvHostMap->find(getFullId());
			    		if (  it != csvHostMap->end() )
			    		{
			    			if( (*it).second.size() > 1 )
			    				throw CustomException(getType()+" "+ getFullId() + ": routing node stamps but "+toString<int>((*it).second.size())+" hosts provided through CSV (one expected)", __FUNCTION_NAME__);

			    			if ( (*it).second.size() == 1 )
			    				{
			    				hostList.clear();
			    				hostList.push_back(it->second.front());
			    				}
			    		}
			    }
			};



	 void RoutingNodeStamps::setHostsMainController()
	 {
             Component::setHostsMainController();// Enforces  that setHosts called first on parent

		 // If hostList set from csv (always performed when creating the component)
		 // the hostList is still unchanged at this point
		 // (propagation from a parent composite only performed if hostList was empty)
		 // Thus if the hostList is still empty we can safely call the setHost controller
		 if (hostList.empty())
		 {
			 setHosts();
		 }

		 // Classic error check
		 if(hostList.size() > 1)
			 throw CustomException("RoutingNodeStamps "+ getFullId()+" : "+toString<int>(hostList.size())+" hosts for mapping (one expected).", __FUNCTION_NAME__);
		 else if(hostList.empty())
			 throw CustomException("RoutingNodeStamps "+ getFullId()+" : no host for mapping (one expected).", __FUNCTION_NAME__);
	 };


}; }; // end namespace


