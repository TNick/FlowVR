## source files


SET(lib-name flowvr-balzacservicelayer)               






INCLUDE_DIRECTORIES(../../include
                    ${FLOWVR_INCLUDE_DIR} 
                    ${FLOWVRRENDER_INCLUDE_DIR} 
                    ${FLOWVR_portutils_INCLUDE_DIR} )


SET(balzac-SRC display.cpp
               glutdata.cpp
               input.cpp
               servicelayer.cpp
               systemcontrol.cpp )


               
ADD_LIBRARY(${lib-name} SHARED ${balzac-SRC})


TARGET_LINK_LIBRARIES(${lib-name} 
                      ${FLOWVR_base_LIBRARY} 
                      ${FLOWVR_mod_LIBRARY} 
                      ${FLOWVR_ftl_LIBRARY}    
                      ${FLOWVRRENDER_LIBRARY}
                      ${FLOWVR_portutils_LIBRARY}
                      ${GLUT_LIBRARIES} )
                      
SET_TARGET_PROPERTIES(${lib-name} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION})
INSTALL(TARGETS ${lib-name} LIBRARY DESTINATION lib )