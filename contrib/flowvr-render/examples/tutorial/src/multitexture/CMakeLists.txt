INCLUDE_DIRECTORIES( ../../include ${FLOWVR_INCLUDE_DIR} 
                     ${FLOWVRRENDER_INCLUDE_DIR})
                         
#########################################################
## EXAMPLE MODULES
#########################################################

SET( PLUG_LIB_DIR lib )

FILE(GLOB PLUGS-SRC *Plug.cpp )

# ${plug} contains the full build path to the source code 
FOREACH(plug ${PLUGS-SRC})
 MESSAGE( STATUS "file: " ${plug} )
 # strip plug-name from plug (full path)
 # find plugin name:
 # search for:
 # 'any prefix' ( ".*" -> match any character any number of times ) until reached a 'slash'
 # that has a suffix '.cpp' at the end
 # now: capture the name (any character up to the point before 'cpp') in \1
 # store in plug-name, take from plug
 STRING( REGEX REPLACE ".*/(.*)[.]cpp$" "\\1" plug-name ${plug} )
 MESSAGE( STATUS "plug: " ${plug-name} )
 ADD_LIBRARY(${plug-name} SHARED ${plug})
 SET_TARGET_PROPERTIES(${plug-name} PROPERTIES PREFIX "")

  # we assume that any plugin needs msg types defined for balzac now and then                      
  TARGET_LINK_LIBRARIES( ${plug-name} ${FLOWVR_BALZAC_AUX_LIBRARY} 
                                      ${FLOWVRRENDER_LIBRARY} 
                                      ${FLOWVR_base_LIBRARY}  
                                      ${FLOWVR_mod_LIBRARY}  
                                      ${FLOWVR_portutils_LIBRARY} )
                      
 INSTALL(TARGETS ${plug-name} DESTINATION ${PLUG_LIB_DIR}
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
ENDFOREACH(plug)