
# For the uninstalled version
IF(NOT EXISTS ${CMAKE_BINARY_DIR}/share/flowvr-render/data)
  FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/flowvr-render)
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/share/flowvr-render/data OUTPUT_VARIABLE output ERROR_VARIABLE output RESULT_VARIABLE result)

  IF(result)
    MESSAGE(STATUS "Failed symlinking FlowVRRender data. making a copy...")
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/share/flowvr-render/data OUTPUT_VARIABLE output ERROR_VARIABLE output RESULT_VARIABLE result)
  ENDIF(result)

  IF(result)
    MESSAGE(SEND_ERROR "FlowVRRender data cannot be used in uninstalled version. Only installed version will work.")
  ENDIF(result)
ENDIF(NOT EXISTS ${CMAKE_BINARY_DIR}/share/flowvr-render/data)

#  For the installed version
INSTALL(DIRECTORY  .   DESTINATION  share/flowvr-render/data/ PATTERN ".svn" EXCLUDE)

