# TUTORIAL EXAMPLE 

INCLUDE_DIRECTORIES(../include
                    ${FLOWVR_INCLUDE_DIR}
                    ${FLOWVR_base_INCLUDE_DIR}
                    ${FLOWVR_mod_INCLUDE_DIR}
                    ${FLOWVR_ftl_INCLUDE_DIR}
                    ${FLOWVRRENDER_INCLUDE_DIR})


# MODULE  BINARIES

IF(BUILD_MODULES)
  
    # used only for out-of-source build
    
    
    LINK_LIBRARIES(${FLOWVR_base_LIBRARY} 
                   ${FLOWVR_mod_LIBRARY} 
                   ${FLOWVR_ftl_LIBRARY} 
                   ${FLOWVRRENDER_LIBRARY} 
                   ${CMAKE_THREAD_LIBS_INIT} 
                   ${OPENGL_LIBRARIES} 
                   ${GLUT_LIBRARIES} )

    FOREACH(target creatorparticles viewerparticles1 viewerparticles2)
        ADD_EXECUTABLE(${target} ${target}.cpp)
        INSTALL(TARGETS ${target}  DESTINATION ${MODULE_DIR})
    ENDFOREACH(target)    
  
ENDIF(BUILD_MODULES)
