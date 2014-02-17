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
* File: .//FlowVR/vtkFlowVRImageReader.h                                             *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
// .NAME vtkFlowVRImageReader - FlowVRImageReader class for vtk
// .SECTION Description
// None.

#ifndef __vtkFlowVRImageReader_h
#define __vtkFlowVRImageReader_h

#include "vtkImageSource.h"

#include <flowvr/module.h>

class vtkFlowVRModule;

class VTK_COMMON_EXPORT vtkFlowVRImageReader : public vtkImageSource
{
public:
  static vtkFlowVRImageReader *New();
  vtkTypeRevisionMacro(vtkFlowVRImageReader,vtkImageSource);
  
  // Description:
  // Set what type of scalar data this source should generate.
  vtkSetMacro(OutputScalarType,int);
  vtkGetMacro(OutputScalarType,int);
  void SetOutputScalarTypeToFloat()
    {this->SetOutputScalarType(VTK_FLOAT);}
  void SetOutputScalarTypeToDouble()
    {this->SetOutputScalarType(VTK_DOUBLE);}
  void SetOutputScalarTypeToLong()
    {this->SetOutputScalarType(VTK_LONG);}
  void SetOutputScalarTypeToUnsignedLong()
    {this->SetOutputScalarType(VTK_UNSIGNED_LONG);};
  void SetOutputScalarTypeToInt()
    {this->SetOutputScalarType(VTK_INT);}
  void SetOutputScalarTypeToUnsignedInt()
    {this->SetOutputScalarType(VTK_UNSIGNED_INT);}
  void SetOutputScalarTypeToShort()
    {this->SetOutputScalarType(VTK_SHORT);}
  void SetOutputScalarTypeToUnsignedShort()
    {this->SetOutputScalarType(VTK_UNSIGNED_SHORT);}
  void SetOutputScalarTypeToChar()
    {this->SetOutputScalarType(VTK_CHAR);}
  void SetOutputScalarTypeToUnsignedChar()
    {this->SetOutputScalarType(VTK_UNSIGNED_CHAR);}

  // Description:
  // Set/Get the number of components this source should generate
  vtkSetMacro(OutputComponents, int);
  vtkGetMacro(OutputComponents, int);
  
  // Description:
  // Set what type of scalar data this source should read
  vtkSetMacro(InputScalarType,int);
  vtkGetMacro(InputScalarType,int);
  void SetInputScalarTypeToFloat()
    {this->SetInputScalarType(VTK_FLOAT);}
  void SetInputScalarTypeToDouble()
    {this->SetInputScalarType(VTK_DOUBLE);}
  void SetInputScalarTypeToLong()
    {this->SetInputScalarType(VTK_LONG);}
  void SetInputScalarTypeToUnsignedLong()
    {this->SetInputScalarType(VTK_UNSIGNED_LONG);};
  void SetInputScalarTypeToInt()
    {this->SetInputScalarType(VTK_INT);}
  void SetInputScalarTypeToUnsignedInt()
    {this->SetInputScalarType(VTK_UNSIGNED_INT);}
  void SetInputScalarTypeToShort()
    {this->SetInputScalarType(VTK_SHORT);}
  void SetInputScalarTypeToUnsignedShort()
    {this->SetInputScalarType(VTK_UNSIGNED_SHORT);}
  void SetInputScalarTypeToChar()
    {this->SetInputScalarType(VTK_CHAR);}
  void SetInputScalarTypeToUnsignedChar()
    {this->SetInputScalarType(VTK_UNSIGNED_CHAR);}

  // Description:
  // Set/Get the number of components this source should read
  vtkSetMacro(InputComponents, int);
  vtkGetMacro(InputComponents, int);
  
  // Description:
  // Set what type of scalar data this source should read and generate
  void SetScalarType(int t)
  { this->SetInputScalarType(t); this->SetOutputScalarType(t); }
  void SetScalarTypeToFloat()
    {this->SetScalarType(VTK_FLOAT);}
  void SetScalarTypeToDouble()
    {this->SetScalarType(VTK_DOUBLE);}
  void SetScalarTypeToLong()
    {this->SetScalarType(VTK_LONG);}
  void SetScalarTypeToUnsignedLong()
    {this->SetScalarType(VTK_UNSIGNED_LONG);};
  void SetScalarTypeToInt()
    {this->SetScalarType(VTK_INT);}
  void SetScalarTypeToUnsignedInt()
    {this->SetScalarType(VTK_UNSIGNED_INT);}
  void SetScalarTypeToShort()
    {this->SetScalarType(VTK_SHORT);}
  void SetScalarTypeToUnsignedShort()
    {this->SetScalarType(VTK_UNSIGNED_SHORT);}
  void SetScalarTypeToChar()
    {this->SetScalarType(VTK_CHAR);}
  void SetScalarTypeToUnsignedChar()
    {this->SetScalarType(VTK_UNSIGNED_CHAR);}

  // Description:
  // Set/Get the number of components this source should read
  void SetComponents(int n)
  { this->SetInputComponents(n); this->SetOutputComponents(n); }

  // Description:
  // Set/Get the dimensions of the image this source should generate
  vtkSetVector3Macro(OutputDimensions, int);
  vtkGetVector3Macro(OutputDimensions, int);

  // Description:
  // Set/Get FlowVR's port name
  void SetPortName(const char* name);
  const char* GetPortName();

  // Description:
  // Set/Get FlowVR's size stamp name
  void SetStampSizesName(const char* name);
  const char* GetStampSizesName();

  vtkFlowVRModule* GetModule() { return Module; }

  void Read();

protected:
  vtkFlowVRImageReader();
  ~vtkFlowVRImageReader();

  int InputComponents;
  int InputScalarType;

  int OutputDimensions[3];
  int OutputComponents;
  int OutputScalarType;

  vtkFlowVRModule* Module;
  int ModuleObserverTag;

  //BTX  
  flowvr::InputPort Port;
  flowvr::Message LastMessage;
  flowvr::StampInfo StampSizes;
  //ETX

  virtual void ExecuteInformation();
  virtual void ExecuteData(vtkDataObject *outData);

private:
  vtkFlowVRImageReader(const vtkFlowVRImageReader&);  // Not implemented.
  void operator=(const vtkFlowVRImageReader&);  // Not implemented.
};

#endif 
