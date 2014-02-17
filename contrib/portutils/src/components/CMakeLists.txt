PROJECT(flowvr_portutils_comp)

INCLUDE_DIRECTORIES(
  ${flowvr_portutils_SOURCE_DIR}/include
  ${flowvr_app_SOURCE_DIR}/include
  )

LINK_LIBRARIES( ${FLOWVR_app_LIBRARY} )

SET(compname flowvr-portmodule.comp )

ADD_LIBRARY(${compname} SHARED portmodule.comp.cpp 
                                   portmodulerun.comp.cpp 
                                   portparallel.comp.cpp
                                   portinterfacecache.cpp)
                                   
TARGET_LINK_LIBRARIES(${compname}
                      flowvr-portutils
                      flowvr-base
                      ftlm
                      flowvr-app
                      flowvr-app.comp
                     )
                      
	                      
#SET_TARGET_PROPERTIES(${compname} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION})
INSTALL(TARGETS ${compname} LIBRARY DESTINATION lib)
