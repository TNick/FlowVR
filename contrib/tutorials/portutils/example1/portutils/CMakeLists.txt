PROJECT( example1-portutils )
CMAKE_MINIMUM_REQUIRED( VERSION "2.6" FATAL_ERROR )

SET( CMAKE_MODULE_PATH ${FLOWVR_PREFIX}/cmake
                       ${FLOWVR_PREFIX}/share/flowvr-contrib/cmake 
                       ${CMAKE_MODULE_PATH} )

FIND_PACKAGE( FlowVR REQUIRED )

SET( COMP_LIB_DIR components )
SET( MODULE_DIR bin )

ADD_SUBDIRECTORY(src)

