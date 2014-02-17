/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                       code
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA.  ALL RIGHTS RESERVED.                                    *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING file for further information.                           *
*                                                                 *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: DataService.cpp                                    
* Contacts: assenmac                                     
*                                                                 *
******************************************************************/

#include "services/DataService.h"
#include <string.h>

DEFIMP_SERVICEPLUG(example3::DataService)


namespace example3
{

	DataService::Data::Data()
	{
		memset( data, 0, 256 );
	}

	DataService::DataService( flowvr::portutils::Module &m )
	: IServiceLayer(m)
	{

	}

	DataService::~DataService()
	{
	}

	bool DataService::setParameters( const flowvr::portutils::ARGS & )
	{
		return true;
	}


	DataService::Data DataService::produce()
	{
		return Data();
	}

	void DataService::consume( Data & )
	{
		// hummy yummy yummy...
	}


	const char *DataService::serialize( const Data &d, size_t &n )
	{
		n = 256;
		return &d.data[0];
	}

	DataService::Data  DataService::deserialize( const char *d, size_t n )
	{
		Data data;
		memcpy( data.data, d, n );
		return data;
	}
} // namespace
