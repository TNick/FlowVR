from flowvrapp import *
from balzac import *
from portutils import *

renderer = BalzacLostInSpaceViewer("renderer")

viewer = PortModule("tutorial/sceneproducer", "config/multitexture.xml")

viewer.getPort("scene").link(renderer.new_scene_port())

app.generate_xml("rendertut")

