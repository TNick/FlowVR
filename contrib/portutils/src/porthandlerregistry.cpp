/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                        PortUtils library                        *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
*                                                                 *
*   The source code is  covered by different licences,            *
*   mainly  GNU LGPL and GNU GPL. Please refer to the             *
*   copyright.txt file in each subdirectory.                      *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*  Ingo Assenmacher,                                              *
*  Bruno Raffin                                                   *
*                                                                 *
*******************************************************************/


#include <flowvr/portutils/porthandlerregistry.h>


#include <set>
#include <iostream>
#include <memory>
#include <algorithm>

#include <flowvr/portutils/porthandler.h>

namespace
{
	template<class T> class deleteOp : public std::unary_function<T, void>
	{
	public:
		void operator()( T *obj ) const
		{
			delete obj;
		}
	};

	std::auto_ptr<flowvr::portutils::PortHandlerRegistry> Ssingleton(NULL);
}

namespace flowvr
{
	namespace portutils
	{
		PortHandlerRegistry *PortHandlerRegistry::getSingleton()
		{
			if( Ssingleton.get() == NULL)
				Ssingleton.reset(new PortHandlerRegistry);
			return Ssingleton.get();
		}


		PortHandlerRegistry::PortHandlerRegistry()
		{

		}

		PortHandlerRegistry::~PortHandlerRegistry()
		{
	//		std::cout << "PortHandlerRegistry::~PortHandlerRegistry()" << std::endl;
	//		std::set<PortHandlerBase*> stDelete;
	//
	//		for( HNDMAP::const_iterator it = m_map.begin();
	//			 it != m_map.end(); ++it )
	//			stDelete.insert( (*it).second );
	//
	//		std::for_each( stDelete.begin(), stDelete.end(),
	//					   deleteOp<PortHandlerBase>() );
		}

		void PortHandlerRegistry::registerPortHandler( const std::string &strSymbol, PortHandlerBase *pHandler)
		{
			m_map[strSymbol] = pHandler;
	//		std::cout << "registering [" << pHandler << "] as handler with name [" << strSymbol << "]" << std::endl;
		}

		bool PortHandlerRegistry::unregisterPortHandler( const std::string &strSymbol )
		{
			HNDMAP::iterator it = m_map.find( strSymbol );
			if(it == m_map.end())
				return false;

			m_map.erase(it);
			return true;
		}

		bool PortHandlerRegistry::getIsPortHandler( const std::string &strSymbol ) const
		{
			return (m_map.find( strSymbol ) != m_map.end());
		}

		PortHandlerBase *PortHandlerRegistry::getPortHandler( const std::string &strSymbol ) const
		{
			HNDMAP::const_iterator it = m_map.find( strSymbol );
			if(it == m_map.end())
				return NULL;

			return (*it).second;
		}
	} // namespace portutils
} // namespace flowvr

