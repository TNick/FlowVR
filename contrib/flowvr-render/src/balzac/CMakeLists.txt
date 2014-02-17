PROJECT( Balzac )

FIND_PACKAGE( FlowVRRender REQUIRED )
FIND_PACKAGE( FreeGlut REQUIRED )


# Temporary modif to make it run on mac os x (bruno)
FIND_PACKAGE( GLEW REQUIRED )
IF(GLEW_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR})
ENDIF(GLEW_FOUND)


IF( FREEGLUT_FOUND )
    ADD_DEFINITIONS( -DBALZAC_USE_FREEGLUT )
ENDIF( FREEGLUT_FOUND )


SET(COMP_INCLUDE_DIR include/flowvr/render/components)
SET(COMP_INSTALL_DIR lib/flowvr/components)
SET(COMP_LIB_DIR lib/flowvr/components/)
SET(SHARE_DIR share/flowvr-render/balzac)
SET(MODULE_BIN_DIR bin)

SET( BALZAC_APP_DIR ${MODULE_BIN_DIR}/balzac )
SET( BALZAC_SHARE_DIR ${SHARE_DIR} )
SET( BALZAC_PLUGS_DIR ${BALZAC_SHARE_DIR}/plugs )
SET( BALZAC_SERVICES_DIR ${BALZAC_SHARE_DIR}/services )
SET( BALZAC_SHADERS_DIR ${BALZAC_SHARE_DIR}/data )
SET( BALZAC_GFX_DIR ${BALZAC_SHARE_DIR}/data )
SET( BALZAC_COMPONENTS_DIR ${CMAKE_INSTALL_DIR}/lib/flowvr/components )

SET(app Balzac)

#INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION include/flowvr/render FILES_MATCHING PATTERN "*.h" REGEX ".svn" EXCLUDE)
#INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/config/"  DESTINATION ${SHARE_DIR}/config FILES_MATCHING PATTERN "*.xml" REGEX ".svn" EXCLUDE)

#FOREACH( shell sh )
#  CONFIGURE_FILE(config/app-config.${shell}.in ${CMAKE_BINARY_DIR}/${app}-config.${shell} @ONLY)
#  INSTALL( PROGRAMS ${CMAKE_BINARY_DIR}/${app}-config.${shell} DESTINATION bin RENAME flowvr-balzac-config.${shell} )
#ENDFOREACH( shell sh )

##################################################################333


INCLUDE_DIRECTORIES( ../include ${FLOWVR_INCLUDE_DIR}
                                ${FLOWVRRENDER_INCLUDE_DIR})


###############################################################################
## BUILD SUBDIRECTORIES FIRST
###############################################################################

INCLUDE_DIRECTORIES(${FLOWVR_portutils_INCLUDE_DIR} )

ADD_SUBDIRECTORY(servicelayer)
ADD_SUBDIRECTORY(plugs)
ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(filters)

# TODO: convert from flowvr-app
#ADD_SUBDIRECTORY(components)
ADD_SUBDIRECTORY(services)

###############################################################################
## BALZAC EXECUTABLE
###############################################################################

SET( balzacname flowvr-balzac )




ADD_EXECUTABLE( ${balzacname} balzac.cpp ) # just depends on balzac.cpp

IF(UNIX)
  IF(NOT APPLE)
    # Note that it's a gcc version thing here, not really related to platform.
    # the below is needed to ensure that rtti information is passed
    # in between all .so files and the executable that is linking them.
    # in the port-utils, we use .so loading for executables, so this is
    # a mandatory flag. Note that this is valid for gcc > 3.x)
    SET_TARGET_PROPERTIES( ${balzacname} PROPERTIES LINK_FLAGS -Wl,-E)
  ENDIF( NOT APPLE )
ENDIF(UNIX)

TARGET_LINK_LIBRARIES( ${balzacname} ${FLOWVR_BALZAC_LIBRARY} 
                              ${FLOWVR_portutils_LIBRARY} 
                              ${GLUT_LIBRARIES}
                              ${FLOWVRRENDER_LIBRARY} )

INSTALL( TARGETS ${balzacname} DESTINATION bin ) 
