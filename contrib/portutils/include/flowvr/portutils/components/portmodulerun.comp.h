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

#ifndef PORTMODULERUN_COMP_H_
#define PORTMODULERUN_COMP_H_

#include <flowvr/app/core/component.h>
#include <flowvr/app/core/run.h>
#include <flowvr/app/core/exception.h>
#include <flowvr/app/components/metamodule.comp.h>
#include <flowvr/xml.h>


namespace flowvr
{
	namespace portutils
	{
		class PortModuleRun : public flowvr::app::MetaModule
		{
		public:
			/**
			 * @brief constructor
			 * @param id_  metamodule id
			 * @param interfacefile the configuration file name for the module to load.
			 *        Can be an absolute or relative path.
			 * @param pRun a prototype for the command line to augment for loading the module
			 * @param parent (optional) a pointer to the parent of this port-module.
			 *        When given, the contructor will call parent->insertObject(this).
			 * @param strBinaryName the name of the binary to launch, defaults to
			 *        'flowvr-portbinary'
			 * @param bAllowOverride when set to true, it is ok to override the binary name
			 *        by the parameter 'modulebinary' during execute(). If you set this
			 *        to false, it will always use the file name as given in strBinaryName.
			 *        If set to true (default) it will look for a parameter value of 'modulebinary'
			 *        and only if that is empty, the strBinaryName will be used.
			 */
			PortModuleRun(const std::string& id_,
						  const std::string &interfacefile,
						  const flowvr::app::CmdLine &pRun,
						  flowvr::app::Composite *parent = NULL,
						  const std::string &strBinaryName = "flowvr-portbinary",
						  bool bAllowOverride = true );
			/**
			 * @brief copy constructor
			 *
			 * @param m the MetaModule to copy
			 * Copies the run argument from the other metamodule.
			 */
			PortModuleRun(const PortModuleRun& m);

			/**
			 * @brief destructor
			 *
			 * The metamodule destructor will delete the associated run pointer, as well as the
			 * prototype to use for cloning the interface of the module to start.
			 */
			virtual ~PortModuleRun();

			/**
			 * @brief virtual create:  a new instance of the object initialized with
			 *        same parameter values as this.
			 *
			 * @return a metamodule, cloned by using the constructor passing the id and the portfile parameter
			 */
			virtual flowvr::app::Component* create() const;

			/**
			 * @brief retrieve the prototype used for this PortModuleRun
			 */
			const flowvr::app::Component &getPrototype() const;


			flowvr::app::CmdLine*  getRun() const;

			/**
			 * @brief get the XML structure of runfile
			 *
			 * @return a pointer to the XML description
			 *
			 *
			 * creates a run-string as follows:
			 * \<run metamoduleid=[metamoduleid]"\>[run-string-determined by the run member]\</run\>
			 */
			flowvr::xml::DOMElement* XMLRunBuild() const;


			/**
			 * @brief configure (i.e.: modify the run-argument by augmenting parameters)
			 *
			 * The configuration does:
			 * - add two parameters (portfile, dbgWait) to the command line
			 * - adds the parameter "idstring" with the component id to the command line
			 * - adds parameters according to the interface definition, along with the prefix of th service
			 *   - count parameters will be expanded without argument
			 *   - non-count parameters will be expanded with the argument, in case it is not found,
			 *     the default will be used, if no defaul was given, configure() will throw.
			 */
			void configure();


			/**
			 * @brief execute this component
			 *
			 * Adds a PortParallel child, named 'body' and passes the parallel prototype to it.
			 * It links every port of the PortParallel to all ports of this module. As they use
			 * the same interface file, this should work always. Calls configure() in the end to
			 * setup runtim arguments.
			 */
			void execute();

		protected:
		private:
			flowvr::app::CmdLine *run;      /**<  pointer to run command */
//			std::string metamoduleIdSuffix, /**< An optional metamodule id */
			std::string	strPortfile,        /**< the portfile to use for sub-modules */
						strModuleBinary;    /**< the binary to launch */
			bool        m_bAllowOverride;
			/**
			 * @brief private default constructor.
			 */
			PortModuleRun();

			Component *m_prototype; /**< the prototype to use during cloning of the composite */
		};
	} // namespace portutils
} // namespace flowvr
	

#endif // PORTMODULE_COMP_H_
