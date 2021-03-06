# - Try to find CEGUI
# Once done this will define
#
#  CEGUI_FOUND - system has LUA
#  CEGUI_INCLUDE_DIR - the LUA include directory
#  CEGUI_LIBRARIES - Link these to use LUA
#


FIND_PATH(CEGUI_INCLUDE_DIR CEGUIBase.h
  PATHS
  /usr/include/CEGUI
  /usr/local/include/CEGUI
)

FIND_LIBRARY(CEGUI_LIBRARIES
   NAMES CEGUIBase
  PATHS
  /usr/lib
  /usr/local/lib
)

FIND_LIBRARY(CEGUI_LUALIBRARIES
   NAMES CEGUILuaScriptModule
   PATHS
   /usr/lib
   /usr/local/lib
)

IF(NOT CEGUI_LUALIBRARIES)
 MESSAGE( FATAL_ERROR "CegRenderer needs Lua bindings activated in CEGUI" )
ELSE(NOT CEGUI_LUALIBRARIES )
 SET( CEGUI_LIBRARIES ${CEGUI_LIBRARIES} ${CEGUI_LUALIBRARIES} )
 MESSAGE( STATUS "Found CEGUI LuaLibraries in ${CEGUI_LUALIBRARIES}" )
 MESSAGE( STATUS "Setting CEGUI_LIBRARIES to ${CEGUI_LIBRARIES}" )
ENDIF( NOT CEGUI_LUALIBRARIES )

IF(CEGUI_INCLUDE_DIR AND CEGUI_LIBRARIES)
   SET(CEGUI_FOUND TRUE)
   SET(CEGUI_DIR ${CEGUI_INCLUDE_DIR})
ENDIF(CEGUI_INCLUDE_DIR AND CEGUI_LIBRARIES)
 

# show the LUA_INCLUDE_DIR and LUA_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(CEGUI_INCLUDE_DIR CEGUI_LIBRARIES)
