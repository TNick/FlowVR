cmake_minimum_required(VERSION 2.8)
# For uninstalled version
IF(NOT EXISTS ${CMAKE_BINARY_DIR}/share/vtk-flowvr/Examples)
  FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/vtk-flowvr/Examples)
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/Tcl ${CMAKE_BINARY_DIR}/share/vtk-flowvr/Examples/Tcl OUTPUT_VARIABLE output ERROR_VARIABLE output RESULT_VARIABLE result)
  
  IF(result)
    MESSAGE(STATUS "Failed symlinking VTKFlowVR Examples. making a copy...")
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/share/vtk-flowvr/Examples OUTPUT_VARIABLE output ERROR_VARIABLE output RESULT_VARIABLE result)
  ENDIF(result)
  
  IF(result)
    MESSAGE(SEND_ERROR "VTKFlowVR Examples cannot be used in uninstalled version. Only installed version will work.")
  ENDIF(result)
ENDIF(NOT EXISTS ${CMAKE_BINARY_DIR}/share/vtk-flowvr/Examples)

# For installed version
INSTALL(DIRECTORY  Tcl   DESTINATION  share/vtk-flowvr/Examples/ PATTERN ".svn" EXCLUDE )

