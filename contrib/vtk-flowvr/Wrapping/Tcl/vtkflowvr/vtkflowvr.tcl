package require vtk

if {[info commands vtkFlowVRModule] != "" ||
    [::vtk::load_component vtkFlowVRTCL] == ""} {
    package provide vtkflowvr 1.0
}

