ADD_SUBDIRECTORY(src)



FILE(GLOB include-files include/flowvr/*.h)
INSTALL(FILES ${include-files} DESTINATION include/flowvr/utils/gltrace)

