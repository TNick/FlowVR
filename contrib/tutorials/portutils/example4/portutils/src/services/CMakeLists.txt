###################################################################
## CMAKE FILE TO BUILD A DIRECTORY OF SERVICES
##
## CHANGE:
## - DESTINATION (where to install the SERVICES)
## - all-DEPS fields (all to add libraries needed for all SERVICES)
## - <myservice>-DEPS (individual dependencies for single SERVICES)
###################################################################

FIND_PACKAGE( FlowVR REQUIRED )

# each cpp is a servicein of its own
# glob all files ending with cpp in this directory
FILE(GLOB SERVICES-SRC *.cpp )

INCLUDE_DIRECTORIES( ../../include ${FLOWVR_INCLUDE_DIR} 
                     ${PORTUTILS_INCLUDE_DIR} )

LINK_DIRECTORIES( ${PORTUTILS_LIB_DIR} )

SET( DESTINATION ./services )


# SET( all-DEPS <deps-for-all-libs> )
# SET( <myservice>-DEPS <all-libs-deps> )



# ${service} contains the full build path to the source code 
FOREACH(service ${SERVICES-SRC})
 # strip service-name from service (full path)
 # find servicein name:
 # search for:
 # 'any prefix' ( ".*" -> match any character any number of times ) until reached a 'slash'
 # that has a suffix '.cpp' at the end
 # now: capture the name (any character up to the point before 'cpp') in \1
 # store in service-name, take from service
 STRING( REGEX REPLACE ".*/(.*)[.]cpp$" "\\1" service-name ${service} )

 MESSAGE( STATUS "file: " ${service} )
 MESSAGE( STATUS "service: " ${service-name} )

 ADD_LIBRARY(${service-name} SHARED ${service})
 
  # we assume that any servicein needs msg types defined for balzac now and then                      
  TARGET_LINK_LIBRARIES( ${service-name} 
                         ${FLOWVR_base_LIBRARY} 
                         ${FLOWVR_ftl_LIBRARY} 
                         ${FLOWVR_portutils_LIBRARY} 
                         ${all-DEPS} 
                         ${${service-name}-DEPS} )
                      
 INSTALL(TARGETS ${service-name} DESTINATION ${DESTINATION} 
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
ENDFOREACH(service)
