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
 * File: ./include/flowvr/app/components/filterlogger.comp.h       *
 *                                                                 *
 * Contacts:  Antoine Vanel                                        *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__FILTER_LOGGER_H
#define __FLOWVR_APP__FILTER_LOGGER_H

#include "flowvr/app/components/filter.comp.h"

namespace flowvr
{
	namespace app
	{
		/**
		 * \class FilterLogger
		 * \brief Filter dedicated to traces : Collect traces from the daemon. Create a raw message that will be writen in a file
		 **/

		class FilterLogger : public Filter
		{
			public :

			FilterLogger(const std::string& id_) : Filter(id_, "DefaultLogger")
			{
				addPort("log", OUTPUT);
                                
                addParameter("period", "60");
                addParameter("buffer_nb", "5");
                addParameter("buffer_size", "512000");

                // no call to set info, because we do not call XMLBuild method of class component.
			}

            virtual Component* create() const {return new FilterLogger(getId());};
		};
          
      }
} // end namespace

#endif // __FLOWVR_APP__FILTER_LOGGER_H


