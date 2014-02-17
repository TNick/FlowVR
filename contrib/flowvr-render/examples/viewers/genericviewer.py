import sys

from flowvrapp import *
from balzac import *
from portutils import *
"""
GenericViewer
"""

if (len(sys.argv) == 2):
  selectedViewer = sys.argv[1]
else:
  """
  Pass one of these viewers as argument:
  fire sphere1 sphere2 text1 text2 volume1 spheres
  """
  selectedViewer = "fire"
  
print "selectedViewer = %s" % selectedViewer
    
class GenericViewer(Module):
  def __init__(self, name, host = 'localhost'):
    cmdline = 'bin/%s' % selectedViewer
    Module.__init__(self, name, cmdline = cmdline, host = host)
    self.addPort("scene", direction = "out")
    

viewer = GenericViewer("viewer")

renderer = BalzacLostInSpaceViewer("renderer")
renderer.navcomp.set_arg('plugins-setcamparams-POSITION', [0,0,-3])

viewer.getPort("scene").link(renderer.new_scene_port())

app.generate_xml("genericviewer")
