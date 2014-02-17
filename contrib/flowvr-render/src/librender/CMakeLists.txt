# Render LIBRARY
SET(librender-SRC bbox.cpp chunk.cpp
                  chunkrenderreader.cpp chunkrenderwriter.cpp
		  texfont.cpp engine.cpp
		  font.cpp mesh.cpp
		  mesh_dist.cpp mesh_io_mesh.cpp
		  mesh_io_obj.cpp mesh_io_off.cpp
		  mesh_io_stl.cpp cg/cgengine.cpp)

#SET_SOURCE_FILES_PROPERTIES(librender/cg/cgengine.cpp PROPERTIES COMPILE_FLAGS -DFLOWVR_RENDER_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")
#SET_SOURCE_FILES_PROPERTIES(librender/chunkwriter.cpp PROPERTIES COMPILE_FLAGS -DFLOWVR_RENDER_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

ADD_LIBRARY(flowvr-render SHARED ${librender-SRC})
TARGET_LINK_LIBRARIES(flowvr-render ${Devil_LIBRARIES} 
                                    ${FLOWVR_ftl_LIBRARY} 
                                    ${FLOWVR_base_LIBRARY} 
                                    ${OPENGL_LIBRARIES} 
                                    ${CG_LIBRARY} 
                                    ${CG_GL_LIBRARY} 
                                    ${GLEW_LIBRARIES})
                                     
SET_TARGET_PROPERTIES(flowvr-render PROPERTIES SOVERSION "${PROJECT_VERSION_MAJOR}" VERSION "${PROJECT_VERSION}")
INSTALL(TARGETS flowvr-render LIBRARY DESTINATION lib)
