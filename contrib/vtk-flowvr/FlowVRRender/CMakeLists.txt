SET (vtkFlowVRRender-SRCS vtkFlowVRRenderWindow.cxx vtkFlowVRRenderer.cxx
     vtkFlowVRActor.cxx vtkFlowVRImageActor.cxx vtkFlowVRTexture.cxx
     vtkFlowVRPolyDataMapper.cxx vtkFlowVRVolumeMapper.cxx
     vtkFlowVRRenderFactory.cxx vtkFlowVRRenderWindowInteractor.cxx 
     vtkFlowVRProperty.cxx )


#
# Create the vtkFlowVRRender C++ library
#

ADD_LIBRARY (vtkFlowVRRender SHARED ${vtkFlowVRRender-SRCS})
ADD_DEPENDENCIES(vtkFlowVRRender ${FLOWVRRENDER_LIBRARY})
TARGET_LINK_LIBRARIES(vtkFlowVRRender vtkFlowVR vtkRendering vtkVolumeRendering ${FLOWVR_ftl_LIBRARY} ${FLOWVRRENDER_LIBRARY})
#SET_TARGET_PROPERTIES(vtkFlowVRRender PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION})
INSTALL(TARGETS vtkFlowVRRender DESTINATION ${LIB_DIR})
FILE(GLOB vtkFlowVRRender-HEADERS *.h)
INSTALL(FILES ${vtkFlowVRRender-HEADERS} DESTINATION include/vtk-flowvr)

#
# Create the vtkFlowVRRender Tcl library
#
IF(VTK_LANGUAGES MATCHES "TCL" AND VTKFLOWVR_WRAP_TCL)
  INCLUDE(${VTK_CMAKE_DIR}/vtkWrapTcl.cmake)
  VTK_WRAP_TCL3(vtkFlowVRRenderTCL vtkFlowVRRenderTCL-SRCS "${vtkFlowVRRender-SRCS}"  "")

  INCLUDE(FindTCL)
  MESSAGE(STATUS ${TCL_INCLUDE_PATH})
  INCLUDE_DIRECTORIES(${TCL_INCLUDE_PATH})

  ADD_LIBRARY(vtkFlowVRRenderTCL SHARED ${vtkFlowVRRenderTCL-SRCS})
  TARGET_LINK_LIBRARIES (vtkFlowVRRenderTCL vtkFlowVRTCL vtkFlowVRRender vtkRenderingTCL vtkVolumeRenderingTCL)
  SET_SOURCE_FILES_PROPERTIES(${vtkFlowVRRenderTCL-SRCS} PROPERTIES GENERATED 1)
  INSTALL(TARGETS vtkFlowVRRenderTCL DESTINATION ${LIB_DIR})
ENDIF(VTK_LANGUAGES MATCHES "TCL" AND VTKFLOWVR_WRAP_TCL)

#
# Create the vtkFlowVRRender Python library. 
#
IF (VTK_LANGUAGES MATCHES "PYTHON" AND VTKFLOWVR_WRAP_PYTHON)
  INCLUDE(${VTK_CMAKE_DIR}/vtkWrapPython.cmake)
  VTK_WRAP_PYTHON3 (vtkFlowVRRenderPython vtkFlowVRRenderPython-SRCS "${vtkFlowVRRender-SRCS}" "")

	INCLUDE(FindPythonLibs)
	MESSAGE(STATUS ${PYTHON_INCLUDE_PATH})
	INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

  ADD_LIBRARY (vtkFlowVRRenderPython SHARED ${vtkFlowVRRenderPython-SRCS})
  TARGET_LINK_LIBRARIES (vtkFlowVRRenderPython vtkFlowVRPython vtkFlowVRRender vtkRenderingPythonD vtkVolumeRenderingPythonD)
  SET_SOURCE_FILES_PROPERTIES(${vtkFlowVRRenderPython-SRCS} PROPERTIES GENERATED 1)
  TARGET_LINK_LIBRARIES (vtkFlowVRRenderPython vtkFlowVRRender vtkCommonPythonD)
  INSTALL(TARGETS vtkFlowVRRenderPython DESTINATION ${LIB_DIR})
ENDIF (VTK_LANGUAGES MATCHES "PYTHON" AND VTKFLOWVR_WRAP_PYTHON)

#
# Create the vtkFlowVRRender Java library. 
#
#IF (VTK_LANGUAGES MATCHES "JAVA" AND VTKFLOWVR_WRAP_JAVA)
#  INCLUDE(${VTK_CMAKE_DIR}/vtkWrapJava.cmake)
#  VTK_WRAP_JAVA3(vtkFlowVRRenderJava vtkFlowVRRenderJava-SRCS "${vtkFlowVRRender-SRCS}" "")
#  ADD_LIBRARY(vtkFlowVRRenderJava SHARED ${vtkFlowVRRenderJava-SRCS})
#  TARGET_LINK_LIBRARIES(vtkFlowVRRenderJava vtkFlowVRJava vtkFlowVRRender vtkRenderingJava vtkVolumeRenderingJava)
# SET_SOURCE_FILES_PROPERTIES(${vtkFlowVRRenderJava-SRCS} PROPERTIES GENERATED 1)
# INSTALL(TARGETS vtkFlowVRRenderJava DESTINATION ${LIB_DIR})
#ENDIF (VTK_LANGUAGES MATCHES "JAVA" AND VTKFLOWVR_WRAP_JAVA)
