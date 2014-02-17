SET(INSTALL_PREFIX ${CMAKE_BINARY_DIR})
CONFIGURE_FILE(FlowVRRenderConfig.cmake.in FlowVRRenderConfig.cmake @ONLY)
SET(INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
CONFIGURE_FILE(FlowVRRenderConfig.cmake.dist.in ${FlowVRRender_BINARY_DIR}/FlowVRRenderConfig.cmake.dist @ONLY)
INSTALL(FILES ${FlowVRRender_BINARY_DIR}/FlowVRRenderConfig.cmake.dist
        DESTINATION share/flowvr-render/cmake
        RENAME FlowVRRenderConfig.cmake)
        
