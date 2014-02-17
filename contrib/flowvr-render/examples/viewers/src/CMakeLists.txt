INCLUDE_DIRECTORIES(${FLOWVR_base_INCLUDE_DIR}
                    ${FLOWVR_mod_INCLUDE_DIR}
                    ${FLOWVR_ftl_INCLUDE_DIR}
                    ${FLOWVRRENDER_INCLUDE_DIR})

LINK_LIBRARIES(${FLOWVR_base_LIBRARY} ${FLOWVR_mod_LIBRARY} ${FLOWVR_ftl_LIBRARY} ${FLOWVRRENDER_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

# used only for out-of-source build

FOREACH(target text1 text2 spheres sphere1 sphere2 volume1 fire)
    ADD_EXECUTABLE(${target} ${target}.cpp)
    INSTALL(TARGETS ${target}  DESTINATION ${MODULE_DIR})
ENDFOREACH(target)
