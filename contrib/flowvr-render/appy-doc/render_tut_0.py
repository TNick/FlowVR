from flowvrapp import *
from balzac import *

renderer = BalzacLostInSpaceViewer("renderer")

# bin/viewercube is the viewer executable
viewer = Module("viewer", cmdline = "bin/viewercube")

# other simple examples: 
# viewer = Module("viewer", cmdline = "bin/viewertexture")
# viewer = Module("viewer", cmdline = "bin/viewerobj $FLOWVR_RENDER_PREFIX/share/flowvr-render/data/models/shuttledist16.fmesh")

viewer.add_port("scene")
viewer.add_port("matrix")

# a renderer can have many scene input ports, so just ask for one
viewer.getPort("scene").link(renderer.new_scene_port())

app.generate_xml("rendertut")

