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
 * File: ./include/flowvr/app/components/metamodulempirun.comp.h   *
 *                                                                 *
 * Contacts: jean-denis lesage                                     *
 *                                                                 *
 ******************************************************************/


#ifndef __FLOWVR_APP__METAMODULEMPIRUN_H
#define __FLOWVR_APP__METAMODULEMPIRUN_H

#include "flowvr/app/components/metamoduleparallel.comp.h"


/**
 * \file metamodulempirun.comp.h
 * \brief implements a metamodule using a mpi launcher. 
 */


namespace flowvr{ namespace app{

    /**
     * \class MetaModuleMPIRun
     * \brief Metamodule using a mpi launcher.  This metamodule launches modules of type Module_ (a template). Because mpirun syntax can vary 
     *  between implentation, the type of the launcher (MPIRun_) is a template.  The number of module  instances is defined according to the host list size. 	 */

    template <class Module_,class MPIRun_>
        class MetaModuleMPIRun : public MetaModuleParallel<Module_ , MPIRun_ >
    {
        private :
            /**
             * \brief default constructor
             * \warning You shouldn't use it
             */
            MetaModuleMPIRun() : MetaModuleParallel<Module_,MPIRun_>("") {};
	    
            CmdLine   cmdline; ///<  store the commande line before being inserted in the mpirun  script line
	    
        public :
            /**
             * \brief constructor
             * \param id_ the id of the metamodule
             * \param run_ the command line required to launch the module (usually the binary name of the module)
             */
            MetaModuleMPIRun(const std::string& id_, const CmdLine&  run_);
	    
            /**
             * \brief copy constructor
             */
            MetaModuleMPIRun(const MetaModuleMPIRun& m) : cmdline(this->cmdline), MetaModuleParallel<Module_, MPIRun_ >(m){};

            /**
             * \brief destructor
             */
            virtual ~MetaModuleMPIRun() {};


            /**
             * \brief virtual create:  a new instance of the object initialized with same parameters values as this.
             */
            virtual Component* create() const
            {
                return new MetaModuleMPIRun(this->getId(),this->cmdline);
            };

    };

    template <class Module_, class MPIRun_>
        MetaModuleMPIRun<Module_,MPIRun_>::MetaModuleMPIRun(const std::string& id_,  const CmdLine&  run_) :  MetaModuleParallel<Module_, MPIRun_ >(id_), cmdline(run_)
        {
        	this->setInfo("A MPI metamodule");

        	// Set run using mpirun (the MPIRun_ template enables to change the launcher according to the mpi implementation)
        	MPIRun_  mpirun(this);
        	mpirun.addCommand(run_); ///< the command line given in parameter is used as the argument of mpirun
        	mpirun.setVerbose();
        	this->setRun(mpirun);

        };



};};

#endif //__FLOWVR_APP__METAMODULEMPIRUN_H
