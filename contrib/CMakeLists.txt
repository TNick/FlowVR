PROJECT(FlowVRContrib)

INCLUDE(MacroLogFeature)

# You will find in this directory various contributions to FLowVR

# CHECK FLOWVR DEPENDENCY (other required dependencies checked locally where needed)
FIND_PACKAGE(FlowVR REQUIRED)

SET(FlowVRContrib_DIR ${CMAKE_SOURCE_DIR}/flowvr-contrib/cmake )

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
                     "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
                     "${FlowVRContrib_BINARY_DIR}/cmake")
                     

# install flowvr-contrib-config
FOREACH(config ${ContribName}-config.sh )
  SET(INSTALL_PREFIX ${CMAKE_BINARY_DIR})
  SET(FLOWVRCONFIG_CMAKE "/share/flowvr-contrib/cmake")
  SET(FLOWVRCONTRIBCONFIG_CMAKE "/cmake")

  CONFIGURE_FILE(${config}.in ${CMAKE_BINARY_DIR}/bin/${config} @ONLY)
#  SET(INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
  CONFIGURE_FILE(${config}.in ${FlowVRContrib_BINARY_DIR}/${config}.dist @ONLY)
  INSTALL(PROGRAMS ${FlowVRContrib_BINARY_DIR}/${config}.dist 
          DESTINATION bin
          RENAME ${config})
ENDFOREACH(config)



# Add subdirectories
ADD_SUBDIRECTORY(components)


##########################################
# Major contrib components
##########################################

OPTION(BUILD_CONTRIB_FLOWVR_TESTS "FlowVR Contrib: Tests" FALSE)

OPTION(BUILD_CONTRIB_PORTUTILS  "FlowVR Contrib: Portutils" FALSE)


OPTION(BUILD_CONTRIB_FLOWVR_RENDER "FlowVR Contrib: Rendering Library based on FlowVR" FALSE)
IF( BUILD_CONTRIB_FLOWVR_RENDER) 
    SET(BUILD_CONTRIB_PORTUTILS TRUE  CACHE BOOL "Build  FLowVR Portuils" FORCE)
    MESSAGE(STATUS "FlowVR Render: portutils required. BUILD_CONTRIB_PORTUTILS option turned ON")
ENDIF(BUILD_CONTRIB_FLOWVR_RENDER) 


OPTION(BUILD_CONTRIB_VTK_FLOWVR  "FlowVR Contrib: VTK code encapsulation into FlowVR modules and VTK rendering with FlowVR Render" FALSE)
IF( BUILD_CONTRIB_VTK_FLOWVR) 
    SET(BUILD_CONTRIB_FLOWVR_RENDER TRUE  CACHE BOOL "Build  FlowVR Render" FORCE)
    MESSAGE(STATUS "VTK FlowVR: FlowVR Render required . BUILD_CONTRIB_FLOWVR_RENDER option turned ON")
ENDIF(BUILD_CONTRIB_VTK_FLOWVR) 


OPTION(BUILD_CONTRIB_FLOWVR_VRPN "FlowVR Contrib: Library to use VRPN devices in FlowVR applications." FALSE)



MACRO_LOG_FEATURE(BUILD_CONTRIB_FLOWVR_TESTS "FlowVR Contrib: tests"
                                   "Tests for flowvr." 
                                   "http://flowvr.sf.net" 
                                   FALSE 
                                   "Optional")

MACRO_LOG_FEATURE(BUILD_CONTRIB_PORTUTILS "FlowVR Contrib: portutils"
                                   "Automatic Module Generation" 
                                    "http://flowvr.sf.net" 
                                   FALSE 
                                   "Optional for FlowVR, but required by FlowVR Render")
MACRO_LOG_FEATURE(BUILD_CONTRIB_FLOWVR_RENDER "FlowVR Contrib: FlowVR Render" 
                                      "Rendering Library based on FlowVR" 
                                      "http://flowvr.sf.net" 
                                      FALSE 
                                      "Optional")
MACRO_LOG_FEATURE(BUILD_CONTRIB_VTK_FLOWVR "FlowVR Contrib: VTK FlowVR"
                                   "VTK code encapsulation into FlowVR modules and VTK rendering with FlowVR Render" 
                                   "http://flowvr.sf.net" 
                                   FALSE 
                                   "Optional")
MACRO_LOG_FEATURE(BUILD_CONTRIB_FLOWVR_VRPN "FlowVR Contrib: FlowVR VRPN" 
                                    "Library to use VRPN devices in FlowVR applications." 
                                    "http://flowvr.sf.net" 
                                    FALSE 
                                    "Optional")

MACRO_DISPLAY_FEATURE_LOG()


# Add sub directories  and set variable to find cmake directory 

SET(FlowVRPortUtils_DIR ${CMAKE_CURRENT_SOURCE_DIR}/portutils/cmake)
IF(BUILD_CONTRIB_PORTUTILS)
  ADD_SUBDIRECTORY(portutils)
ENDIF(BUILD_CONTRIB_PORTUTILS)

SET(FlowVRRender_DIR ${CMAKE_CURRENT_SOURCE_DIR}/flowvr-render/cmake)
IF(BUILD_CONTRIB_FLOWVR_RENDER)
  ADD_SUBDIRECTORY(flowvr-render)
ENDIF(BUILD_CONTRIB_FLOWVR_RENDER)

SET(VTKFlowVR_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/vtk-flowvr/cmake)
IF(BUILD_CONTRIB_VTK_FLOWVR)
  ADD_SUBDIRECTORY(vtk-flowvr)
ENDIF(BUILD_CONTRIB_VTK_FLOWVR)

SET(FlowVR-VRPN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/flowvr-vrpn/cmake)
IF(BUILD_CONTRIB_FLOWVR_VRPN)
  ADD_SUBDIRECTORY(flowvr-vrpn)
ENDIF(BUILD_CONTRIB_FLOWVR_VRPN)

IF(BUILD_CONTRIB_FLOWVR_TESTS)
  ADD_SUBDIRECTORY(tests)
ENDIF(BUILD_CONTRIB_FLOWVR_TESTS)

ADD_SUBDIRECTORY(utils)



INSTALL( DIRECTORY tutorials DESTINATION share/flowvr-contrib PATTERN ".svn" EXCLUDE )


FIND_PACKAGE(Doxygen)

IF(DOXYGEN)
  IF(DOT)
    SET(HAVE_DOT "YES")
    SET(CLASS_DIAGRAMS "NO")
  ELSE(DOT)
    SET(HAVE_DOT "NO")
    SET(CLASS_DIAGRAMS "YES")
  ENDIF(DOT)

  # we have to set the excluded items all in one row (or use the += syntax, which we do not do here)
  SET(DOX_ITEMS_EXCLUDED "*/cmake/* */tests/* */flowvr-vim/*")
  SET(DOX_PROJECT_NAME "FlowVR -- contribution directory")
  SET(DOX_PROJECT_PREFIX "FlowVR-Contrib")
  SET(DOX_PROJECT_VERSION ${PROJECT_VERSION})
    
  # use 'global' doxyfile.in from top-level
  CONFIGURE_FILE(../flowvr-suite-codedoc/Doxyfile.in ${FlowVRContrib_BINARY_DIR}/Doxyfile)
  CONFIGURE_FILE(../flowvr-suite-codedoc/footer.html.in ${FlowVRContrib_BINARY_DIR}/footer.html)
  CONFIGURE_FILE(../flowvr-suite-codedoc/header.html.in ${FlowVRContrib_BINARY_DIR}/header.html)
  
  ADD_CUSTOM_TARGET(doc-flowvr-contrib-install 
  COMMAND rm -rf doc/${DOX_PROJECT_PATH}/${DOX_PROJECT_PREFIX}
  COMMAND mkdir -p doc/${DOX_PROJECT_PATH}
  COMMAND cp -rp doc doc/${DOX_PROJECT_PREFIX})
  ADD_CUSTOM_TARGET(doc-flowvr-contrib COMMAND ${DOXYGEN})
  
  
  #INSTALL(DIRECTORY ${FlowVRContrib_BINARY_DIR}/doc/ DESTINATION ${CMAKE_INSTALL_PREFIX}/doc/${DOX_PROJECT_PREFIX} )
  INSTALL( FILES ../flowvr-suite-codedoc/flowvr-logo-small.png ../flowvr-suite-codedoc/stylesheet.css DESTINATION doc)
ENDIF(DOXYGEN)



ADD_SUBDIRECTORY(cmake.in)
# install cmake macros 
INSTALL(DIRECTORY  cmake  DESTINATION  share/flowvr-contrib/   
  PATTERN ".svn" EXCLUDE 
  PATTERN "FlowVRConfig.cmake" EXCLUDE )


