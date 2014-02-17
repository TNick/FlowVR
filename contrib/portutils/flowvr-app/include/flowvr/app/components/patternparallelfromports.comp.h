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

#ifndef __FLOWVR_APP_COMPARALLEL_FROM_PORTS_H_
#define __FLOWVR_APP_COMPARALLEL_FROM_PORTS_H_

#include "flowvr/app/core/component.h"
#include "flowvr/app/components/connection.comp.h"

namespace flowvr
{
  namespace app
  {

    /**
     * \brief this component has 1 parameter
     * instances_from_port : port name used to find number of instances to create
     */
    template<class Component_> class PatternParallelFromPorts : public Composite
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
            PatternParallelFromPorts(const std::string& id_) : Composite(id_, "patternparallelfromports")
            {
                setInfo("N components in parallel");
                addParameter("instances_from_port");
                this->createInterface();
            }

            virtual ~PatternParallelFromPorts()
            {
            }

            virtual Component* create() const
            {
                return new PatternParallelFromPorts<Component_>(getId());

            }
            
            virtual void execute()
            {
			const std::string portName = getParameter<std::string>("instances_from_port");
			Port* pPort = this->getPort(portName);
			unsigned int i = 0;

			std::list<const Port*> primlist;
			pPort->getPrimitiveSiblingList(primlist);

			if( primlist.empty() )
				{
					throw CustomException("In PatternParallelFromPorts " + getFullId() + ", no primitive sibling connected on port '"+portName+"' (no Primitive linked to this port)", __FUNCTION_NAME__);
				}

			this->setParameter<unsigned int>("nb_instances",primlist.size());
			for(std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
			{
				Component_ * rnNode = addObject<Component_>(toString<unsigned int>(i));
				for (pPortIterator itLink = rnNode->getPortBegin(); itLink!= rnNode->getPortEnd(); ++itLink)
				{
					if ((*itLink)->getId() == portName)
					{
						link(pPort,*itLink);
						(*itLink)->addRestrictionToPrimitiveSibling(*it);
					}
					else
					{
						Port* p = this->getPort((*itLink)->getId());
						try
						{
							std::list<const Port*> pprimlist;
							p->getPrimitiveSiblingList(pprimlist);

							if (pprimlist.size() != primlist.size())
								link(*itLink, p);
							else
							{
								std::list<const Port*>::const_iterator itP=pprimlist.begin();
								for(unsigned int ip = 0; ip != i; ++ip)
								{
									++itP;
								}
								link(p,*itLink);
								(*itLink)->addRestrictionToPrimitiveSibling(*itP);
							}
						}
						catch(CustomException e)
						{
							link(*itLink, p);
						}
					}
				}
				++i;
			}
		}


         virtual void setHosts()
         {
			//SAME AS SIBLING
			const std::string portName = getParameter<std::string>("instances_from_port");
			Port* pPort = this->getPort(portName);
			unsigned int i = 0;

			std::list<const Port*> primlist;
			pPort->getPrimitiveSiblingListSkipConnections(primlist);

			if ( primlist.size() == 0)
				throw NoPrimitiveConnectedException(portName, this->getFullId(), __FUNCTION_NAME__);

			for(std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it)
			{
				Component* rnNode = getComponent(toString<unsigned int>(i));
				Component* siblingComp = (*it)->getOwner();

				// siblingComp is a primitive (but not a connection)  so one host expected
				if(siblingComp->hostList.size() > 1 )
                                    throw CustomException("Primitive "+getFullId()+" looking for host of Primitive " + siblingComp->getFullId() + ": "+ toString<int>(hostList.size())+ " hosts (one expected)", __FUNCTION_NAME__);
				else if( siblingComp->hostList.empty() )
					throw CustomException("Primitive "+getFullId()+" looking for host of Primitive " + siblingComp->getFullId() + ": no host (one expected)", __FUNCTION_NAME__);

				rnNode->hostList.push_back(siblingComp->hostList.front());
				rnNode->hostList.back().appendtoOrigin(siblingComp->getFullId());

				++i;
			}
		 }
};

}//namespaces
}//namespaces
#endif	//__FLOWVR_APP_COMPARALLEL_FROM_PORTS_H_
