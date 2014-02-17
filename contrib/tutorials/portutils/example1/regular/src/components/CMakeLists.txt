INCLUDE_DIRECTORIES(../../include ${FLOWVR_INCLUDE_DIR})

SET( app producerconsumerregular )

SET( components-SRC producerconsumerregular.comp.cpp
                    metamoduleconsumer.comp.cpp
                    metamoduleproducer.comp.cpp
                    moduleproducer.comp.cpp
                    moduleconsumer.comp.cpp )
                    
# COMPONENT LIBRARY
ADD_LIBRARY(${app}.comp SHARED  ${components-SRC})
# Should work but Linux complain about not finding dl lib used by flowvr-plugd
TARGET_LINK_LIBRARIES(${app}.comp ${FLOWVR_base_LIBRARY} 
                                  ${FLOWVR_mod_LIBRARY}  
                                  ${FLOWVR_ftl_LIBRARY} 
                                  ${FLOWVR_app_LIBRARY}   
                                  ${FLOWVR_app_comp_LIBRARY}  )

INSTALL(TARGETS ${app}.comp LIBRARY DESTINATION ${COMP_LIB_DIR} )