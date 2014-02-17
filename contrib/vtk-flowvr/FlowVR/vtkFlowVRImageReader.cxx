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
* File: .//FlowVR/vtkFlowVRImageReader.cxx                                           *
*                                                                                                          *
* Contacts:                                                                                          *
*                                                                                                          *
******************************************************************/
#include "vtkFlowVRImageReader.h"
#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkFlowVRModule.h"

#include <flowvr/module.h>

#include <string.h>

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFlowVRImageReader, "$Revision: 5485 $");
vtkStandardNewMacro(vtkFlowVRImageReader);

std::string generatePortName()
{
  static int nport = 0;
  char buf[16];
  snprintf(buf,sizeof(buf),"in%d",nport);
  ++nport;
  return buf;
}

void vtkFlowVRImageReaderWaitCallback(vtkObject* source, unsigned long eid, void* clientdata, void* calldata)
{
  ((vtkFlowVRImageReader*) clientdata)->Read();
}

vtkFlowVRImageReader::vtkFlowVRImageReader()
: Port(generatePortName()), StampSizes("Sizes", flowvr::TypeArray::create(3,flowvr::TypeInt::create()))
{
  Port.stamps->add(&StampSizes);
  Module = vtkFlowVRModule::DefaultModule;
  if (Module != NULL)
  {
    Module->Register(this);
    Module->AddInputPort(&Port);
    vtkCallbackCommand* cmd = vtkCallbackCommand::New();
      cmd->SetClientData(this);
      cmd->SetCallback(vtkFlowVRImageReaderWaitCallback);
    ModuleObserverTag = Module->AddObserver(vtkCommand::LeaveEvent, cmd);
    cmd->Delete();
  }

  OutputDimensions[0] = 1;
  OutputDimensions[1] = 1;
  OutputDimensions[2] = 1;
  OutputComponents = 1;
  OutputScalarType = VTK_UNSIGNED_CHAR;
  InputComponents = 1;
  InputScalarType = VTK_UNSIGNED_CHAR;
}

vtkFlowVRImageReader::~vtkFlowVRImageReader()
{
  if (Module != NULL)
  {
    Module->RemoveObserver(ModuleObserverTag);
    Module->UnRegister(this);
    Module = NULL;
  }
}

void vtkFlowVRImageReader::SetPortName(const char* name)
{
  if (name!=NULL)
  {
    this->Port.name = name;
    this->Modified();
  }
}

const char* vtkFlowVRImageReader::GetPortName()
{
  return Port.name.c_str();
}

void vtkFlowVRImageReader::SetStampSizesName(const char* name)
{
  if (name!=NULL)
  {
    this->StampSizes.setName(name);
    this->Modified();
  }
}

const char* vtkFlowVRImageReader::GetStampSizesName()
{
  return StampSizes.getName().c_str();
}

void vtkFlowVRImageReader::ExecuteInformation()
{
  vtkImageData *data = this->GetOutput();
  if (!LastMessage.stamps.empty())
  {
    data->SetSpacing(1.0, 1.0, 1.0);
    int nx=OutputDimensions[0],ny=OutputDimensions[1],nz=OutputDimensions[2];
    if (StampSizes.valid())
    {
      if (StampSizes.valid(0)) LastMessage.stamps.read(StampSizes[0],nx);
      if (StampSizes.valid(1)) LastMessage.stamps.read(StampSizes[1],ny);
      if (StampSizes.valid(2)) LastMessage.stamps.read(StampSizes[2],nz);
      vtkDebugMacro( << "image dims: "<<nx<<','<<ny<<','<<nz);
    }
    data->SetDimensions(nx,ny,nz);
    data->SetExtent(0,nx-1,0,ny-1,0,nz-1);
    data->SetWholeExtent(0,nx-1,0,ny-1,0,nz-1);
    data->SetUpdateExtent(0,nx-1,0,ny-1,0,nz-1);
  }
  data->SetNumberOfScalarComponents(OutputComponents);
  data->SetScalarType(OutputScalarType);
}

template<class IT, class OT>
void convImage(OT* ptr, const IT* src, int n, int inc, int onc)
{
  int i;
  if (inc > onc)
  {
    while (n--)
    {
      for (i=0;i<onc;i++) ptr[i] = (OT)src[i];
      ptr+=onc;
      src+=inc;
    }
  }
  else
  {
    while (n--)
    {
      for (i=0;i<inc;i++) ptr[i] = (OT)src[i];
      for (;i<onc;i++) ptr[i] = (OT)0;
      ptr+=onc;
      src+=inc;
    }
  }
}

template<class OT>
void convImageI(int it, OT* ptr, const void* src, int n, int inc, int onc)
{
  switch (it)
  {
    vtkTemplateMacro5(convImage, ptr, (const VTK_TT*)src, n, inc, onc);
    default:
      vtkGenericWarningMacro("Execute: Unknown input ScalarType");
      return;
  }
}

void vtkFlowVRImageReader::ExecuteData(vtkDataObject *outData)
{
  vtkDebugMacro( << "ExecuteData");

  vtkImageData *data = vtkImageData::SafeDownCast(outData);
  if (!data)
  {
    vtkWarningMacro("Call to ExecuteData with non vtkImageData output");
    return;
  }

  ExecuteInformation();

  data->AllocateScalars();

  //data->SetExtent(data->GetUpdateExtent());
  if (!LastMessage.data.empty())
  {
    if (OutputComponents == InputComponents && OutputScalarType == InputScalarType)
    {
      vtkDataArray* array = data->GetPointData()->GetScalars();
      //vtkDebugMacro( << "Array size = "<<size<<" * "<<psize);
      array->SetVoidArray((void*)LastMessage.data.readAccess(),array->GetSize(),1);
      //memcpy(array->GetVoidPointer(0), LastMessage.data.readAccess(), LastMessage.data.getSize());
    }
    else
    {
      vtkDataArray* array = data->GetPointData()->GetScalars();
      void* ptr = data->GetScalarPointer();
      const void* src = LastMessage.data.readAccess();
      int n = array->GetSize() / OutputComponents;
      switch (OutputScalarType)
      {
	vtkTemplateMacro6(convImageI, InputScalarType, (VTK_TT*)ptr, src, n, InputComponents, OutputComponents);
      default:
	vtkGenericWarningMacro("Execute: Unknown output ScalarType");
	return;
      }
    }
//    array->Modified();
//    data->Modified();
    //vtkDebugMacro( << *data );
  }
  else
  {
    vtkDebugMacro( << "Message data empty!");
  }
}

void vtkFlowVRImageReader::Read()
{
  flowvr::ModuleAPI* module = Port.getModule();
  if (module == NULL) return;
  flowvr::Message m;
  module->get(&Port,m);
  if (!m.valid() || m == LastMessage) return;
  int it=-2, last_it=-2;
  m.stamps.read(Port.stamps->it,it);
  if (LastMessage.valid())
    LastMessage.stamps.read(Port.stamps->it,last_it);
  if (it != last_it)
  {
    vtkDebugMacro( << "New message: "<<it);
    LastMessage = m;
    this->Modified();
    //this->ExecuteData(GetOutput());
  }
}
