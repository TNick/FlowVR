#
# Try to find VTK and include its settings (otherwise complain)
#

INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)

IF (USE_VTK_FILE)
  INCLUDE (${USE_VTK_FILE})
ELSE (USE_VTK_FILE)
  SET (VTKFLOWVR_CAN_BUILD 0)
ENDIF (USE_VTK_FILE)

#
# Build shared libs ?
#
# Defaults to the same VTK setting.
#

IF (USE_VTK_FILE)

  # Standard CMake option for building libraries shared or static by default.
  OPTION(BUILD_SHARED_LIBS 
         "Build with shared libraries." 
         ${VTK_BUILD_SHARED_LIBS})
  # Copy the CMake option to a setting with VTKFLOWVR_ prefix for use in
  # our project.  This name is used in vtkflowvrConfigure.h.in.
  SET(VTKFLOWVR_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

  # If this is a build tree, provide an option for putting
  # this project's executables and libraries in with VTK's.
  IF (EXISTS ${VTK_DIR}/bin)
    OPTION(USE_VTK_OUTPUT_PATHS
           "Use VTK's output directory for this project's executables and libraries."
           OFF)
    MARK_AS_ADVANCED (USE_VTK_OUTPUT_PATHS)
    IF (USE_VTK_OUTPUT_PATHS)
      SET (LIBRARY_OUTPUT_PATH ${VTK_DIR}/bin)
      SET (EXECUTABLE_OUTPUT_PATH ${VTK_DIR}/bin)
    ENDIF (USE_VTK_OUTPUT_PATHS)
  ENDIF (EXISTS ${VTK_DIR}/bin)

ENDIF (USE_VTK_FILE)


#
# Wrap Tcl, Java, Python
#
# Rational: even if your VTK was wrapped, it does not mean that you want to 
# wrap your own local classes. 
# Default value is OFF as the VTK cache might have set them to ON but 
# the wrappers might not be present (or yet not found).
#

#
# Tcl
# 

IF (VTK_WRAP_TCL)

  OPTION(VTKFLOWVR_WRAP_TCL 
         "Wrap classes into the TCL interpreted language." 
         ON)

ELSE (VTK_WRAP_TCL)

  IF (VTKFLOWVR_WRAP_TCL)
    MESSAGE("Warning. VTKFLOWVR_WRAP_TCL is ON but the VTK version you have "
            "chosen has not support for Tcl (VTK_WRAP_TCL is OFF).  "
            "Please set VTKFLOWVR_WRAP_TCL to OFF.")
    SET (VTKFLOWVR_WRAP_TCL OFF)
  ENDIF (VTKFLOWVR_WRAP_TCL)

ENDIF (VTK_WRAP_TCL)

#
# Python
# 

IF (VTK_WRAP_PYTHON)

  OPTION(VTKFLOWVR_WRAP_PYTHON 
         "Wrap classes into the Python interpreted language." 
         ON)

  IF (VTKFLOWVR_WRAP_PYTHON)
    IF (WIN32)
      IF (NOT BUILD_SHARED_LIBS)
        MESSAGE("Error. Python support requires BUILD_SHARED_LIBS to be ON.")
        SET (VTKFLOWVR_CAN_BUILD 0)
      ENDIF (NOT BUILD_SHARED_LIBS)  
    ENDIF (WIN32)
  ENDIF (VTKFLOWVR_WRAP_PYTHON)

ELSE (VTK_WRAP_PYTHON)

  IF (VTKFLOWVR_WRAP_PYTHON)
    MESSAGE("Warning. VTKFLOWVR_WRAP_PYTHON is ON but the VTK version you have "
            "chosen has not support for Python (VTK_WRAP_PYTHON is OFF).  "
            "Please set VTKFLOWVR_WRAP_PYTHON to OFF.")
    SET (VTKFLOWVR_WRAP_PYTHON OFF)
  ENDIF (VTKFLOWVR_WRAP_PYTHON)

ENDIF (VTK_WRAP_PYTHON)

#
# Java
# 

IF (VTK_WRAP_JAVA)

  OPTION(VTKFLOWVR_WRAP_JAVA 
         "Wrap classes into the Java interpreted language." 
         ON)

  IF (VTKFLOWVR_WRAP_JAVA)
    IF (WIN32)
      IF (NOT BUILD_SHARED_LIBS)
        MESSAGE("Error. Java support requires BUILD_SHARED_LIBS to be ON.")
        SET (VTKFLOWVR_CAN_BUILD 0)
      ENDIF (NOT BUILD_SHARED_LIBS)  
    ENDIF (WIN32)

    # Tell the java wrappers where to go.
    SET(VTK_JAVA_HOME ${VTKFLOWVR_BINARY_DIR}/java/vtkflowvr)
    MAKE_DIRECTORY(${VTK_JAVA_HOME})
  ENDIF (VTKFLOWVR_WRAP_JAVA)

ELSE (VTK_WRAP_JAVA)

  IF (VTKFLOWVR_WRAP_JAVA)
    MESSAGE("Warning. VTKFLOWVR_WRAP_JAVA is ON but the VTK version you have "
            "chosen has not support for Java (VTK_WRAP_JAVA is OFF).  "
            "Please set VTKFLOWVR_WRAP_JAVA to OFF.")
    SET (VTKFLOWVR_WRAP_JAVA OFF)
  ENDIF (VTKFLOWVR_WRAP_JAVA)

ENDIF (VTK_WRAP_JAVA)

# Setup our local hints file in case wrappers need them.
SET(VTK_WRAP_HINTS ${VTKFLOWVR_SOURCE_DIR}/Wrapping/hints)
