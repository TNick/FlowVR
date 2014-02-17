import sys

from flowvrapp import *
from balzac import *
from portutils import *
"""
Particles
"""

class CreatorParticles(Module):
  def __init__(self, name, host = 'localhost'):
    cmdline = 'bin/creatorparticles'
    Module.__init__(self, name, cmdline = cmdline, host = host)
    self.addPort("points", direction = "out")
    self.addPort("colors", direction = "out")
    
    
class ViewerParticles(Module):
  def __init__(self, name, host = 'localhost'):
    cmdline = 'bin/viewerparticles2 --version=2'
    Module.__init__(self, name, cmdline = cmdline, host = host)
    self.addPort("points", direction = "in")
    self.addPort("colors", direction = "in")
    self.addPort("scene", direction = "out")
    
    
creator = CreatorParticles("creator")
viewer = ViewerParticles("viewer")
#viewer = PortModule("tutorial/sceneproducer", "config/textureupdate.xml")

creator.getPort("points").link(viewer.getPort("points"))
creator.getPort("colors").link(viewer.getPort("colors"))

renderer = BalzacLostInSpaceViewer("renderer")
renderer.navcomp.set_arg('plugins-setcamparams-POSITION', [0,0,-12])

viewer.getPort("scene").link(renderer.new_scene_port())

app.generate_xml("particles")
