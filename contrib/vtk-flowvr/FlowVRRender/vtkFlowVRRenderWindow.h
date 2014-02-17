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
*                                                                 *
* File: .//FlowVRRender/vtkFlowVRRenderWindow.h                   *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
// .NAME vtkFlowVRRenderWindow - FlowVRRenderWindow class for vtk
// .SECTION Description
// None.

#ifndef __vtkFlowVRRenderWindow_h
#define __vtkFlowVRRenderWindow_h

#include <vtkRenderWindow.h>
#include <flowvr/module.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <flowvr/render/balzac/servicelayer/display.h>
#include <flowvr/render/balzac/servicelayer/glutdata.h>

class vtkFlowVRModule;

class VTK_RENDERING_EXPORT vtkFlowVRRenderWindow : public vtkRenderWindow
{
public:
  static vtkFlowVRRenderWindow *New();
  vtkTypeRevisionMacro(vtkFlowVRRenderWindow,vtkRenderWindow);

  // Description:
  // Set/Get FlowVR's port name
  void SetPortName(const char* name);
  const char* GetPortName();

  //BTX

  // Description:
  // Get FlowVR Port
  flowvr::render::SceneOutputPort* GetOutputPort()
  { return &Port; }

  // Description:
  // Get flowvr-render scene writer
  flowvr::render::ChunkWriter* GetSceneWriter()
  { return &Scene; }

  // Description:
  // Generate a new ID
  flowvr::ID GenerateID();

  //ETX


  // Description:
  // "Deinitialize" the rendering window.  This will shutdown all system-specific
  // resources.  After having called this, it should be possible to destroy
  // a window that was used for a SetWindowId() call without any ill effects.
  virtual void Finalize(void);

  //
  // vtkRenderWindow virtual methods
  //
  
  // Description:
  // Initialize the rendering process.
  virtual void Start();

  // Description:
  // A termination method performed at the end of the rendering process
  // to do things like swapping buffers (if necessary) or similar actions.
  virtual void Frame();

  virtual void HideCursor() {};
  virtual void ShowCursor() {};

  // Description:
  // Turn on/off rendering full screen window size.
  virtual void SetFullScreen(int f) { FullScreen = f; };

  // Description:
  // Remap the rendering window. This probably only works on UNIX right now.
  // It is useful for changing properties that can't normally be changed
  // once the window is up.
  virtual void WindowRemap() {};
  // Description:
  // Set/Get the pixel data of an image, transmitted as RGBRGBRGB. The
  // front argument indicates if the front buffer should be used or the back 
  // buffer. It is the caller's responsibility to delete the resulting 
  // array. It is very important to realize that the memory in this array
  // is organized from the bottom of the window to the top. The origin
  // of the screen is in the lower left corner. The y axis increases as
  // you go up the screen. So the storage of pixels is from left to right
  // and from bottom to top.
  virtual int SetPixelData(int, int, int, int, unsigned char *,int)
  { return 0; };

  virtual int SetPixelData(int, int, int, int, vtkUnsignedCharArray*,
                           int )
  { return 0; };

  virtual unsigned char *GetPixelData(int, int, int, int, int)
  { return NULL; }

  virtual int GetPixelData(int ,int ,int ,int , int,
                           vtkUnsignedCharArray*)
  { return 0; }

  // Description:
  // Same as Get/SetPixelData except that the image also contains an alpha
  // component. The image is transmitted as RGBARGBARGBA... each of which is a
  // float value. The "blend" parameter controls whether the SetRGBAPixelData
  // method blends the data with the previous contents of the frame buffer
  // or completely replaces the frame buffer data.
  virtual float *GetRGBAPixelData(int ,int ,int ,int ,int )
  {return NULL; };

  virtual int GetRGBAPixelData(int, int, int, int, int, vtkFloatArray* )
  {return 0; };

  virtual int SetRGBAPixelData(int ,int ,int ,int ,float *,int,
                               int blend=0)
  {return 0; };

  virtual int SetRGBAPixelData(int, int, int, int, vtkFloatArray*,
                               int, int blend=0)
  {return 0; };

  virtual void ReleaseRGBAPixelData(float *data) {}
  virtual unsigned char *GetRGBACharPixelData(int ,int ,int ,int ,int )
  {return NULL; };

  virtual int GetRGBACharPixelData(int ,int, int, int, int,
                                   vtkUnsignedCharArray*)
  {return 0; };

  virtual int SetRGBACharPixelData(int ,int ,int ,int ,unsigned char *, int,
                                   int blend=0)
  {return 0; };

  virtual int SetRGBACharPixelData(int, int, int, int,
                                   vtkUnsignedCharArray *,
                                   int, int blend=0)
  {return 0; };

  // Description:
  // Set/Get the zbuffer data from the frame buffer.
  virtual float *GetZbufferData(int, int, int, int )
  {return NULL; };

  virtual int GetZbufferData( int, int, int, int, vtkFloatArray*)
  {return 0; };

  virtual int SetZbufferData(int, int, int, int, float *)
  {return 0; };

  virtual int SetZbufferData( int, int, int, int, vtkFloatArray * )
  {return 0; };
  
  // Description:
  // Check to see if a mouse button has been pressed.  All other events
  // are ignored by this method.  Ideally, you want to abort the render
  // on any event which causes the DesiredUpdateRate to switch from
  // a high-quality rate to a more interactive rate.  
  virtual int GetEventPending()
  { return 0; };
  
  // Description:
  // Dummy stubs for vtkWindow API.
  virtual void SetDisplayId(void *) {};
  virtual void SetWindowId(void *)  {};
  virtual void SetNextWindowId(void *) {};
  virtual void SetParentId(void *)  {};
  virtual void *GetGenericDisplayId() { return NULL; };
  virtual void *GetGenericWindowId() { return NULL; };
  virtual void *GetGenericParentId() { return NULL; };
  virtual void *GetGenericContext() { return NULL; };
  virtual void *GetGenericDrawable() { return NULL; };
  virtual void SetWindowInfo(char *) {};
  virtual void SetNextWindowInfo(char *) {};
  virtual void SetParentInfo(char *) {};

  // Description:
  // Make this the current window. 
  virtual void MakeCurrent() {};

  // Description:
  // This method should be defined by the subclass. How many bits of
  // precision are there in the zbuffer?
  virtual int GetDepthBufferSize() { return 24; };

  virtual int GetZbufferData(int, int, int, int, float*) {};
  virtual int* GetScreenSize() { screen_size[1]=screen_size[0]=512;return screen_size;};


  virtual int GetColorBufferSizes(int*){return 0;}
  virtual void CheckGraphicError(){}
  virtual int HasGraphicError(){return 0;}
  virtual const char* GetLastGraphicErrorString(){return NULL;}

  // pur virtual functions in VTK 5.6
  virtual void WaitForCompletion() {}
  virtual bool IsCurrent(){ return true; }


protected:
  int screen_size[2];
  vtkFlowVRRenderWindow();
  ~vtkFlowVRRenderWindow();

  //BTX
  vtkFlowVRModule* Module;
  flowvr::render::SceneOutputPort Port;
  flowvr::render::ChunkWriter Scene;

  //ETX

private:
  vtkFlowVRRenderWindow(const vtkFlowVRRenderWindow&);  // Not implemented.
  void operator=(const vtkFlowVRRenderWindow&);  // Not implemented.
  bool createdByMe;
};

#endif 
