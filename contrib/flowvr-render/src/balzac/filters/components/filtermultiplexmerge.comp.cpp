/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                   Balzac FlowVR render frontend                 *
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
* File: filtermultiplexmerge.comp.cpp                             *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

#include "filtermultiplexmerge.comp.h"


using namespace flowvr::app;

namespace flowvr
{
	namespace render
	{
		namespace balzac
		{

			class FilterMultiplexMergePrimitive : public Filter
			{
			public:
				FilterMultiplexMergePrimitive(const std::string &strId)
				: Filter( strId, "FilterMultiplexMerge" )
				{
					addPort("trigger", INPUT, STAMPS, "", flowvr::app::Port::ST_BLOCKING );
					addPort("out", OUTPUT );
					addParameter<int>("nb", 0);
					addParameter<bool>("useSegments",false);
				}

				virtual void createPorts(int nNum)
				{
					// get the number of in ports
					for(int i = 0; i < nNum; ++i)
						addPort("in"+toString<int>(i), INPUT, FULL, "", flowvr::app::Port::ST_BLOCKING);

					setParameter<int>("nb", nNum);
				}


				virtual Component* create() const
				{
					FilterMultiplexMergePrimitive* r = new FilterMultiplexMergePrimitive(getId());
					r->createPorts( getPortSize() );
                    return r;
				}
			};


			FilterMultiplexMerge::FilterMultiplexMerge( const std::string &strId )
			: Composite( strId, "Shell" )
			{
				addPort("in", INPUT, FULL, "", flowvr::app::Port::ST_BLOCKING );
				addPort("trigger", INPUT, STAMPS, "", flowvr::app::Port::ST_BLOCKING );
				addPort("out", OUTPUT, FULL );
			}

			Component* FilterMultiplexMerge::create() const
			{
				return new FilterMultiplexMerge(getId());
			}

			void FilterMultiplexMerge::execute()
			{
	            Port* p = getPort("in");
				std::list<const Port*> primlist;
				p->getPrimitiveSiblingList(primlist);
				unsigned int numberInputs = primlist.size();

	            FilterMultiplexMergePrimitive  * fmultiplex =  addObject<FilterMultiplexMergePrimitive>("body");
	            fmultiplex->createPorts(numberInputs);

	            unsigned int counter = 0;

				for( std::list<const Port*>::const_iterator it = primlist.begin(); it != primlist.end(); ++it, ++counter)
				{
					link(p, fmultiplex->getPort("in" + toString<unsigned int>(counter)));
					fmultiplex->getPort("in" + toString<unsigned int>(counter))->addRestrictionToPrimitiveSibling(*it);
				}

	            link(fmultiplex->getPort("out"), getPort("out"));
	            link(fmultiplex->getPort("trigger"), getPort("trigger"));
			}
		}
	}
}
