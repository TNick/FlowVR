# VTKFLOWVR EXAMPLE (deprecated, uses flowvr-app)

#INCLUDE_DIRECTORIES(../include ${FLOWVR_INCLUDE_DIR})


# COMPONENT LIBRARIES
#FOREACH(comp  vtkflowvr.comp)

  # SOURCE FILES
#  SET(components-SRC	${comp}.cpp)
  
#  ADD_TEST(${comp}_LIBRARY_TEST "flowvr-test" "${LIB_DIR}/lib${comp}${CMAKE_SHARED_LIBRARY_SUFFIX}")
#  SET_TESTS_PROPERTIES(${comp}_LIBRARY_TEST PROPERTIES PASS_REGULAR_EXPRESSION "successfully tested")


#  ADD_LIBRARY(${comp}  SHARED  ${components-SRC})
#  TARGET_LINK_LIBRARIES(${comp} ${FLOWVR_base_LIBRARY} 
#                                ${FLOWVR_mod_LIBRARY} 
#                                ${FLOWVR_ftl_LIBRARY} 
#                                ${FLOWVR_app_LIBRARY}   
#                                ${FLOWVR_app_comp_LIBRARY} 
#                                ${FLOWVR_render_comp_LIBRARY} )
#  SET_TARGET_PROPERTIES(${comp} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION})
#  INSTALL(TARGETS ${comp} LIBRARY DESTINATION ${COMP_LIB_DIR} )
  
#ENDFOREACH(comp)

