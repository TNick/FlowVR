PROJECT(flowvrd)

# MPI plugin currently broken
IF ( BUILD_FLOWVRD_MPI_PLUGIN )
  FIND_PACKAGE(MPI)
  IF(MPI_INCLUDE_PATH)
    SET(BUILD_FLOWVRD_MPI_PLUGIN OFF CACHE BOOL "Should FlowVRd MPI communication plugin be built")
  ELSE(MPI_INCLUDE_PATH)
    SET(BUILD_FLOWVRD_MPI_PLUGIN OFF CACHE BOOL "Should FlowVRd MPI communication plugin be built" FORCE)
  ENDIF(MPI_INCLUDE_PATH)
  MACRO_LOG_FEATURE(BUILD_FLOWVRD_MPI_PLUGIN "MPI" "Message Passing Interface network plugin for FlowVR" "" FALSE "" "Used by FlowVR to provide MPI communications between Daemons")
ENDIF( BUILD_FLOWVRD_MPI_PLUGIN )

INCLUDE_DIRECTORIES("${flowvr_base_SOURCE_DIR}/include"
		    "${flowvr_ftl_SOURCE_DIR}/include"
		    "${flowvrd_SOURCE_DIR}/include"
		    ${CMAKE_BINARY_DIR}/include 
		    )


ADD_SUBDIRECTORY(src)

INSTALL(FILES include/flowvr/daemon.h include/flowvr/telnet.h DESTINATION include/flowvr)

FOREACH(include-subdir plugd plugins)
 FILE(GLOB include-files include/flowvr/${include-subdir}/*.h)
 INSTALL(FILES ${include-files} DESTINATION include/flowvr/${include-subdir})
ENDFOREACH(include-subdir)
