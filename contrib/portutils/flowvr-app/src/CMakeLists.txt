# FLOWVR APP and APP.COMP LIBRARIES

INCLUDE_DIRECTORIES(${flowvr_base_SOURCE_DIR}/include 
                    ${flowvrd_SOURCE_DIR}/include  
                    ${flowvr_ftl_SOURCE_DIR}/include 
                    ${flowvr_modules_SOURCE_DIR}/include 
                    ${flowvr_parse_SOURCE_DIR}/include 
                    ${FLOWVR_commands_LIBRARY} 
                    ${flowvr_app_SOURCE_DIR}/include  
		    "${CMAKE_SOURCE_DIR}/flowvr/flowvr-modules/include" #flowvr-modules_SOURCE_DIR hasn't been set yet. 
                    )



# FLOWVR APP BINARY 

#FLOWVR-TEST LIBAPP
# keep it as it can be a useful piece of code, but don't compile it and expose the binary.
#This binary has the flowvr-app included part of flowvr-app library with static compilation
#ADD_EXECUTABLE(flowvr-test testlib.cpp msg.cpp exception.cpp port.cpp component.cpp data.cpp  class.cpp controllerlog.cpp parameter.cpp parameterlist.cpp)
#TARGET_LINK_LIBRARIES(flowvr-test ftlm flowvr-base flowvr-utils flowvr-parse ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
#INSTALL(TARGETS flowvr-test RUNTIME DESTINATION bin)
	



#CORE LIBRARY
SET(app-SRC msg.cpp
	   exception.cpp
	   port.cpp
       data.cpp
	   arch.cpp
	   spreadsheet.cpp
	   class.cpp
	   run.cpp
	   controllerlog.cpp
       parameter.cpp
	   parameterlist.cpp
       flowvr-app.cpp
	   connection.comp.cpp
       filter.comp.cpp
	   module.comp.cpp
	   component.cpp
	   applicationTracer.cpp)
	

ADD_LIBRARY(flowvr-app SHARED ${app-SRC})
TARGET_LINK_LIBRARIES(flowvr-app  ftlm flowvr-base)
SET_TARGET_PROPERTIES(flowvr-app PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION})
INSTALL(TARGETS flowvr-app LIBRARY DESTINATION lib )


#BASIC FLOWVR COMPONENTS
SET(components-SRC	
		connection.comp.cpp
		filter.comp.cpp
		module.comp.cpp
		sync.comp.cpp
		filterrank.comp.cpp
		filterrankprimitive.comp.cpp
		filtermax.comp.cpp
		filtermerge.comp.cpp
		filtermergeindexpos.comp.cpp
		filtermultiplex.comp.cpp
		filtermerge2d.comp.cpp
		filterfirstonly.comp.cpp
		filtersignaland.comp.cpp
		filterroutingnode.comp.cpp
		filtermaskrouting.comp.cpp
		filtermaskroutingprimitive.comp.cpp
		filterrotate.comp.cpp
		filterrank.comp.cpp
		filterpresignal.comp.cpp
		filterscatter.comp.cpp
		filterfrequency.comp.cpp
		filterlogger.comp.cpp
		routingnode.comp.cpp
        patternsyncbase.comp.cpp
        filterlogger.comp.cpp
        flowvr-app.comp.cpp)


ADD_LIBRARY(flowvr-app.comp SHARED ${components-SRC})
TARGET_LINK_LIBRARIES(flowvr-app.comp ftlm flowvr-base flowvr-app)
#SET_TARGET_PROPERTIES(flowvr-app.comp PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION})
INSTALL(TARGETS flowvr-app.comp LIBRARY DESTINATION lib )


