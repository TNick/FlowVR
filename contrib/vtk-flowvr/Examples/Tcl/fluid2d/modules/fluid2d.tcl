package require vtk
package require vtkflowvr

vtkFlowVRModule module

vtkFlowVRImageReader img0
  img0 SetPortName density
  img0 SetStampSizesName N
  img0 SetInputComponents 2
  img0 SetOutputComponents 3
#  img0 DebugOn

vtkImageExtractComponents img1
  img1 SetInput [img0 GetOutput]
  img1 SetComponents 0

vtkImageExtractComponents img2
  img2 SetInput [img0 GetOutput]
  img2 SetComponents 1

#vtkImageEllipsoidSource img0

vtkContourFilter contour
  contour SetInput [img1 GetOutput]
  contour GenerateValues 5 10.0 250.0
#  contour DebugOn

vtkContourFilter contour2
  contour2 SetInput [img2 GetOutput]
  contour2 GenerateValues 5 10.0 250.0
#  contour2 DebugOn

vtkImageActor imageActor
  imageActor SetInput [img0 GetOutput]
  imageActor VisibilityOn
#  imageActor DebugOn

vtkPolyDataMapper contourMapper
  contourMapper SetInput [contour GetOutput]
#  contourMapper SetScalarRange 0.0 255.0
  contourMapper ScalarVisibilityOff

vtkActor contourActor
  contourActor SetMapper contourMapper
  contourActor SetPosition 0 0 1
#  contourActor VisibilityOn

vtkPolyDataMapper contour2Mapper
  contour2Mapper SetInput [contour2 GetOutput]
#  contour2Mapper SetScalarRange 0.0 255.0
  contour2Mapper ScalarVisibilityOff

vtkActor contour2Actor
  contour2Actor SetMapper contour2Mapper
  contour2Actor SetPosition 0 0 1

# Create outline an outline of the sampled data.
vtkOutlineFilter outline
  outline SetInput [img0 GetOutput]

vtkPolyDataMapper outlineMapper
  outlineMapper SetInput [outline GetOutput]

vtkActor outlineActor
  outlineActor SetMapper outlineMapper
  eval [outlineActor GetProperty] SetColor 0 0 0

vtkRenderer ren
  ren SetBackground 0.0 0.2 0.4

vtkRenderWindow renWin
  renWin AddRenderer ren
  renWin SetSize 1024 768

ren AddProp contourActor
ren AddProp contour2Actor
ren AddProp outlineActor
ren AddProp imageActor

module Initialize

module Start renWin

# prevent the tk window from showing up then start the event loop
wm withdraw .
