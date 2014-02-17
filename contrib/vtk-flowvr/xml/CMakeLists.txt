SET(desc-FILES vtk.desc.xml vtkpara.desc.xml vtkflowvr.desc.xml)
FOREACH(desc ${desc-FILES})
  CONFIGURE_FILE(${desc} share/vtk-flowvr/modules/desc/${desc} COPYONLY)
ENDFOREACH(desc ${desc-FILES})

INSTALL(FILES ${desc-FILES} DESTINATION share/vtk-flowvr/modules/desc)
