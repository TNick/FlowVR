#########################################################################################
## FLOWVR-RENDER PROJECT
#########################################################################################

PROJECT(FlowVRRender)
SET(PROJECT_VERSION_MAJOR "1")
SET(PROJECT_VERSION_MINOR "9")
SET(PROJECT_VERSION_PATCH "0")
SET(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

#########################################################################################
## CMAKE STUFF
#########################################################################################

SET( FlowVRRender_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake )


MESSAGE (" PORTUTILS ${FlowVRPortUtils_DIR}")
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${FlowVR_DIR} ${FlowVRRender_DIR} ${FlowVRPortUtils_DIR})

#########################################################################################
## PYTHON API
#########################################################################################
INSTALL(FILES balzac.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/flowvr/python)

#########################################################################################
## DEPENDENCIES
#########################################################################################

FIND_PACKAGE(FlowVR REQUIRED)
FIND_PACKAGE(Freetype REQUIRED)
FIND_PACKAGE(X11 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(Cg REQUIRED) # REQUIRE  GL
FIND_PACKAGE(Devil REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(FlowVRPortUtils REQUIRED)
FIND_PACKAGE(Doxygen)



#########################################################################################
# LOG
#########################################################################################
INCLUDE(MacroLogFeature)

MACRO_LOG_FEATURE(DOXYGEN "Doxygen" "Documentation generation" "http://www.doxygen.org" FALSE "Optional")
MACRO_LOG_FEATURE(GLEW_FOUND "GLEW" "OpenGL Extension Wrangler Library" "http://glew.sourceforge.net" TRUE "Required by FlowVR Render")
MACRO_LOG_FEATURE(OPENGL_FOUND "OpenGL" "" "http://www.opengl.org"  TRUE "Required by FlowVR Render (for the renderer)" )
MACRO_LOG_FEATURE(GLUT_FOUND "Glut" "The OpenGL Utility Toolkit" "http://freeglut.sourceforge.net" TRUE "Required by FlowVR Render (for the renderer)")
MACRO_LOG_FEATURE(X11_FOUND "X11" "" "" FALSE "Required for font generation and OpenGL wrapper")
MACRO_LOG_FEATURE(FREETYPE_FOUND "Freetype" "" "" FALSE "Required for font generation tool")
MACRO_LOG_FEATURE(Devil_FOUND "Devil Image Library" "Image handling library" "http://openil.sourceforge.net" TRUE "Required by FlowVR Render")
MACRO_LOG_FEATURE(FLOWVR_FOUND "FlowVR" "FlowVR Middelware" "http://flowvr.sourceforge.net" TRUE "Required By FlowVR Render")
MACRO_LOG_FEATURE(FLOWVR_portutils_FOUND "FlowVR Contrib: Portutils"  "Automatic Module Generation"  "http://flowvr.sourceforge.net" TRUE "Required By FlowVR Render")



###################################################################################
## CG DEFS
###################################################################################
IF(CG_FOUND AND CG_GL_LIBRARY)
  SET(FLOWVR_RENDER_HAVE_CG TRUE)
  INCLUDE_DIRECTORIES(${CG_INCLUDE_PATH})
ELSE(CG_FOUND AND CG_GL_LIBRARY)
  SET(FLOWVR_RENDER_HAVE_CG FALSE)  
ENDIF(CG_FOUND AND CG_GL_LIBRARY)
MACRO_LOG_FEATURE(CG_FOUND "Cg" "NVIDIA Cg Toolkit" "http://developer.nvidia.com" TRUE "Required by FlowVR Render")


#########################################################################################
## SETTINGS AND TESTS AND CONFIGURATION
#########################################################################################

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
INCLUDE_DIRECTORIES(${FlowVRRender_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${FlowVRRender_SOURCE_DIR}/include/flowvr/render/balzac/components)

##
## PUT BALZAC DEFINITIONS HERE
## WE NEED THEM FOR THE CONFIGURATION OF THE GLOBAL PATHS
SET( SHARE_DIR share/flowvr-render/balzac)
SET( MODULE_BIN_DIR bin)
SET( BALZAC_APP_DIR ${MODULE_BIN_DIR}/balzac )
SET( BALZAC_SHARE_DIR ${SHARE_DIR} )
SET( BALZAC_PLUGS_DIR ${BALZAC_SHARE_DIR}/plugs )
SET( BALZAC_SERVICES_DIR ${BALZAC_SHARE_DIR}/services )
SET( BALZAC_SHADERS_DIR ${BALZAC_SHARE_DIR}/data )
SET( BALZAC_GFX_DIR ${BALZAC_SHARE_DIR}/data )
SET( BALZAC_COMPONENTS_DIR ${CMAKE_INSTALL_DIR}/lib/flowvr/components )


FOREACH(config flowvr-render-config.sh)
  SET(INSTALL_PREFIX ${CMAKE_BINARY_DIR})
  SET(FLOWVRRENDERCONFIG_CMAKE)	
  CONFIGURE_FILE(${config}.in ${CMAKE_BINARY_DIR}/bin/${config} @ONLY)
  SET(INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
  SET(FLOWVRRENDERCONFIG_CMAKE "/share/flowvr-render/cmake")
  CONFIGURE_FILE(${config}.in ${FlowVRRender_BINARY_DIR}/${config}.dist @ONLY)
  INSTALL(PROGRAMS ${FlowVRRender_BINARY_DIR}/${config}.dist 
          DESTINATION bin
          RENAME ${config})
ENDFOREACH(config)


# Install cmake macros and modules
INSTALL(DIRECTORY  cmake  DESTINATION  share/flowvr-render/   
  PATTERN "FlowVRRenderConfig.cmake" EXCLUDE 
  PATTERN "FindBalzac.cmake" EXCLUDE
  PATTERN ".svn" EXCLUDE )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib )

ADD_SUBDIRECTORY( cmake.in )



###############################################################################
##
###############################################################################

INCLUDE_DIRECTORIES(${FLOWVR_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR} )

IF(X11_FOUND)
  INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
ENDIF(X11_FOUND)

IF(FREETYPE_FOUND)
  INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIRS})
ENDIF(FREETYPE_FOUND)

IF(GLEW_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR})
ENDIF(GLEW_FOUND)

###################################################################################
## 
###################################################################################


ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(src)

ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(examples)



#########################################################################################
## DOXYGEN (documentation) stuff
#########################################################################################

IF(DOXYGEN)
  IF(DOT)
    SET(DOX_HAVE_DOT "YES")
  ELSE(DOT)
    SET(DOX_HAVE_DOT "NO")
  ENDIF(DOT)

  # we have to set the excluded items all in one row (or use the += syntax, which we do not do here)
  SET(DOX_ITEMS_EXCLUDED "*/data/* */.svn/* */examples/* */src/viewers/*")
  SET(DOX_PROJECT_NAME "FlowVR -- Render")
  SET(DOX_PROJECT_PREFIX "FlowVRRender")
  SET(DOX_PROJECT_VERSION ${PROJECT_VERSION})
  
    
  # use 'global' doxyfile.in from top-level
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/flowvr-suite-codedoc/Doxyfile.in ${FlowVRRender_BINARY_DIR}/Doxyfile)
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/flowvr-suite-codedoc/footer.html.in ${FlowVRRender_BINARY_DIR}/footer.html)
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/flowvr-suite-codedoc/header.html.in ${FlowVRRender_BINARY_DIR}/header.html)
  
  ADD_CUSTOM_TARGET(doc-flowvr-render-install 
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/doc/${DOX_PROJECT_PATH}/${DOX_PROJECT_PREFIX}
  COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/doc/${DOX_PROJECT_PATH}
  COMMAND cp -rp ${CMAKE_CURRENT_BINARY_DIR}/doc ${CMAKE_INSTALL_PREFIX}/doc/${DOX_PROJECT_PREFIX})
  ADD_CUSTOM_TARGET(doc-flowvr-render COMMAND ${DOXYGEN} )
  
  INSTALL( FILES ${CMAKE_SOURCE_DIR}/flowvr-suite-codedoc/flowvr-logo-small.png ${CMAKE_SOURCE_DIR}/flowvr-suite-codedoc/stylesheet.css 
           DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
ENDIF(DOXYGEN)


#########################################################################################
# packaging
#########################################################################################
SET(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
SET(CPACK_PACKAGE_VENDOR "INRIA - Grenoble Universities")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FlowVR Render description...")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${FlowVRRender_SOURCE_DIR}/README")
#SET(CPACK_RESOURCE_FILE_LICENSE "${FlowVRRender_SOURCE_DIR}/copyright-apps.txt")
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
MACRO_DISPLAY_FEATURE_LOG()

#########################################################################################
#########################################################################################
