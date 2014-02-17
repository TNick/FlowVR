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
 *  Contact : Jean-Denis.Lesage@imag.fr                            *
 *                                                                 *
 ******************************************************************/

// basic components includes
#include <flowvr/app/components/metamoduleflowvr-run-ssh-singleton.comp.h>
#include <flowvr/render/components/moduleviewer.comp.h>

// primes specific components includes

#ifndef _METAMODULEVIEWERTEXTURE_H_
#define _METAMODULEVIEWERTEXTURE_H_

using namespace flowvr::app;
using namespace flowvr::render;

namespace tutorial {

    // The goal of this component is to encapsulate viewer component and add a command line to launch it
    // ModuleViewer is a generic class with the common interface of viewer components. Use it if your viewer has only a scene and a matrix ports.
class MetaModuleViewerTexture : public MetaModuleFlowvrRunSSHSingleton<ModuleViewer>
{
public :
 MetaModuleViewerTexture(const std::string& id_ ) : MetaModuleFlowvrRunSSHSingleton<ModuleViewer>(id_, CmdLine("viewertexture") ) 
  {
    setInfo("Metamodule launching viewer texture module");
  };

  //Mandatory virtual destructor
  virtual ~MetaModuleViewerTexture(){};
	
  // Mandatory create  method
  virtual Component* create() const { return new MetaModuleViewerTexture(getId());};
};

};

#endif //_METAMODULEVIEWERTEXTURE_H_
