from flowvrapp import *
from balzac import *
from portutils import *

renderer = BalzacLostInSpaceViewer("renderer")

viewer = PortModule("tutorial/sceneproducer", "config/textureupdate.xml")

# put a feedback loop to limit the thoughput of the texutre update
mf = filters.SyncMaxFrequency("maxfreq")
mf.parameters["freq"] = 4

mf.getPort("out").link(viewer.getPort("beginIt"))
viewer.getPort("endIt").link(mf.getPort("endIt"))


viewer.getPort("scene").link(renderer.new_scene_port())

app.generate_xml("rendertut")

