PROJECT(FlowVR)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
SET(FLOWVR_PREFIX ${CMAKE_BINARY_DIR})


###############################################################
## DEPENDENCIES
###############################################################
INCLUDE(MacroLogFeature)

FIND_PACKAGE(PythonLibs 2)
MACRO_LOG_FEATURE(PYTHONLIBS_FOUND  "PythonLibs" 
  "Python Libraries" 
  "www.python.org/" 
  TRUE
  "Required to process FlowVR applications and to generate the Python Module API")


FIND_PACKAGE(PythonInterp 2)
MACRO_LOG_FEATURE(PYTHONINTERP_FOUND  "PythonInterp" 
  "Python Interpreter" 
  "www.python.org/" 
  TRUE
  "Required to process FlowVR applications")

FIND_PACKAGE(Threads)
MACRO_LOG_FEATURE(CMAKE_USE_PTHREADS_INIT "Threads" 
                                          "Threading support" 
                                          "" 
                                          TRUE 
                                          "Mandatory")
FIND_PACKAGE(OpenGL)
MACRO_LOG_FEATURE(OPENGL_FOUND "OpenGL" 
                               "OpenGL support" 
                               "http://www.opengl.org" 
                               FALSE  
                               "Required by FlowVR GLTrace, FlowVR GLGraph and some examples")
FIND_PACKAGE(GLUT)
MACRO_LOG_FEATURE(GLUT_FOUND "Glut" 
                             "The OpenGL Utility Toolkit" 
                             "http://freeglut.sourceforge.net" 
                             FALSE 
                             "Required by FlowVR GLTrace, FlowVR GLGraph and some examples")
FIND_PACKAGE(Doxygen)
MACRO_LOG_FEATURE(DOXYGEN "Doxygen" 
                          "Documentation generation" 
                          "http://www.doxygen.org" 
                          FALSE 
                          "Optional")

###################################################################
## GENERAL SETTINGS NEEDED FOR ALL SUBCOMPONENTS
###################################################################
INCLUDE_DIRECTORIES(${FlowVR_BINARY_DIR}/flowvr-base/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE}/flowvr-ftl/include)

###################################################################
# install flowvr-config shell script
###################################################################
FOREACH(config flowvr-config.sh )
  SET(INSTALL_PREFIX ${CMAKE_BINARY_DIR})
  SET(FLOWVRCONFIG_CMAKE) # assign the empty variable for this script
  CONFIGURE_FILE(${config}.in ${CMAKE_BINARY_DIR}/bin/${config} @ONLY)
  
  SET(INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
  SET(FLOWVRCONFIG_CMAKE "/share/flowvr/cmake")
  CONFIGURE_FILE(${config}.in ${FlowVR_BINARY_DIR}/${config}.dist @ONLY)
  
  INSTALL(PROGRAMS ${FlowVR_BINARY_DIR}/${config}.dist 
          DESTINATION bin
          RENAME ${config})
ENDFOREACH(config)

ADD_SUBDIRECTORY(cmake.in)

# install cmake macros 
INSTALL(DIRECTORY  cmake  DESTINATION  share/flowvr/   
  PATTERN ".svn" EXCLUDE 
  PATTERN "FlowVRConfig.cmake" EXCLUDE )

##################################################################
## COMPONENTS
##################################################################
  
set(components flowvr-base 
               flowvr-ftl 
               flowvr-daemon
               flowvr-interface 
               flowvr-scripts 
               flowvr-modules 
               flowvr-launcher 
               flowvr-appy
               flowvr-moduleapi
               flowvr-examples )



FOREACH(component ${components})
    ADD_SUBDIRECTORY(${component})  
ENDFOREACH(component ${components})


###################################################################
# packaging
###################################################################
SET(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
SET(CPACK_PACKAGE_VENDOR "INRIA - Grenoble Universities")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FlowVR: A Middleware for High Performance Interactive Computing")

SET(CPACK_PACKAGE_DESCRIPTION_FILE "${FlowVR_SOURCE_DIR}/../README")

SET(CPACK_RESOURCE_FILE_LICENSE "${FlowVR_SOURCE_DIR}/flowvr-codedoc/flowvr_copyright.txt")
SET(CPACK_SOURCE_IGNORE_FILES "/CVS/;/_darcs/;/\\\\\\\\.svn/;\\\\\\\\.swp$;\\\\\\\\.#;/#")

# Pick a generator
IF(UNIX)
  IF(APPLE)
    SET(CPACK_GENERATOR "PackageMaker;STGZ")
  ELSE(APPLE)
    SET(CPACK_GENERATOR "STGZ;TGZ")
  ENDIF(APPLE)
  SET(CPACK_SOURCE_GENERATOR "TGZ")
ELSE(UNIX)
  SET(CPACK_GENERATOR "NSIS;ZIP")
  SET(CPACK_SOURCE_GENERATOR "ZIP")
ENDIF(UNIX)

INCLUDE(CPack)

INCLUDE(cmake/Pipol.cmake)


#######################################################################
## DOXYGEN
#######################################################################

IF(DOXYGEN)
  IF(DOT)
    SET(DOX_HAVE_DOT "YES")
  ELSE(DOT)
    SET(DOX_HAVE_DOT "NO")
  ENDIF(DOT)

  # we have to set the excluded items all in one row (or use the += syntax, which we do not do here)
  SET(DOX_ITEMS_EXCLUDED "*/tinyxml* */${ContribName}/* */flowvr-render/* */flowvr-vrpn/* */vtk-flowvr/* */flowvr/cmake/* */flowvr-examples/* */flowvr-interface/*  */flowvr-utils/* */flowvr-app/* */flowvr-parse/* */.svn/* */topology/*")
  SET(DOX_PROJECT_NAME "FlowVR -- base library")
  SET(DOX_PROJECT_PREFIX "FlowVR")
  SET(DOX_PROJECT_VERSION ${PROJECT_VERSION})
  
  SET(DOX_PREDEFINED_SYMBOLS "DEBUG FLOWVR_SCHED_HAVE_GETCPU FLOWVR_IPC_DEBUG")
    
  # use 'global' doxyfile.in from top-level
  CONFIGURE_FILE(../flowvr-suite-codedoc/Doxyfile.in ${FlowVR_BINARY_DIR}/Doxyfile)
  CONFIGURE_FILE(../flowvr-suite-codedoc/footer.html.in ${FlowVR_BINARY_DIR}/footer.html)
  CONFIGURE_FILE(../flowvr-suite-codedoc/header.html.in ${FlowVR_BINARY_DIR}/header.html)
  
  ADD_CUSTOM_TARGET(doc-flowvr-install 
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/doc/${DOX_PROJECT_PATH}/${DOX_PROJECT_PREFIX}
  COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/doc/${DOX_PROJECT_PATH}
  COMMAND cp -rp ${CMAKE_CURRENT_BINARY_DIR}/doc ${CMAKE_INSTALL_PREFIX}/doc/${DOX_PROJECT_PREFIX})
  ADD_CUSTOM_TARGET(doc-flowvr COMMAND ${DOXYGEN})
  
  INSTALL( FILES ../flowvr-suite-codedoc/flowvr-logo-small.png ../flowvr-suite-codedoc/stylesheet.css 
           DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
           
ENDIF(DOXYGEN)


MACRO_DISPLAY_FEATURE_LOG()
