FIND_PACKAGE(FlowVRRender REQUIRED )

INCLUDE_DIRECTORIES( ../../include ${FLOWVR_INCLUDE_DIR} 
                     ${FLOWVRRENDER_INCLUDE_DIR} )
                     
                     

###############################################################################
## TRANSFORMSTATESERVICE
###############################################################################
SET( lib-name transformstateservice )
               
ADD_LIBRARY(${lib-name} SHARED transformstateservice.cpp)

TARGET_LINK_LIBRARIES(${lib-name} 
                      ${FLOWVR_base_LIBRARY} 
                      ${FLOWVR_mod_LIBRARY} 
                      ${FLOWVR_ftl_LIBRARY}    
                      ${FLOWVR_portutils_LIBRARY})
SET_TARGET_PROPERTIES(${lib-name} PROPERTIES PREFIX "")
                      
INSTALL(TARGETS ${lib-name} LIBRARY DESTINATION ${SHARE_DIR}/services )


###############################################################################
## AUTOBOUNDSSERVICE
###############################################################################
SET( lib-name autoboundsservice )
               
ADD_LIBRARY(${lib-name} SHARED autoboundsservice.cpp)

TARGET_LINK_LIBRARIES(${lib-name}
                      transformstateservice 
                      ${FLOWVR_base_LIBRARY} 
                      ${FLOWVR_mod_LIBRARY} 
                      ${FLOWVR_ftl_LIBRARY}       
                      ${FLOWVR_portutils_LIBRARY})
SET_TARGET_PROPERTIES(${lib-name} PROPERTIES PREFIX "")                      
INSTALL(TARGETS ${lib-name} LIBRARY DESTINATION ${SHARE_DIR}/services )

###############################################################################
## EXTRACTBOUNDS
###############################################################################
SET( lib-name extractboundsservice )
               
ADD_LIBRARY(${lib-name} SHARED extractboundsservice.cpp)

TARGET_LINK_LIBRARIES(${lib-name}                     
                      ${FLOWVR_base_LIBRARY} 
                      ${FLOWVR_mod_LIBRARY} 
                      ${FLOWVR_ftl_LIBRARY}    
                      ${FLOWVRRENDER_LIBRARY}   
                      ${FLOWVR_portutils_LIBRARY})
SET_TARGET_PROPERTIES(${lib-name} PROPERTIES PREFIX "")                      
INSTALL(TARGETS ${lib-name} LIBRARY DESTINATION ${SHARE_DIR}/services )

