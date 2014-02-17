# TUTORIAL EXAMPLE 

INCLUDE_DIRECTORIES(../include ${FLOWVR_INCLUDE_DIR})

# MODULE  BINARIES

IF(BUILD_MODULES)
  
  
   FOREACH(target viewercube viewertexture viewerobj multitexture textureupdate) 
        ADD_SUBDIRECTORY(${target})  
   ENDFOREACH(target)
  
ENDIF(BUILD_MODULES)
