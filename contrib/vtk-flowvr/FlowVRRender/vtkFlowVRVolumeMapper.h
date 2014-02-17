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
* File: .//FlowVRRender/vtkFlowVRVolumeMapper.h                                      *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
// .NAME vtkFlowVRVolumeMapper - volume mapper

// .SECTION Description
// vtkFlowVRVolumeMapper renders a volume using  texture mapping.


// .SECTION see also
// vtkVolumeMapper

#ifndef __vtkFlowVRVolumeMapper_h
#define __vtkFlowVRVolumeMapper_h

#include "vtkVolumeTextureMapper.h"
#include <flowvr/id.h>

class VTK_RENDERING_EXPORT vtkFlowVRVolumeMapper : public vtkVolumeTextureMapper
{
public:
  vtkTypeRevisionMacro(vtkFlowVRVolumeMapper,vtkVolumeTextureMapper);
  void PrintSelf( ostream& os, vtkIndent indent );

  static vtkFlowVRVolumeMapper *New();
  
//BTX

  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS
  // Render the volume
  virtual void Render(vtkRenderer *ren, vtkVolume *vol);

//ETX

  vtkGetMacro(Quality,int);
  vtkSetMacro(Quality,int);

  vtkGetStringMacro(Shader);
  vtkSetStringMacro(Shader);

  vtkGetMacro(Steps,int);
  vtkSetMacro(Steps,int);

protected:
  vtkFlowVRVolumeMapper();
  ~vtkFlowVRVolumeMapper();

  int Quality;

  char* Shader;

  int Steps;
  
//BTX

  flowvr::ID IdPrim;
  flowvr::ID IdTexture;
  flowvr::ID IdTTransfer;
  vtkTimeStamp TimeTexture;
  vtkTimeStamp TimeTTransfer;

  int            Gen;

//ETX

private:
  vtkFlowVRVolumeMapper(const vtkFlowVRVolumeMapper&);  // Not implemented.
  void operator=(const vtkFlowVRVolumeMapper&);  // Not implemented.
};


#endif


