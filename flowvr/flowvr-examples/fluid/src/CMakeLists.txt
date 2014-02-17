
INCLUDE_DIRECTORIES(../include ${FLOWVR_INCLUDE_DIR})


#  Fluid  MPI (parallel version)
IF(BUILD_MPI_FLUID)
  # SOURCE FILES
  SET(components-SRC   ${myapp}.cpp)
ENDIF(BUILD_MPI_FLUID)




# MODULE  BINARIES
IF(BUILD_MODULES)

  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
  
  ADD_EXECUTABLE(gldens  gldens.cpp)
  TARGET_LINK_LIBRARIES(gldens   ${FLOWVR_base_LIBRARY}  ${FLOWVR_ftl_LIBRARY} ${FLOWVR_mod_LIBRARY} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
  INSTALL(TARGETS gldens  DESTINATION ${MODULE_DIR})	
 
   # sequential fluid
  ADD_EXECUTABLE(fluid  fluid.cpp Vec3D.cpp Turbulent.cpp TurbulentBase.cpp)
  TARGET_LINK_LIBRARIES(fluid  ${FLOWVR_base_LIBRARY} ${FLOWVR_ftl_LIBRARY} ${FLOWVR_mod_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
  INSTALL(TARGETS fluid  DESTINATION ${MODULE_DIR})	

   # parallel fluid (same source but set USE_MPI variable)
   IF(BUILD_MPI_FLUID)

     INCLUDE_DIRECTORIES(${MPI_CXX_INCLUDE_PATH})
     ADD_EXECUTABLE(fluidmpi  fluid.cpp Vec3D.cpp Turbulent.cpp TurbulentBase.cpp)
     set_target_properties(fluidmpi PROPERTIES COMPILE_FLAGS -DUSE_MPI )
     TARGET_LINK_LIBRARIES(fluidmpi  ${FLOWVR_base_LIBRARY} ${FLOWVR_ftl_LIBRARY} ${FLOWVR_mod_LIBRARY} ${MPI_CXX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}) # issue on mac with openmpi package installed from port: MPI_CXX_LIBRARIES does not include /opt/local/lib/libmpi_cxx.dylib. Need to includ it manually.
     INSTALL(TARGETS fluidmpi  DESTINATION ${MODULE_DIR})	
   ENDIF(BUILD_MPI_FLUID)


ENDIF(BUILD_MODULES)
