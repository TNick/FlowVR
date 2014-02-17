# SOURCE FILES
INCLUDE_DIRECTORIES(
  ${flowvr_portutils_SOURCE_DIR}/include
  ${flowvr_base_SOURCE_DIR}/include
  ${flowvr_ftl_SOURCE_DIR}/include
  )

SET(target flowvr-portutils)

SET(${target}-SRC  porthandler.cpp 
                   porthandlerregistry.cpp 
                   portcomm.cpp 
                   portconfigure.cpp 
                   portmodule.cpp 
                   portservicelayer.cpp
                   portparameter.cpp )

# MODULE  BINARIES
# build targets bound to flowvr

ADD_LIBRARY(${target} SHARED ${${target}-SRC})
TARGET_LINK_LIBRARIES(${target}
                      flowvr-base 
                      flowvr-mod
                      ftlm
	              ${CMAKE_THREAD_LIBS_INIT} 
	              ${CMAKE_DL_LIBS} )
                        
SET_TARGET_PROPERTIES(${target} PROPERTIES
                      SOVERSION ${PROJECT_VERSION_MAJOR} 
                      VERSION ${PROJECT_VERSION})
                                              
INSTALL( TARGETS ${target}  LIBRARY DESTINATION lib )

SET( execname flowvr-portbinary )

ADD_EXECUTABLE(${execname} portbinary.cpp )

IF(UNIX)
  IF(NOT APPLE)
    # Note that it's a gcc version thing here, not really related to platform.
    # the below is needed to ensure that rtti information is passed
    # in between all .so files and the executable that is linking them.
    # in the port-utils, we use .so loading for executables, so this is
    # a mandatory flag. Note that this is valid for gcc > 3.x)
    SET_TARGET_PROPERTIES( ${execname} PROPERTIES LINK_FLAGS -Wl,-E)
  ENDIF( NOT APPLE )
ENDIF(UNIX)

TARGET_LINK_LIBRARIES( ${execname} flowvr-portutils )

INSTALL(TARGETS ${execname} DESTINATION bin )

ADD_SUBDIRECTORY(components)
ADD_SUBDIRECTORY(tools)
