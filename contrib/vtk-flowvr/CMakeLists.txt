PROJECT (vtkflowvr)

SET(FlowVRVTK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${FlowVR_DIR} ${FlowVRRender_DIR})


FIND_PACKAGE(FlowVR)
FIND_PACKAGE(FlowVRRender)
FIND_PACKAGE(VTK)

SET( LIB_DIR lib/vtk )

# Test if VTK support TCL wrapping (required for running the examples)
SET(VTK_TCL_WRAP_FOUND FALSE)
SET(VTK_MESSAGE "Required with  support for TcL wrapping to run the examples)")

IF (VTK_FOUND)
  IF(VTK_LANGUAGES MATCHES "TCL")
       SET(VTK_TCL_WRAP_FOUND TRUE)
  ELSE(VTK_LANGUAGES MATCHES "TCL")
       SET(VTK_TCL_WRAP_FOUND FALSE)
       SET(VTK_MESSAGE "Required. VTK found but without support for TcL wrapping: examples cannot  be executed")
  ENDIF(VTK_LANGUAGES MATCHES "TCL")
ENDIF (VTK_FOUND)

IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})


  INCLUDE_DIRECTORIES (${FLOWVR_INCLUDE_DIR} ${vtkflowvr_SOURCE_DIR}/FlowVR
    ${FLOWVRRENDER_INCLUDE_DIR} ${vtkflowvr_SOURCE_DIR}/FlowVRRender)
    
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${ContribName}/flowvr-render/include)

  
#  MESSAGE(" configure VTK-CONFIG ")
#  FOREACH(config vtk-flowvr-config.sh )
#    SET(INSTALL_PREFIX ${CMAKE_BINARY_DIR})
#    CONFIGURE_FILE(${config}.in ${CMAKE_BINARY_DIR}/bin/${config} @ONLY)
#   SET(install_prefix ${cmake_install_prefix})
#   CONFIGURE_FILE(${config}.in ${vtkflowvr_BINARY_DIR}/${config}.dist @ONLY)
#   INSTALL(PROGRAMS  ${vtkflowvr_BINARY_DIR}/${config}.dist 
#     DESTINATION bin
#     RENAME ${config})
# ENDFOREACH(config)
  
  #SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
#  SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
#  MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
  #
  # Wrap Tcl, Java, Python
  #
  # Rational: even if your VTK was wrapped, it does not mean that you want to 
  # wrap your own local classes. 
  # Default value is OFF as the VTK cache might have set them to ON but 
  # the wrappers might not be present (or yet not found).
  #
  
  # Tcl
  IF(VTK_LANGUAGES MATCHES "TCL")
    OPTION(VTKFLOWVR_WRAP_TCL "Wrap classes into the TCL interpreted language." ON)
    #  INCLUDE("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
  ELSE(VTK_LANGUAGES MATCHES "TCL")
    IF (VTKFLOWVR_WRAP_TCL)
      MESSAGE("Warning. VTKFLOWVR_WRAP_TCL is ON but the VTK version you have "
	"chosen has not support for Tcl (VTK_WRAP_TCL is OFF).  "
	"Please set VTKFLOWVR_WRAP_TCL to OFF.")
      SET (VTKFLOWVR_WRAP_TCL OFF)
    ENDIF (VTKFLOWVR_WRAP_TCL)
  ENDIF(VTK_LANGUAGES MATCHES "TCL")
  
  # Python
  IF (VTK_LANGUAGES MATCHES "PYTHON")
    OPTION(VTKFLOWVR_WRAP_PYTHON
      "Wrap classes into the Python interpreted language." ON)
    #  INCLUDE("${VTK_CMAKE_DIR}/vtkWrapPython.cmake")
    IF (VTKFLOWVR_WRAP_PYTHON)
      IF (WIN32)
	IF (NOT BUILD_SHARED_LIBS)
	  MESSAGE("Error. Python support requires BUILD_SHARED_LIBS to be ON.")
	ENDIF (NOT BUILD_SHARED_LIBS)  
      ENDIF (WIN32)
    ENDIF (VTKFLOWVR_WRAP_PYTHON)
  ELSE (VTK_LANGUAGES MATCHES "PYTHON")
    IF (VTKFLOWVR_WRAP_PYTHON)
      MESSAGE("Warning. VTKFLOWVR_WRAP_PYTHON is ON but the VTK version you have "
        "chosen has not support for Python (VTK_WRAP_PYTHON is OFF).  "
        "Please set VTKFLOWVR_WRAP_PYTHON to OFF.")
      SET (VTKFLOWVR_WRAP_PYTHON OFF)
    ENDIF (VTKFLOWVR_WRAP_PYTHON)
  ENDIF (VTK_LANGUAGES MATCHES "PYTHON")
  
  # Java
  #IF (VTK_LANGUAGES MATCHES "JAVA")
  #  OPTION(VTKFLOWVR_WRAP_JAVA "Wrap classes into the Java interpreted language."  ON)
  #  #  INCLUDE("${VTK_CMAKE_DIR}/vtkWrapJava.cmake")
  #  IF (VTKFLOWVR_WRAP_JAVA)
  #    IF (WIN32)
#	IF (NOT BUILD_SHARED_LIBS)
#	  MESSAGE("Error. Java support requires BUILD_SHARED_LIBS to be ON.")
#	ENDIF (NOT BUILD_SHARED_LIBS)  
#      ENDIF (WIN32)
      
#      # Tell the java wrappers where to go.
#      SET(VTK_JAVA_HOME ${vtkflowvr_BINARY_DIR}/java/vtkflowvr)
#      MAKE_DIRECTORY(${VTK_JAVA_HOME})
#    ENDIF (VTKFLOWVR_WRAP_JAVA)
#  ELSE (VTK_LANGUAGES MATCHES "JAVA")
#    IF (VTKFLOWVR_WRAP_JAVA)
#      MESSAGE("Warning. VTKFLOWVR_WRAP_JAVA is ON but the VTK version you have "
#        "chosen has not support for Java (VTK_WRAP_JAVA is OFF).  "
#        "Please set VTKFLOWVR_WRAP_JAVA to OFF.")
#      SET (VTKFLOWVR_WRAP_JAVA OFF)
#    ENDIF (VTKFLOWVR_WRAP_JAVA)
#  ENDIF (VTK_LANGUAGES MATCHES "JAVA")
  
  
  ADD_SUBDIRECTORY(FlowVR)
  ADD_SUBDIRECTORY(FlowVRRender)
  ADD_SUBDIRECTORY(Wrapping)
  ADD_SUBDIRECTORY(Tests)
  ADD_SUBDIRECTORY(data)
  ADD_SUBDIRECTORY(xml)
  ADD_SUBDIRECTORY(Examples)
  ADD_SUBDIRECTORY(vtkflowvr)

  # Demo Script 
 # MACRO(ADD_DEMO script)
 #   SET(INSTALL_PREFIX ${CMAKE_BINARY_DIR})
 #   CONFIGURE_FILE(${script}.in ${CMAKE_BINARY_DIR}/bin/${script} @ONLY)
 #  SET(INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
 #  CONFIGURE_FILE(${script}.in ${vtkflowvr_BINARY_DIR}/${script}.dist @ONLY)
 #   EXEC_PROGRAM(chmod ${CMAKE_BINARY_DIR}/bin ARGS 755 ${script} OUTPUT_VARIABLE OUTPUT RETURN_VALUE return)
 #   IF(return)
 #     MESSAGE("Error while setting executable attribute for ${script}. Only installed version will be usable.")
 #  ENDIF(return)
 #  INSTALL(PROGRAMS ${vtkflowvr_BINARY_DIR}/${script}.dist
 #          DESTINATION bin
 #          RENAME ${script})
 # ENDMACRO(ADD_DEMO)
  
  #ADD_DEMO(vtk-flowvr-demo.sh)
  
ENDIF(VTK_FOUND)


# To make a clean FindFlowVRVTK.cmake, we strip TCL/python/java, if wrapping disabled
IF (VTKFLOWVR_WRAP_TCL) 
  SET(FLOWVR_VTK_TCL_LIBRARY "${CMAKE_INSTALL_PREFIX}/lib/vtk/libvtkFlowVRTCL${CMAKE_SHARED_LIBRARY_SUFFIX}")
	SET(FLOWVR_VTKRENDER_TCL_LIBRARY  "${CMAKE_INSTALL_PREFIX}/lib/vtk/libvtkFlowVRRenderTCL${CMAKE_SHARED_LIBRARY_SUFFIX}")
ELSE (VTKFLOWVR_WRAP_TCL) 
  SET(FLOWVR_VTK_TCL_LIBRARY "\"\"")
	SET(FLOWVR_VTKRENDER_TCL_LIBRARY "\"\"") 
ENDIF (VTKFLOWVR_WRAP_TCL) 
  
IF (VTKFLOWVR_WRAP_PYTHON) 
	SET(FLOWVR_VTK_PYTHON_LIBRARY  "${CMAKE_INSTALL_PREFIX}/lib/vtk/libvtkFlowVRPython${CMAKE_SHARED_LIBRARY_SUFFIX}")
	SET(FLOWVR_VTKRENDER_PYTHON_LIBRARY "${CMAKE_INSTALL_PREFIX}/lib/vtk/libvtkFlowVRRenderPython${CMAKE_SHARED_LIBRARY_SUFFIX}")
ELSE (VTKFLOWVR_WRAP_PYTHON) 
  SET(FLOWVR_VTK_PYTHON_LIBRARY "\"\"")
	SET(FLOWVR_VTKRENDER_PYTHON_LIBRARY "\"\"") 
ENDIF (VTKFLOWVR_WRAP_PYTHON) 

  
# Doxygen documentation
FIND_PACKAGE(Doxygen)
IF(DOXYGEN)
  IF(DOT)
    SET(HAVE_DOT "YES")
    SET(CLASS_DIAGRAMS "NO")
  ELSE(DOT)
    SET(HAVE_DOT "NO")
    SET(CLASS_DIAGRAMS "YES")
  ENDIF(DOT)
  
  CONFIGURE_FILE(Doxyfile.in ${vtkflowvr_BINARY_DIR}/Doxyfile)
  ADD_CUSTOM_TARGET(doc-vtk-flowvr COMMAND ${DOXYGEN})
ENDIF(DOXYGEN)

CONFIGURE_FILE(cmake/FindFlowVRVTK.cmake.in  ${CMAKE_BINARY_DIR}/share/vtk-flowvr/cmake/FindFlowVRVTK.cmake @ONLY)
INSTALL( FILES ${CMAKE_BINARY_DIR}/share/vtk-flowvr/cmake/FindFlowVRVTK.cmake DESTINATION share/vtk-flowvr/cmake )

