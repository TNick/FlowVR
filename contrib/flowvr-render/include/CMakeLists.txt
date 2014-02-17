FILE(GLOB HEADERS flowvr/render/*.h flowvr/render/*.inl)
INSTALL(FILES ${HEADERS} DESTINATION include/flowvr/render)

FILE(GLOB HEADERS flowvr/render/cg/*.h)
INSTALL(FILES ${HEADERS} DESTINATION include/flowvr/render/cg)

FILE(GLOB HEADERS flowvr/render/components/*.h)
INSTALL(FILES ${HEADERS} DESTINATION include/flowvr/render/components)

INSTALL(DIRECTORY flowvr/render/balzac DESTINATION include/flowvr/render PATTERN ".svn" EXCLUDE)
