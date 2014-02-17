
# each cpp is a plugin of its own
# glob all files ending with cpp in this directory
FILE(GLOB LIBS-SRC *.cpp )

INCLUDE_DIRECTORIES( ../../include ${FLOWVR_INCLUDE_DIR} )

# ${lib} contains the full build path to the source code 
FOREACH(lib ${LIBS-SRC})
 # strip libs-name from plug (full path)
 # find plugin name:
 # search for:
 # 'any prefix' ( ".*" -> match any character any number of times ) until reached a 'slash'
 # that has a suffix '.cpp' at the end
 # now: capture the name (any character up to the point before 'cpp') in \1
 # store in libs-name, take from plug
 STRING( REGEX REPLACE ".*/(.*)[.]cpp$" "\\1" lib-name ${lib} )
 ADD_LIBRARY(flowvr-${lib-name} SHARED ${lib})
 TARGET_LINK_LIBRARIES(flowvr-${lib-name} ${FLOWVR_base_LIBRARY})
 SET_TARGET_PROPERTIES(flowvr-${lib-name} PROPERTIES
                      SOVERSION ${PROJECT_VERSION_MAJOR} 
                      VERSION ${PROJECT_VERSION})
                      
 INSTALL(TARGETS flowvr-${lib-name} LIBRARY DESTINATION lib
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
ENDFOREACH(lib)

