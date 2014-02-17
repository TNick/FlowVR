## set things related to balzac

SET(FLOWVR_BALZAC_DIR ${CMAKE_SOURCE_DIR}/flowvr-render)
SET(FLOWVR_BALZAC_INCLUDE_DIR ${FLOWVR_BALZAC_DIR}/include/flowvr/render/balzac/servicelayer 
                       ${FLOWVR_BALZAC_DIR}/include/flowvr/render/balzac/data
                       ${FLOWVR_BALZAC_DIR}/include/flowvr/render/balzac 
                       ${FLOWVR_BALZAC_DIR}/include )
                       
SET(FLOWVR_BALZAC_COMP_INCLUDE_DIR ${FLOWVR_BALZAC_DIR}/include/flowvr/render/balzac/components )                     
                       
SET(FLOWVR_BALZAC_LIBRARY flowvr-balzacservicelayer)
SET(FLOWVR_BALZAC_filtermultiplexmerge_COMP_LIB filtermultiplexmerge.comp ) 
SET(FLOWVR_BALZAC_balzacbasic_COMP_LIB balzacbasic.comp ) 
SET(FLOWVR_BALZAC_balzaclostinspaceviewer_COMP_LIB balzaclostinspaceviewer.comp)

SET(FLOWVR_BALZAC_COMP_LIBRARY  ${FLOWVR_BALZAC_filtermultiplesmerge_COMP_LIB}  
                         ${FLOWVR_BALZAC_balzacbasic_COMP_LIB}
                         ${FLOWVR_BALZAC_balzaclostinspaceviewer_COMP_LIB} )
                         
SET(FLOWVR_BALZAC_AUX_LIBRARY flowvr-balzacmsgtypes )

SET(FLOWVR_BALZAC_LIBRARIES ${FLOWVR_BALZAC_LIBRARY} ${FLOWVR_BALZAC_COMP_LIBRARY} ${FLOWVR_BALZAC_AUX_LIBRARY} )


SET(FLOWVRRENDER_PREFIX ${CMAKE_BINARY_DIR} CACHE INTERNAL "Path to FlowVR Render root")


SET(FLOWVRRENDER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/flowvr-render/include
                             ${FLOWVR_BALZAC_INCLUDE_DIR} )
                             
SET(FLOWVRRENDER_LIBRARY flowvr-render )
SET(FLOWVR_render_comp_LIBRARY ${FLOWVR_BALZAC_COMP_LIBRARY} )

SET(FLOWVRRENDER_FOUND TRUE)

