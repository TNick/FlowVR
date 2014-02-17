PROJECT(flowvr_parse)


ADD_SUBDIRECTORY(src)

INSTALL(FILES include/flowvr/parse/Deamons.h include/flowvr/parse/ParserCommands.h include/flowvr/parse/Objects.h DESTINATION include/flowvr/parse)



# FILE(GLOB dtd-files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} dtd/*.dtd dtd/*.cat)
# FOREACH(dtd-file ${dtd-files})
#   CONFIGURE_FILE(${dtd-file} ${CMAKE_BINARY_DIR}/share/flowvr/parse/${dtd-file})
#   INSTALL(FILES ${CMAKE_BINARY_DIR}/share/flowvr/parse/${dtd-file} DESTINATION share/flowvr/parse/dtd)
# ENDFOREACH(dtd-file)

# CONFIGURE_FILE(xsl/flowvr-net2dot.xsl ${CMAKE_BINARY_DIR}/share/flowvr/parse/xsl/flowvr-net2dot.xsl)
# INSTALL(FILES ${CMAKE_BINARY_DIR}/share/flowvr/parse/xsl/flowvr-net2dot.xsl
#         DESTINATION share/flowvr/parse/xsl)
