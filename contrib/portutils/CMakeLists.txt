PROJECT(FlowVRPortUtils)


SET( FlowVRPortUtils_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${FlowVR_DIR} ${FlowVRRender_DIR} ${FlowVRPortUtils_DIR})

FIND_PACKAGE(FlowVR REQUIRED)


IF(BUILD_CONTRIB_PORTUTILS)

SET( flowvr_portutils_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
  
INCLUDE_DIRECTORIES(${FLOWVR_base_INCLUDE_DIR}) #${FlowVR_BINARY_DIR}/flowvr-base/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE}/flowvr-ftl/include)
ADD_SUBDIRECTORY(flowvr-app)
ADD_SUBDIRECTORY(flowvr-parse)  
ADD_SUBDIRECTORY(src)

  INSTALL( DIRECTORY include/flowvr/portutils DESTINATION include/flowvr FILES_MATCHING PATTERN "*.h" REGEX ".svn" EXCLUDE )
  INSTALL(FILES portutils.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/flowvr/python)
ENDIF(BUILD_CONTRIB_PORTUTILS)




