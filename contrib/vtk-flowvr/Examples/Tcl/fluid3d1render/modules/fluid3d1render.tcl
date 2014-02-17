package require vtk
package require vtkflowvr
package require vtkflowvrrender

vtkFlowVRModule module

set NX 66
set NY 66
set NZ 34

set X0 0
set Y0 0
set Z0 0
set X1 [expr $NX-1]
set Y1 [expr $NY-1]
set Z1 [expr $NZ-1]

puts $argv

set INDEX [expr $env(FLOWVR_RANK)]
set NBPROC [expr $env(FLOWVR_NBPROC)]
set NCOLS [lindex $argv 0]
set NROWS [lindex $argv 1]

set COL [expr $INDEX / $NROWS]
set ROW [expr $INDEX % $NROWS]

set x0 [expr $X0 + (( $COL   *($X1-$X0+2))/$NCOLS)]
set x1 [expr $X0 + ((($COL+1)*($X1-$Y0+2))/$NCOLS)]
set y0 [expr $Y0 + (( $ROW   *($Y1-$Y0+2))/$NROWS)]
set y1 [expr $Y0 + ((($ROW+1)*($Y1-$Y0+2))/$NROWS)]
set z0 $Z0
set z1 $Z1

vtkFlowVRImageReader img0
  img0 SetPortName in
#  img0 SetStampSizesName N
  img0 SetInputComponents 1
  img0 SetOutputComponents 1
  img0 SetOutputDimensions $NX $NY $NZ
#  img0 DebugOn

vtkExtractVOI imgpart
  imgpart SetInput [img0 GetOutput]
  imgpart SetVOI $x0 $x1 $y0 $y1 $z0 $z1
#  imgpart SetSampleRate 2 2 2

vtkMarchingCubes iso
  iso SetInput [imgpart GetOutput]
  iso SetValue 0 64

#vtkPolyDataNormals normals
#   normals SetInput [iso GetOutput]
#   normals SetFeatureAngle 45

vtkPolyDataMapper isoMapper
   isoMapper SetInput [iso GetOutput]
   isoMapper ScalarVisibilityOff

vtkActor isoActor
   isoActor SetMapper isoMapper
#   eval [isoActor GetProperty] SetColor 0 1 1
set tc(0) {0 1 1}
set tc(1) {0 0 1}
set tc(2) {1 0 1}
set tc(3) {1 0 0}
set tc(4) {1 1 0}
set tc(5) {0 1 0}
set tc(6) {0.5 0 1}
set tc(7) {0 1 0.5}
   eval [isoActor GetProperty] SetColor [lindex $tc($INDEX) 0]  [lindex $tc($INDEX) 1]  [lindex $tc($INDEX) 2]
   isoActor SetPosition $x0 $y0 $z0

vtkOutlineFilter outline
    outline SetInput [img0 GetOutput]
vtkPolyDataMapper outlineMapper
    outlineMapper SetInput [outline GetOutput]
vtkActor outlineActor
    outlineActor SetMapper outlineMapper
  eval [outlineActor GetProperty] SetColor 1 1 1

vtkRenderer ren
  ren SetBackground 1.0 1.0 1.0

vtkRenderWindow renWin
  renWin SetSize 1024 768
  renWin AddRenderer ren



#isoActor RotateZ -90
#isoActor RotateY -60
#outlineActor RotateZ -90
#outlineActor RotateY -60

ren AddProp isoActor
ren AddProp outlineActor

module Initialize

module Wait

renWin Render

[ren GetActiveCamera] Roll -90
[ren GetActiveCamera] Elevation -60
[ren GetActiveCamera] Zoom 1.75

#module Start renWin
#loop it O 910 {
for {set x 0} {$x<910} {incr x} {
  module Wait
  renWin Render
}

puts "module loop ended."

exit 0

# prevent the tk window from showing up then start the event loop
wm withdraw .
