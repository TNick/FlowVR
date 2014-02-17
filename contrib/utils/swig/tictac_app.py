
from flowvrapp import *

putmodule = Module("put", cmdline = "python tictac_module.py put")
outport = putmodule.add_port("text", direction = "out")

getmodule = Module("get", cmdline = "python tictac_module.py get")
inport = getmodule.add_port("text", direction = "in")

outport.link(inport)

spy = Module("spy", cmdline = "xterm -e python spy_module.py")
spy.add_port("in", direction = "in")

outport.link(spy.getPort("in"))


app.generate_xml("tictac")



