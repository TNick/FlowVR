import os, pdb, sys

import flowvrapp, portutils, filters


class FilterMultiplexMerge(filters.FilterWithManyInputs):
  """ useful to merge several scene ports """
  
  plugin_name = 'flowvr.plugins.FilterMultiplexMerge'
  messagetype = 'full'

  def __init__(self, name, host = ''):
    filters.FilterWithManyInputs.__init__(self, name, host = host)
    self.addPort('trigger', direction = 'in', messagetype = 'stamps')        
    self.parameters["useSegments"] = False
    




class BalzacLostInSpaceViewer(flowvrapp.Component):

  def __init__(self, prefix,
               interface = 'balzac-normal.xml',
               navigate = 'balzac-lostinspacenavigate.xml'):    
    flowvrapp.Component.__init__(self)
    
    self.balzac = balzac = portutils.PortModule( prefix + '/BalzacBasic', interface, cmdline = "flowvr-balzac")

    # self.ports['scene'] = balzac.getPort('scene')
    self.ports['endIt'] = balzac.getPort('endIt')
    
    self.mmp = mmp = FilterMultiplexMerge( prefix + '/mmp')

    mmp.getPort('out').link(balzac.getPort("scene"))
    balzac.getPort('endIt').link(mmp.getPort('trigger'))

    self.navcomp = navcomp = portutils.PortModule( prefix + '/navigate', navigate)

    # navcomp.set_arg('plugins.setcamparams.CAMID', 25)

    balzac.getPort("devices").link(navcomp.getPort("devices"))
    balzac.getPort("viewport").link(navcomp.getPort("viewport"))

    navcomp.getPort("scene").link(mmp.newInputPort())

    # self.ports['scene_out'] = balzac.getPort('scene_out')
    self.ports['viewport'] = balzac.getPort('viewport')
   
    if 'transform' in navcomp.ports:
      self.ports['transform'] = navcomp.getPort('transform')

  def new_scene_port(self):
    "get a new port to plug scenes on"
    name = 'scene-%d' % self.mmp.parameters['nb']
    self.ports[name] = self.mmp.newInputPort()
    return self.ports[name]


class ModuleViewer(flowvrapp.Module):

  def __init__(self, name, run = None, cmdline = None, host = ''):
    flowvrapp.Module.__init__(self, name, run = run, cmdline = cmdline, host = host)
    self.addPort('scene', direction = 'out')
    self.addPort('matrix', direction = 'in')

class ModuleViewerObj(ModuleViewer):
  # requires viewerobj from the tutorial
  
  def __init__(self, name, meshname = '$FLOWVR_RENDER_PREFIX/share/flowvr-render/data/models/shuttledist16.fmesh', host = ''):
    run = flowvrapp.FlowvrRunSSH("viewerobj " + meshname)
    ModuleViewer.__init__(self, name, run, host = host)
  

if __name__ == '__main__':  

  # tutorial examples
  
  exampleId = int(sys.argv[1]) if len(sys.argv) > 1 else 1
  
  lisv = BalzacLostInSpaceViewer('tutorial/lostinspaceviewer')

  if exampleId == 1:
    scenemaker = ModuleViewer('tutorial/sceneproducer', flowvrapp.FlowvrRunSSH('viewercube'))
  elif exampleId == 2:
    scenemaker = ModuleViewerObj('tutorial/sceneproducer')
  elif exampleId == 3:
    scenemaker = ModuleViewer('tutorial/sceneproducer', flowvrapp.FlowvrRunSSH('viewertexture'))
  elif exampleId == 4:
    scenemaker = portutils.PortModule('tutorial/sceneproducer', 'multitexture.xml')
  elif exampleId == 5:
    scenemaker = portutils.PortModule("tutorial/sceneproducer", "textureupdate.xml")
    mf = filters.SyncMaxFrequency("mf")
    mf.getPort("out").link(scenemaker.getPort("beginIt"))
    scenemaker.getPort("endIt").link(mf.getPort("endIt"))
    mf.parameters["freq"] = 1
  else:
    assert False  

  scenemaker.getPort('scene').link(lisv.new_scene_port())

  flowvrapp.app.generate_xml("balzactutorial")
  
