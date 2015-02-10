# FLOWVR BASE LIBRARY


 if(POLICY CMP0046)
   # some files are not yet generated on first CMake run, resulting in CMake warning
   # old behaviour hides the warning
   cmake_policy(SET CMP0046 OLD)
 endif(POLICY CMP0046)

INCLUDE_DIRECTORIES("${flowvr_base_SOURCE_DIR}/include" 
		    "${flowvr_base_BINARY_DIR}/include" 
                    "${CMAKE_BINARY_DIR}/include" 
                    "${CMAKE_SOURCE_DIR}/flowvr/flowvr-ftl/include" #flowvr-ftl_SOURCE_DIR hasn't been set yet.
                   )

STRING(TOLOWER ${CMAKE_SYSTEM_NAME} system-NAME)

STRING(TOUPPER ${CMAKE_SYSTEM_NAME} TOPOLOGY_SYS)

INCLUDE(CheckTypeSize)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckIncludeFiles)


CHECK_TYPE_SIZE( "unsigned long" TOPOLOGY_SIZEOF_UNSIGNED_LONG)
CHECK_TYPE_SIZE( "unsigned int" TOPOLOGY_SIZEOF_UNSIGNED_INT)

CHECK_FUNCTION_EXISTS(ffs  TOPOLOGY_HAVE_FFS)
CHECK_FUNCTION_EXISTS(ffs  TOPOLOGY_HAVE_DECL_FFS)
CHECK_FUNCTION_EXISTS(ffsl TOPOLOGY_HAVE_FFSL)


CHECK_FUNCTION_EXISTS(sched_getcpu _SCHED_HAVE_GETCPU)
CHECK_FUNCTION_EXISTS(cpu_number _SCHED_HAVE_CPUNUMBER)


# we definitely want to have a value on the left hand side
# for SCHED_HAVE_GETCPU either 0 or 1

IF( ${_SCHED_HAVE_GETCPU} )
 # note: we remove the "_" prefix
 SET( SCHED_HAVE_GETCPU 1 )
ELSE( ${_SCHED_HAVE_GETCPU} )
 # note: we remove the "_" prefix
 SET( SCHED_HAVE_GETCPU 0 )
ENDIF( ${_SCHED_HAVE_GETCPU} )


# this is a test for darwin
IF( ${_SCHED_HAVE_CPUNUMBER} )
 # note: we remove the "_" prefix
 SET( SCHED_HAVE_CPUNUMBER 1 )
ELSE( ${_SCHED_HAVE_CPUNUMBER} )
 # note: we remove the "_" prefix
 SET( SCHED_HAVE_CPUNUMBER 0 )
ENDIF( ${_SCHED_HAVE_CPUNUMBER} )

CHECK_FUNCTION_EXISTS(host_info _TOPO_HAVE_HOST_INFO)

IF( ${_TOPO_HAVE_HOST_INFO} )
 # note: we remove the "_" prefix
 SET( TOPO_HAVE_HOST_INFO 1 )
ELSE( ${_TOPO_HAVE_HOST_INFO} )
 # note: we remove the "_" prefix
 SET( TOPO_HAVE_HOST_INFO 0 )
ENDIF( ${_TOPO_HAVE_HOST_INFO} )


CHECK_INCLUDE_FILE(alloca.h TOPO_HAVE_ALLOCA_H)

CHECK_SYMBOL_EXISTS(_SC_NPROCESSORS_ONLN unistd.h _TOPO_HAVE_DECL__SC_NPROCESSORS_ONLN)
CHECK_SYMBOL_EXISTS(_SC_NPROC_ONLN unistd.h _TOPO_HAVE_DECL__SC_NPROC_ONLN)
CHECK_SYMBOL_EXISTS(_SC_NPROCESSORS_CONF unistd.h _TOPO_HAVE_DECL__SC_NPROCESSORS_CONF)
CHECK_SYMBOL_EXISTS(_SC_NPROC_CONF unistd.h _TOPO_HAVE_DECL__SC_NPROC_CONF)
CHECK_SYMBOL_EXISTS(_ATFILE_SOURCE unistd.h _TOPO_DEFINE_ATFILE_SOURCE)

IF( ${_TOPO_HAVE_DECL__SC_NPROCESSORS_ONLN} )
 # note: we remove the "_" prefix
 SET( TOPO_HAVE_DECL__SC_NPROCESSORS_ONLN 1 )
ELSE( ${_TOPO_HAVE_DECL__SC_NPROCESSORS_ONLN} )
 # note: we remove the "_" prefix
 SET( TOPO_HAVE_DECL__SC_NPROCESSORS_ONLN 0 )
ENDIF( ${_TOPO_HAVE_DECL__SC_NPROCESSORS_ONLN} )

IF( ${_TOPO_HAVE_DECL__SC_NPROC_ONLN} )
 # note: we remove the "_" prefix
 SET( TOPO_HAVE_DECL__SC_NPROC_ONLN 1 )
ELSE( ${_TOPO_HAVE_DECL__SC_NPROC_ONLN} )
 # note: we remove the "_" prefix
 SET( TOPO_HAVE_DECL__SC_NPROC_ONLN 0 )
ENDIF( ${_TOPO_HAVE_DECL__SC_NPROC_ONLN} )


IF( ${_TOPO_HAVE_DECL__SC_NPROCESSORS_CONF} )
 # note: we remove the "_" prefix
 SET( TOPO_HAVE_DECL__SC_NPROCESSORS_CONF 1 )
ELSE( ${_TOPO_HAVE_DECL__SC_NPROCESSORS_CONF} )
 # note: we remove the "_" prefix
 SET( TOPO_HAVE_DECL__SC_NPROCESSORS_CONF 0 )
ENDIF( ${_TOPO_HAVE_DECL__SC_NPROCESSORS_CONF} )

IF( ${_TOPO_HAVE_DECL__SC_NPROC_CONF} )
 # note: we remove the "_" prefix
 SET( TOPO_HAVE_DECL__SC_NPROC_CONF 1 )
ELSE( ${_TOPO_HAVE_DECL__SC_NPROC_CONF} )
 # note: we remove the "_" prefix
 SET( TOPO_HAVE_DECL__SC_NPROC_CONF 0 )
ENDIF( ${_TOPO_HAVE_DECL__SC_NPROC_CONF} )

IF( ${_TOPO_DEFINE_ATFILE_SOURCE} )
 # note: we remove the "_" prefix
 SET( TOPO_DEFINE_ATFILE_SOURCE 1 )
ELSE( ${_TOPO_DEFINE_ATFILE_SOURCE} )
 # note: we remove the "_" prefix
 SET( TOPO_DEFINE_ATFILE_SOURCE 0 )
ENDIF( ${_TOPO_DEFINE_ATFILE_SOURCE} )



SET(TOPOLOGY_NBMAXCPUS 1024)
SET(TOPOLOGY_CACHE_LEVEL_MAX 3)


CONFIGURE_FILE(../include/flowvr/topology/config.h.in ${flowvr_base_BINARY_DIR}/include/flowvr/topology/config.h)
INSTALL(FILES ${flowvr_base_BINARY_DIR}/include/flowvr/topology/config.h DESTINATION include/flowvr/topology)

FILE(GLOB topology-includes-priv ${CMAKE_BINARY_DIR}/include/flowvr/topology/*.h)
FILE(GLOB topology-includes ${topology-includes-priv} ../include/flowvr/topology/*.h)

INSTALL(FILES ${topology-includes} DESTINATION include/flowvr/topology)
#INSTALL(FILES ${flowvr_base_SOURCE_DIR}/include/flowvr/topology/topology.h DESTINATION include/flowvr/topology)


###############################
## CONFIGURE MULTIBUF HANDLING
###############################

# TCP
CHECK_FUNCTION_EXISTS(writev  _FLOWVR_OS_HAVE_WRITEV)

CHECK_SYMBOL_EXISTS( HOST_NAME_MAX "unistd.h;sys/limits.h" _FLOWVR_OS_HAVE_HOST_NAME_MAX)

IF( NOT ${_FLOWVR_OS_HAVE_HOST_NAME_MAX} )
 CHECK_SYMBOL_EXISTS( _POSIX_HOST_NAME_MAX "sys/limits.h;unistd.h" _FLOWVR_OS_HAVE_HOST_NAME_MAX )
ENDIF( NOT ${_FLOWVR_OS_HAVE_HOST_NAME_MAX} )

IF( ${_FLOWVR_OS_HAVE_HOST_NAME_MAX} )
 SET( _OS_HOST_NAME_MAX ${_FLOWVR_OS_HAVE_HOST_NAME_MAX} )
ELSE( ${_FLOWVR_OS_HAVE_HOST_NAME_MAX} )
 SET( _OS_HOST_NAME_MAX 256 )
ENDIF( ${_FLOWVR_OS_HAVE_HOST_NAME_MAX} )


CONFIGURE_FILE(../include/flowvr/utils/multibuf.h.in ${CMAKE_BINARY_DIR}/include/flowvr/utils/multibuf.h)


SET(base-SRC allocator.cpp 
             stamp.cpp 
             trace.cpp 
             message.cpp 
             buffer.cpp 
             bufferpool.cpp 
             bufferimp.cpp

             daemondata.cpp
             thread.cpp 
             portreader.cpp 
             portwriter.cpp
              
             xml.cpp 

             mem/sharedmemorybuffer.cpp 
             mem/sharedmemoryarea.cpp
             mem/sharedmemorymanager.cpp 
             mem/daemonsharedmemorymanager.cpp 
             mem/mpdata.cpp
             mem/memorybuffer.cpp
             mem/memorymanager.cpp
             
             utils/tcptools.cpp
             utils/backtrace.cpp
             utils/timing.cpp 
             utils/cmdline.cpp
             utils/filepath.cpp             
             utils/size.cpp             
                          
             topo.cpp
             topology/bind.c topology/dolib.c topology/topology.c
             topology/topology-${system-NAME}.c topology/topology-xml.c
             topology/traversal.c topology/topology-synthetic.c
             )
             

ADD_LIBRARY(flowvr-base SHARED ${base-SRC})
ADD_DEPENDENCIES(flowvr-base ${CMAKE_BINARY_DIR}/include/flowvr/topology/config.h)

LINK_DIRECTORIES( ${CMAKE_BUILD_PATH}/lib )


# check for special functions used in timing.cpp
# we have to take care: the -lrt is somehow a standard,
# but at the same time not available on all systems
# (for example mac)

SET(TARGET_LIBRARIES flowvr-base ${CMAKE_THREAD_LIBS_INIT})

FIND_LIBRARY(RT_LIB rt)

# have to make that an OR clause to check for RT_LIB_FOUND as well
# as checking the RT_LIB value itself. Might be a problem of my cmake (2.6)?

IF(RT_LIB_FOUND OR RT_LIB)
  SET(TARGET_LIBRARIES ${TARGET_LIBRARIES} ${RT_LIB})
ENDIF(RT_LIB_FOUND OR RT_LIB)

TARGET_LINK_LIBRARIES( ${TARGET_LIBRARIES}  )

SET_TARGET_PROPERTIES(flowvr-base PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION}) 

#ADD_LIBRARY(flowvr-utils SHARED ext/filepath.cpp)
#TARGET_LINK_LIBRARIES(flowvr-utils flowvr-base)
#SET_TARGET_PROPERTIES(flowvr-utils PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION} COMPILE_FLAGS -DFLOWVR_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")
#INSTALL(TARGETS flowvr-utils LIBRARY DESTINATION lib)

# FLOWVR MODULE LIBRARY
SET(mod-SRC moduleapi.cpp moduleapifactory.cpp moduleapimultiprocessimpl.cpp
            moduleapifileimpl.cpp module.cpp parallel.cpp)
ADD_LIBRARY(flowvr-mod SHARED ${mod-SRC})
TARGET_LINK_LIBRARIES(flowvr-mod flowvr-base ${CMAKE_THREAD_LIBS_INIT})
SET_TARGET_PROPERTIES(flowvr-mod PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION})

INSTALL(TARGETS flowvr-base flowvr-mod LIBRARY DESTINATION lib RUNTIME DESTINATION bin ARCHIVE DESTINATION lib)
