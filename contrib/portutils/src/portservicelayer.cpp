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

#include <flowvr/portutils/portservicelayer.h>

namespace flowvr
{
	namespace portutils
	{

		std::string createParameterName( const std::string &strPrefix, const std::string &strParam )
		{
			if(strPrefix.empty())
				return strParam;
			else
				return strPrefix + std::string("-") + strParam;
		}

		flowvr::portutils::Module &IServiceLayer::getParent() const
		{
			return m_parent;
		}
	} // namespace portutils
} // namespace flowvr
