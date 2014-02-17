# Module
#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/share/flowvr-render/modules/bin)
SET(modules-SRC traceparticles.cpp writeobj.cpp)
FOREACH(modulesrc ${modules-SRC})
  STRING(REPLACE ".cpp" "" module ${modulesrc})
  ADD_EXECUTABLE(flowvr-${module} ${modulesrc})
  TARGET_LINK_LIBRARIES(flowvr-${module} flowvr-render ${FLOWVR_mod_LIBRARY})
  INSTALL(TARGETS flowvr-${module} DESTINATION share/flowvr-render/modules/bin)
ENDFOREACH(modulesrc)
