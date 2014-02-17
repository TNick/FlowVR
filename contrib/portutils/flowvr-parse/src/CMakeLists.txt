INCLUDE_DIRECTORIES(${flowvr_base_SOURCE_DIR}/include
		    ${flowvr_parse_SOURCE_DIR}/include)



# FlowVR graph
SET(INSTALL_PREFIX ${CMAKE_BINARY_DIR})
CONFIGURE_FILE(utils/flowvr-graph ${CMAKE_BINARY_DIR}/bin/flowvr-graph COPYONLY)
EXEC_PROGRAM(chmod ${CMAKE_BINARY_DIR}/bin ARGS 755 flowvr-graph OUTPUT_VARIABLE OUTPUT RETURN_VALUE return)
IF(return)
  MESSAGE("Error while setting executable attribute for flowvr-graph. Only installed version will be usable.")
ENDIF(return)

INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/bin/flowvr-graph DESTINATION bin)

# Perl support for flowvr
IF( BUILD_FLOWVR_PERL ) 
  CONFIGURE_FILE(perl/FlowVR/XML.pm ${CMAKE_BINARY_DIR}/lib/perl5/FlowVR/XML.pm COPYONLY)
  INSTALL(FILES ${CMAKE_BINARY_DIR}/lib/perl5/FlowVR/XML.pm DESTINATION lib/perl5/FlowVR)
ENDIF( BUILD_FLOWVR_PERL )

# Applications
SET(module-SRC module/builder.cpp module/flowvr-module.cpp)
ADD_EXECUTABLE(flowvr-module ${module-SRC})
TARGET_LINK_LIBRARIES(flowvr-module flowvr-base ${CMAKE_THREAD_LIBS_INIT})
INSTALL(TARGETS flowvr-module DESTINATION bin)

SET(network-SRC network/ParserCommands.cpp network/Objects.cpp 
		network/Deamons.cpp network/flowvr-network.cpp)
ADD_EXECUTABLE(flowvr-network ${network-SRC})
TARGET_LINK_LIBRARIES(flowvr-network flowvr-base ${CMAKE_THREAD_LIBS_INIT})
INSTALL(TARGETS flowvr-network DESTINATION bin)

#ADD_EXECUTABLE(flowvr-trace trace/flowvr-trace.cpp)
#TARGET_LINK_LIBRARIES(flowvr-trace flowvr-base ${CMAKE_THREAD_LIBS_INIT})
#INSTALL(TARGETS flowvr-trace DESTINATION bin)

#libflowvr-parse
SET(parse-SRC network/Objects.cpp network/Deamons.cpp network/ParserCommands.cpp)
ADD_LIBRARY(flowvr-parse SHARED ${parse-SRC})
TARGET_LINK_LIBRARIES(flowvr-parse flowvr-base)
SET_TARGET_PROPERTIES(flowvr-parse PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION})
INSTALL(TARGETS flowvr-parse LIBRARY DESTINATION lib)
