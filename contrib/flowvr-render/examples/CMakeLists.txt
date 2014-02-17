# EXAMPLES
# Copy all  examples (no compilation) in /share directory

#installation of the examples 
FOREACH(target tutorial viewers particles targetgroups  ) 
  INSTALL(DIRECTORY  ${target}
    DESTINATION  share/flowvr-render/examples/ PATTERN ".svn" EXCLUDE)
  INSTALL(PROGRAMS ${target}/make-app.sh 
    DESTINATION share/flowvr-render/examples/${target}/
    PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE
    GROUP_READ WORLD_READ ) # to make sure it has the correct execution rights
ENDFOREACH(target)

