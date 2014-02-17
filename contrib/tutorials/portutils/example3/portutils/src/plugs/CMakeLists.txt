###################################################################
## CMAKE FILE TO BUILD A DIRECTORY OF PLUGS
##
## CHANGE:
## - DESTINATION (where to install the plugs)
## - all-DEPS fields (all to add libraries needed for all plugs)
## - <myplug>-DEPS (individual dependencies for single plugs)
###################################################################

FIND_PACKAGE( FlowVR REQUIRED )

# each cpp is a plugin of its own
# glob all files ending with cpp in this directory
FILE(GLOB PLUGS-SRC *.cpp )

INCLUDE_DIRECTORIES( ../../include ${FLOWVR_INCLUDE_DIR} )


SET( DESTINATION ./plugs )


SET( all-DEPS DataService )
# SET( <myplug>-DEPS <all-libs-deps> )



# ${plug} contains the full build path to the source code 
FOREACH(plug ${PLUGS-SRC})
 # strip plug-name from plug (full path)
 # find plugin name:
 # search for:
 # 'any prefix' ( ".*" -> match any character any number of times ) until reached a 'slash'
 # that has a suffix '.cpp' at the end
 # now: capture the name (any character up to the point before 'cpp') in \1
 # store in plug-name, take from plug
 STRING( REGEX REPLACE ".*/(.*)[.]cpp$" "\\1" plug-name ${plug} )

 MESSAGE( STATUS "file: " ${plug} )
 MESSAGE( STATUS "plug: " ${plug-name} )

 ADD_LIBRARY(${plug-name} SHARED ${plug})
 
  # we assume that any plugin needs msg types defined for balzac now and then                      
  TARGET_LINK_LIBRARIES( ${plug-name} 
                         ${FLOWVR_base_LIBRARY} 
                         ${FLOWVR_ftl_LIBRARY} 
                         ${FLOWVR_portutils_LIBRARY} 
                         ${all-DEPS} 
                         ${${plug-name}-DEPS} )
                      
 INSTALL(TARGETS ${plug-name} DESTINATION ${DESTINATION} 
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
ENDFOREACH(plug)
