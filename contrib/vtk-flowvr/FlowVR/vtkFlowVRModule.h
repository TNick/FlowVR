/******* COPYRIGHT ************************************************
*                                                                 *
*                         vtk-flowvr                              *
*          Distributed rendering for VTK using FlowVR Render      *
*                                                                 *
*-----------------------------------------------------------      *
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
* ALL RIGHTS RESERVED.                                            *
*                                                                 *
* This source is covered by the GNU GPL, please refer to the      *
* COPYING file for further information.                           *
*                                                                 *
*----------------------------------------------------------       *
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Thomas Arcila.                                               *
*                                                                 *
*******************************************************************
*                                                                                                          *
* File: .//FlowVR/vtkFlowVRModule.h                                                  *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
// .NAME vtkFlowVRModule - FlowVRModule class for vtk
// .SECTION Description
// None.

#ifndef __vtkFlowVRModule_h
#define __vtkFlowVRModule_h

#include <vtkObject.h>
#include <flowvr/moduleapi.h>
#include <vector>

class vtkRenderWindow;
class vtkRenderWindowInteractor;

class VTK_COMMON_EXPORT vtkFlowVRModule : public vtkObject
{
public:
  static vtkFlowVRModule *New();
  vtkTypeRevisionMacro(vtkFlowVRModule,vtkObject);

  static vtkFlowVRModule *DefaultModule;

  //BTX
  void AddInputPort(flowvr::InputPort* port);
  void AddOutputPort(flowvr::OutputPort* port);
  flowvr::ModuleAPI* GetModule() { return Module; }
  //ETX

  vtkSetStringMacro(Suffix);
  vtkGetStringMacro(Suffix);

  void Initialize();

  int GetStatus();

  int Wait();

  void Start();

  void Start(vtkRenderWindow* win);

protected:
  vtkFlowVRModule();
  ~vtkFlowVRModule();

  //BTX
  flowvr::ModuleAPI* Module;
  std::vector<flowvr::Port*> Ports;
  //ETX

  char* Suffix;

private:
  vtkFlowVRModule(const vtkFlowVRModule&);  // Not implemented.
  void operator=(const vtkFlowVRModule&);  // Not implemented.
};

#endif 
