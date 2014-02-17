/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                                                                 *
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
*******************************************************************
*                                                                 *
*  Contact :                                                      *
*                                                                 *
******************************************************************/

#ifndef DATASERVICE_H_
#define DATASERVICE_H_

#include <flowvr/portutils/portservicelayer.h>


namespace example3
{
	class DataService : public flowvr::portutils::IServiceLayer
	{
		public:
			DataService( flowvr::portutils::Module & );
			~DataService();

			virtual bool setParameters( const flowvr::portutils::ARGS & );

			class Data
			{
			public:
				Data();
				char data[256];
			};

			Data produce();
			void consume( Data & );
			const char *serialize( const Data &, size_t &n );
			Data  deserialize( const char *, size_t n );
	};
} // namespace example3

#endif // DATASERVICE_H_
