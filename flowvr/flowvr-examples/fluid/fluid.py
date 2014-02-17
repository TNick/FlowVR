import sys

from flowvrapp import *

class Fluid(Module):
  " Module fluid computes the fluid dynamics (same module used for fluid and fluidmpi)"
  
  def __init__(self, name, nx, ny):
    Module.__init__(self, name, cmdline = "fluid %d %d" % (nx, ny))
    
    self.addPort("positions", direction = 'in');
    self.addPort("density", direction = 'out');
    self.addPort("velocity", direction = 'out');
    self.addPort("position", direction = 'out');


class GLDens(Module):
  "Module Gldens display the fluid and capture mouse events"

  def __init__(self, name):
    Module.__init__(self, name, cmdline = "gldens 0 0 1")
    self.addPort("positions", direction = 'out');
    self.addPort("density", direction = 'in');
    self.addPort("velocity", direction = 'in');    

nx, ny = 256, 256

if len(sys.argv) >= 3:
  nx, ny = int(sys.argv[1]), int(sys.argv[2])
  

fluid = Fluid("fluid", nx, ny)
gldens = GLDens("gldens")

# Connection from gldens to fluid simulation 
gldens.getPort("positions").link(fluid.getPort("positions"))


# DENSITY DATA TRANSFER
# link fluid (port density)  to connection 
fluid.getPort("density").link(gldens.getPort("density"))


# VELOCITY DATA TRANSFER
# link fluid (port velocity) to connection
fluid.getPort("velocity").link(gldens.getPort("velocity"))



app.generate_xml("fluid")
