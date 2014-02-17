/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                        *
 * INRIA                                                           *
 * ALL RIGHTS RESERVED.	                                           *
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
 * File: ./include/flowvr/app/components/metamodule.comp.h	  *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#ifndef __FLOWVR_APP__METAMODULE_H
#define __FLOWVR_APP__METAMODULE_H

#include "flowvr/app/core/genclass.h"
#include "flowvr/app/core/component.h"
#include "flowvr/app/core/msg.h"
#include "flowvr/app/core/run.h"
#include "flowvr/app/core/host.h"
#include <string>
#include <vector>
#include <map>

/**
 * \file metamodule.comp.h
 * \brief implements the base metamodule class
 */
namespace flowvr
{
	namespace app
	{

		class MetaModule : public Composite
		{
		private:
			std::string metamoduleIdSuffix; ///< An optional metamodule id
		public:
			MetaModule( const std::string &_id )
			: Composite(_id, "metamodule" ), metamoduleIdSuffix(){};

			MetaModule( const MetaModule &m )
			: Composite(m), metamoduleIdSuffix(m.metamoduleIdSuffix){};

			void setMetaModuleIdSuffix(const std::string& suffix_)
			{
				metamoduleIdSuffix = suffix_;
			}

			std::string getMetaModuleIdSuffix() const
			{
				 return metamoduleIdSuffix;
			}

			virtual void setHostListfromCsv()
			{
		      	if (csvHostMap != NULL)
		      	{
		      		const  HostMap::iterator it = csvHostMap->find(getFullId());
		      		if (  it != csvHostMap->end() )
		      		{
                                    hostList.clear();
                                    hostList.insert(hostList.begin(),it->second.begin(),it->second.end());
		      		}
		      	}
			};


		};


		/**
		 * \class MetaModule
		 * \brief This base metamodule class. A metamodule is a composite component with single run  command (launcher).
		 *        The run type is defined by the Run template argument.
		 * \warning a metamodule should only contain modules (primitive modules that is).
		 *          Adding modules is performed in specialized classes inheriting from this base class.
		 */

		template <class Run> ///< Type of run command used by this metamodule
		class MetaModuleRun : public MetaModule
		{
		private:
			Run *run; ///<  pointer to run command

			/**
			 * \brief private default constructor.
			 */
			MetaModuleRun()
			: MetaModule("")
			, run(NULL)
			{}

		public :


			/**
			 * \Brief constructor
			 * \param id_  metamodule id
			 */
			MetaModuleRun(const std::string& id_)
			: MetaModule(id_)
			, run(NULL)
			{}

			/**
			 * \brief copy constructor
			 * @param m the MetaModule to copy
			 * Copies the run argument from the other metamodule.
			 */
			MetaModuleRun(const MetaModuleRun& m)
			: MetaModule(m)
			, run(new Run(*(m.getRun()))) // in case m is not assigned to a run, this will throw
			{}

			/**
			 * \brief destructor
			 * The metamodule destructor will delete the associated run pointer.
			 */
			virtual ~MetaModuleRun()
			{
				delete run;
			}

			/**
			 * \brief virtual create:  a new instance of the object initialized with
			 *        same parameter values as this.
			 * \return a metamodule, cloned by using the constructor passing the id
			 */
			virtual Component* create() const
						{
				return   new MetaModuleRun<Run>(this->getId());
						}


			/**
			 * \brief set the run command to use
			 * \param newrun  the new run to use
			 * \return a pointer to new run
			 */
			Run*   setRun(const Run& newrun)
						{
				if (run != NULL)
					Msg::warning(getType() + " " + getFullId()+": set new run erasing existing one", __FUNCTION_NAME__);

				delete run; // dispose old one
				run = new Run(newrun); ///< copy run command given in parameter
				return run;
						}


			/**
			 * \brief get the current run command
			 * \return a pointer to the run command
			 */
			Run*  getRun()
						{
				if (run == NULL)
					throw NullPointerException(this->getType(), this->getFullId(), __FUNCTION_NAME__);
				return run;
						}


			/**
			 * \brief get the current run command (const version)
			 * \return a const pointer to the run command
			 */
			const Run*  getRun() const
						{
				if (run == NULL)
					throw NullPointerException(this->getType(), this->getFullId(), __FUNCTION_NAME__);
				return run;
						}



			/**
			 * \brief Build the launching command for the metamodule
			 * Creates a run-string as follows:
			 * <run metamoduleid=[metamoduleid]">[run-string-determined by the run member]</run>
			 * \return a pointer to the XML description
			 *
			 */
			virtual xml::DOMElement* XMLRunBuild() const;

		};



	template <class Run>
		xml::DOMElement* MetaModuleRun<Run>::XMLRunBuild() const
		{
			xml::DOMElement* result = new xml::DOMElement("run");

			result->SetAttribute("metamoduleid", getFullId());

			std::string runstring;

			try
			{
				runstring = MetaModuleRun<Run>::getRun()->getString(); // call getString() on run to build the run  script line.
			}
			catch (const CustomException& e)
			{
				delete result;
				throw(e);
			}

			if (runstring.empty())
			{
				delete result;
				throw CustomException(MetaModuleRun<Run>::getType() + " " + MetaModuleRun<Run>::getFullId() + ": empty run string", __FUNCTION_NAME__);
			}

			xml::DOMText* cmd = new xml::DOMText(runstring);
			result->LinkEndChild(cmd);
			return result;
		}

	} // namespace app
} // namespace flowvr

#endif // __FLOWVR_APP__METAMODULE_H
