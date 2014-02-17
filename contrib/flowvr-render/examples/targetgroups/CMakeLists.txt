PROJECT( TargetGroupsExample )

#Require cmake 2.8 at least
IF(DEFINED CMAKE_PATCH_VERSION)
  CMAKE_MINIMUM_REQUIRED(VERSION "2.8" FATAL_ERROR)
ELSE(DEFINED CMAKE_PATCH_VERSION)
  MESSAGE(FATAL_ERROR "Your version of CMake is too old, please use at least cmake 2.6")
ENDIF(DEFINED CMAKE_PATCH_VERSION)

SET(app targetgroups)

# INSTALLATION DIRECTORIES
SET(INCLUDE_DIR include)
SET(COMP_INCLUDE_DIR include/${app}/components) 
SET(LIB_DIR      lib)
SET(CONFIG_DIR config)
SET(COMP_LIB_DIR ${LIB_DIR}/components )
SET(PLUG_LIB_DIR ${LIB_DIR}/plugs )
SET(BIN_DIR    bin)
SET(MODULE_DIR bin)
SET(CMAKE_DIR share/${app}/cmake)
SET(DATA_DIR  data)

ADD_SUBDIRECTORY(src)

CONFIGURE_FILE( ${CONFIG_DIR}/app-config.sh.in bin/${app}-config.sh @ONLY )
INSTALL( PROGRAMS ${CMAKE_BINARY_DIR}/bin/${app}-config.sh DESTINATION bin
         PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE
                     GROUP_READ WORLD_READ )
