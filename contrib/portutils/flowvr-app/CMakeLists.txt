PROJECT(flowvr_app)

# Macro to build global header-file 
MACRO(build_header_file  directory file)
  SET(file flowvr-app.comp.h)
  FILE(GLOB header-files  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/include  ${directory}/*.h EXCEPT flowvr-app.comp.h)
  FOREACH(header ${header-files})
    SET(h-list   "${h-list}#include <${header}>\n")
  ENDFOREACH(header ${header-files})
  CONFIGURE_FILE(${directory}/${file}.in  ${flowvr_app_SOURCE_DIR}/${directory}/${file}  @ONLY)
ENDMACRO(build_header_file)

# build header file flowvr-app.h containing all core headers
build_header_file("include/flowvr/app/core" "flowvr-app.h" )
# build header file flowvr-app.comp.h containing all component  headers
build_header_file("include/flowvr/app/components" "flowvr-app.comp.h"  )

#install .h files
FOREACH(include-subdir app/core app/components)
 FILE(GLOB include-files include/flowvr/${include-subdir}/*.h)
 INSTALL(FILES ${include-files} DESTINATION include/flowvr/${include-subdir} )
ENDFOREACH(include-subdir)

ADD_SUBDIRECTORY(src)



