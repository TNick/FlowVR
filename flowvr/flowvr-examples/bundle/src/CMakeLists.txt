# TICTAC EXAMPLE 


INCLUDE_DIRECTORIES(${FLOWVR_INCLUDE_DIR})

# MODULE  BINARIES

IF(BUILD_MODULES)

FOREACH(target putMultiple getMultiple)
  ADD_EXECUTABLE(${target} ${target}.cpp)
# Should work but Linux complain about not finding dl lib used  by flowvr-plugd
#  TARGET_LINK_LIBRARIES(${target} ${FLOWVR_LIBRARY})
  TARGET_LINK_LIBRARIES(${target} ${FLOWVR_base_LIBRARY} ${FLOWVR_ftl_LIBRARY} ${FLOWVR_mod_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
  INSTALL(TARGETS ${target}  DESTINATION ${MODULE_DIR})	
ENDFOREACH(target)

ENDIF(BUILD_MODULES)
