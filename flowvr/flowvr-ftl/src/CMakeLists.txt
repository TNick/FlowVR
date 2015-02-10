INCLUDE_DIRECTORIES(${flowvr_base_SOURCE_DIR}/include
		    ${flowvr_ftl_SOURCE_DIR}/include
		    ${CMAKE_BINARY_DIR}/include )


SET(ftlm-SRC ftlm/type.cpp ftlm/quat.cpp ftlm/crc.cpp chunk.cpp
             chunkreader.cpp chunkwriter.cpp chunkevents.cpp)
ADD_LIBRARY(ftlm SHARED ${ftlm-SRC})
TARGET_LINK_LIBRARIES(ftlm flowvr-base)
SET_TARGET_PROPERTIES(ftlm PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION})
INSTALL(TARGETS ftlm LIBRARY DESTINATION lib RUNTIME DESTINATION bin ARCHIVE DESTINATION lib)
