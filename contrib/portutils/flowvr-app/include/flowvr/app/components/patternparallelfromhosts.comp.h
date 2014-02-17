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
 *  File : ./include/flowvr/app/components/patternparallel.comp.h  *
 *                                                   		  *
 *      Contacts : jean-denis.lesage@imag.fr                       *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP_COMPARALLEL_HOSTS_H_
#define __FLOWVR_APP_COMPARALLEL_HOSTS_H_

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/connection.comp.h"


namespace flowvr
{
  namespace app
  {

    template<class Component_> class PatternParallelFromHosts : public Composite
    {
        private:

            void createInterface()
            {
                Component_ pattern("pattern");
                for (ConstpPortIterator it = pattern.getPortBegin(); it
                         != pattern.getPortEnd(); ++it)
                    {
                        addPort((*it)->getId(), (*it)->getType());
                    }
            }

        public:
            // Clone interface of component
            PatternParallelFromHosts(const std::string& id_) : Composite(id_, "patternparallelfromhosts")
            {
                setInfo("Create N instances of template component, N being defined from the host list given by the parent component");
                this->createInterface();
            }

            virtual ~PatternParallelFromHosts()
            {
            }


            virtual Component* create() const
            {
                return new PatternParallelFromHosts<Component_>(getId());

            }
            
            virtual void execute()
            {
            	// hostlist provided through propagation from parent
               if (hostList.empty() )
            	   throw CustomException("PatternParallelFromHosts " + getFullId() + ": empty host list (at least one host expected)", __FUNCTION_NAME__);

				unsigned int i = 0;
				for (HostList::const_iterator itHost = hostList.begin(); itHost != hostList.end(); ++itHost, ++i)
				{
					Component_* rnNode =	addObject<Component_>(toString<	unsigned int>(i));
					for (pPortIterator itLink =rnNode->getPortBegin(); itLink != rnNode->getPortEnd(); ++itLink)
					{
						Port* parentPort = this->getPort((*itLink)->getId());
						link(*itLink, parentPort);
					}
				}
            }
        


         virtual void setHosts()
         {

        	if ( hostList.empty() )
              throw CustomException("PatternParallelFromHosts " + getFullId() + ": empty host list (at least one host expected)", __FUNCTION_NAME__);

			unsigned int i = 0;
			for (HostList::const_iterator itHost = hostList.begin(); itHost != hostList.end(); ++itHost, ++i)
				{
				Component* rnNode = getComponent(toString<unsigned int>(i));
				if (dynamic_cast<ConnectionBase*>(rnNode)  == NULL)
				{
					rnNode->hostList.push_back(*itHost);
					rnNode->hostList.back().appendtoOrigin(getId());
				}
			}

		}

    };

}
}
#endif	//_COMPARALLEL_HOSTS_H_
