
## ADDING PIPOL TESTING STUFFS (declared as an advanced option for now)
OPTION(USE_PIPOL "Do you want to use pipol for tssting compilations ?")
MARK_AS_ADVANCED(USE_PIPOL)

IF(USE_PIPOL)

 # ssh/rsync mandatory 
FIND_PROGRAM(HAVE_SSH ssh)
FIND_PROGRAM(HAVE_RSYNC rsync)

# user needs to have a pipol account 
SET(PIPOL_USER "" CACHE STRING "pipol login?")

  IF(PIPOL_USER)
    MESSAGE(STATUS "Pipol user is ${PIPOL_USER}")

  # get pipol systems    
  IF(HAVE_SSH)
      EXECUTE_PROCESS(COMMAND 
      ssh ${PIPOL_USER}@pipol.inria.fr pipol-sub --query=systems 
      OUTPUT_VARIABLE PIPOL_SYSTEMS OUTPUT_STRIP_TRAILING_WHITESPACE)
   ENDIF(HAVE_SSH)
      
   #set macro 
   IF(HAVE_RSYNC)
      MACRO(PIPOL_TARGET SYSTEM_PATTERN)
       
      # defaults
      IF(NOT PIPOL_CONFIGURE_COMMAND)
        SET(PIPOL_CONFIGURE_COMMAND cmake ${CMAKE_SOURCE_DIR} -DCMAKE_INSTALL_PREFIX:PATH=/pipol/flowvr-suite/install-suite   -DBUILD_FLOWVR_RENDER:BOOL=ON  -DBUILD_FLOWVR_GLGRAPH:BOOL=ON  -DBUILD_FLOWVR_RENDER_MPLAYER:BOOL=OFF -DBUILD_FLOWVR_VRPN:BOOL=OFF -DBUILD_VTK_FLOWVR:BOOL=OFF -DCG_COMPILER:PATH=/home/flowvr/avanel/dep_64/usr/bin/cgc -DGLUT_Xi_LIBRARY:FILEPATH=/usr/lib/libXi.so -DGLUT_Xmu_LIBRARY:FILEPATH=/usr/lib/libXmu.so.6)
      ENDIF(NOT PIPOL_CONFIGURE_COMMAND)
      
      IF(NOT PIPOL_MAKE_COMMAND)
        SET(PIPOL_MAKE_COMMAND make -j 2 -$(MAKEFLAGS))
      ENDIF(NOT PIPOL_MAKE_COMMAND)

      IF(NOT PIPOL_MAKE_TEST_COMMAND)
        SET(PIPOL_MAKE_TEST_COMMAND make -$(MAKEFLAGS) test)
      ENDIF(NOT PIPOL_MAKE_TEST_COMMAND)

      IF(NOT PIPOL_MAKE_INSTALL_COMMAND)
        SET(PIPOL_MAKE_INSTALL_COMMAND sudo make -$(MAKEFLAGS) install)
      ENDIF(NOT PIPOL_MAKE_INSTALL_COMMAND)

      IF(NOT PIPOL_POST_INSTALL_COMMAND)
        SET(PIPOL_POST_INSTALL_COMMAND sudo chown ${PIPOL_USER} install_manifest.txt) 
      ENDIF(NOT PIPOL_POST_INSTALL_COMMAND)

      IF(NOT PIPOL_PACKAGE_COMMAND)
        SET(PIPOL_PACKAGE_COMMAND cpack -G \\\$$PIPOL_CPACK_G .)
      ENDIF(NOT PIPOL_PACKAGE_COMMAND)

      IF(NOT PIPOL_SUB_RSYNC_OPTIONS)
        SET(PIPOL_SUB_RSYNC_OPTIONS "-a")
      ENDIF(NOT PIPOL_SUB_RSYNC_OPTIONS)

      STRING(REPLACE ".dd.gz" "" SYSTEM_TARGET ${SYSTEM_PATTERN})
            
      SET(MY_TARGET "")
      STRING(REPLACE "pipol_" "" MY_TARGET "${SYSTEM_PATTERN}")

      ADD_CUSTOM_TARGET( ${SYSTEM_TARGET}
        COMMENT "PIPOL Build : ${MY_TARGET}"
        COMMENT "Source Dir : ${CMAKE_SOURCE_DIR}"
        COMMAND rsync ${PIPOL_USER}@pipol.inria.fr:/usr/local/bin/pipol-sub . 
        COMMAND ./pipol-sub --pipol-user=${PIPOL_USER} ${MY_TARGET} 02:00 --reconnect --group --keep --verbose=1 --export=${CMAKE_SOURCE_DIR} ${PIPOL_RC_DIR_OPTION} --rsynco=${PIPOL_SUB_RSYNC_OPTIONS}
        \"sudo mkdir -p \\\$$PIPOL_WDIR/${PIPOL_USER}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME} \;
        sudo chown ${PIPOL_USER} \\\$$PIPOL_WDIR/${PIPOL_USER}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME} \;
        . ~/call_install_dep.sh \;
        cd \\\$$PIPOL_WDIR/${PIPOL_USER}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME} \;
        ${PIPOL_CONFIGURE_COMMAND} \;
        ${PIPOL_MAKE_COMMAND} \; 
        ${PIPOL_MAKE_INSTALL_COMMAND} \"
      )
        
    ENDMACRO(PIPOL_TARGET)
   ENDIF(HAVE_RSYNC)

        # add a target for each pipol system
        IF(PIPOL_SYSTEMS)
          MESSAGE(STATUS "Adding Pipol targets")
          FOREACH(SYSTEM ${PIPOL_SYSTEMS})
            # target with rc-dir
            SET(SYSTEM "pipol_${SYSTEM}")
            PIPOL_TARGET(${SYSTEM})
          ENDFOREACH(SYSTEM ${PIPOL_SYSTEMS})
        ENDIF(PIPOL_SYSTEMS)
        
 ELSE(PIPOL_USER)
     MESSAGE(FATAL_ERROR "You need a pipol login to use this functionnality : USE_PIPOL ")
 ENDIF(PIPOL_USER)
        
ENDIF(USE_PIPOL)