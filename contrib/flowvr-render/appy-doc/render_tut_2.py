from flowvrapp import *
from balzac import *

renderer = BalzacLostInSpaceViewer("renderer")

# get the navigation module
navcomp = renderer.navcomp

# list its arguments
print navcomp.args

# get the position argument
print "position =", navcomp.get_arg('plugins-setcamparams-POSITION')

# change it 
navcomp.set_arg('plugins-setcamparams-POSITION', [0,0,-10])

print "new position =", navcomp.get_arg('plugins-setcamparams-POSITION')

# to set a background image
# also add
#    interface = "config/balzac-scene.xml"
# to the BalzacLostInSpaceViewer constructor
# renderer.balzac.set_arg("plugins-bgimage-IMAGENAME", "images/flowvr-logo-hr.png")

viewer = Module("viewer", cmdline = "bin/viewercube")
viewer.add_port("scene")

viewer.getPort("scene").link(renderer.new_scene_port())

app.generate_xml("rendertut")

