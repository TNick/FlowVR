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

#include <flowvr/portutils/components/portmodule.comp.h>
#include <flowvr/portutils/components/portinterfacecache.h>
#include <flowvr/app/core/exception.h>


namespace flowvr
{
	namespace portutils
	{
		PortModule::PortModule( const std::string &strId, const std::string &strPortFile, flowvr::app::Composite *parent )
		: flowvr::app::Module( strId )
		, m_strPortfile(strPortFile)
		{
			if( m_strPortfile.empty() )
				throw flowvr::app::CustomException("m_strPortfile empty, can not create PortModule component.", __FILE__);

			PortInterfaceCache *cache = PortInterfaceCache::getSingleton();
			PortInterfaceCache::Interface interface = cache->loadInterface(strId, m_strPortfile);

			if(interface.getIsValid())
			{
				setInfo( interface.m_Info );

				for(PortInterfaceCache::Interface::DESCMAP::const_iterator cit = interface.m_mpDescriptors.begin();
					cit != interface.m_mpDescriptors.end(); ++cit )
						(*cit).second.createPorts( *this, true );
			}

			if(parent)
				parent->insertObject(this);
		}

		flowvr::app::Component *PortModule::create() const
		{
			return new PortModule(getId(), m_strPortfile );
		}
	} // namespace portutils
} // namespace flowvr
