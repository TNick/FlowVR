# Viewers

SET(viewers-SRC skybox.cpp
                obj.cpp
                mesh.cpp
                grid.cpp 
                )

FOREACH(viewersrc ${viewers-SRC})
  STRING(REPLACE ".cpp" "" viewer ${viewersrc})
  ADD_EXECUTABLE(flowvr-viewer-${viewer} ${viewersrc})
   TARGET_LINK_LIBRARIES(flowvr-viewer-${viewer}  ${FLOWVR_mod_LIBRARY} flowvr-render)
  INSTALL(TARGETS flowvr-viewer-${viewer} DESTINATION share/flowvr-render/modules/bin)
ENDFOREACH(viewersrc)
