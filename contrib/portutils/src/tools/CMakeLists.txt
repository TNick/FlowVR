# each cpp is a toolin of its own
# glob all files ending with cpp in this directory
FILE(GLOB TOOLS-SRC *.cpp )

INCLUDE_DIRECTORIES(  ${flowvr_portutils_SOURCE_DIR}/include   )

# ${tool} contains the full build path to the source code 
FOREACH(tool ${TOOLS-SRC})
 # strip tool-name from tool (full path)
 # find toolin name:
 # search for:
 # 'any prefix' ( ".*" -> match any character any number of times ) until reached a 'slash'
 # that has a suffix '.cpp' at the end
 # now: capture the name (any character up to the point before 'cpp') in \1
 # store in tool-name, take from tool
 STRING( REGEX REPLACE ".*/(.*)[.]cpp$" "\\1" tool-name ${tool} )
 ADD_EXECUTABLE( flowvr-${tool-name} ${tool})

 # we assume that any toolin needs msg types defined for balzac now and then                      
 TARGET_LINK_LIBRARIES( flowvr-${tool-name} 
      flowvr-portutils
      ftlm
      )
                      
 INSTALL(TARGETS flowvr-${tool-name} DESTINATION bin 
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ)
ENDFOREACH(tool)

