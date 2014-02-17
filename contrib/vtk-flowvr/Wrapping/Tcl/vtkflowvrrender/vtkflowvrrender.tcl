package require vtk

if {[info commands vtkFlowVRRenderWindow] != "" ||
    [::vtk::load_component vtkFlowVRRenderTCL] == ""} {
    package require vtkflowvr 1.0
    package provide vtkflowvrrender 1.0
    vtkFlowVRRenderFactory factory
      factory Register      
}

