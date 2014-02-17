import sys

from flowvrapp import *
from filters import *


class FilterMerge2D(Filter):
   """Merges messages received on input port into one message sent on output port.
   """
  
   def __init__(self, name, host = ''):
     Filter.__init__(self, name, run = 'flowvr.plugins.Merge2D', host = host)
     self.addPort('in0', direction = 'in')
     self.addPort('in1', direction = 'in')
     self.addPort("out", direction = 'out')



class FluidMPI(Composite):
  """ several instances of a compute module, that compute primes
  together, and output them to the primesOut port""" 
  
  def __init__(self, hosts, nx, ny):
    Composite.__init__(self)

    prefix = "fluid"
    # hosts: string with host names, separated by spaces
    fluidrun = FlowvrRunOpenMPI("fluidmpi %d %d" % (nx, ny), hosts = hosts, prefix = prefix)

    # hosts_list: convert hosts to a list
    hosts_list = hosts.split(",")
    
    # nb of instances
    ninstance = len(hosts_list) 

    # collect ports
    all_positionsin = []
    all_positionout = []
    all_densityout = []
    all_velocityout = []

    for i in range(ninstance): 
      fluid = Module(prefix + "/" + str(i), run = fluidrun, host = hosts_list[i])
      fluid.addPort("positions", direction = 'in');
      fluid.addPort("density", direction = 'out');
      fluid.addPort("velocity", direction = 'out');
      fluid.addPort("position", direction = 'out');

      all_positionsin.append(fluid.getPort("positions"))
      all_positionout.append(fluid.getPort("positions"))
      all_velocityout.append(fluid.getPort("velocity"))
      all_densityout.append(fluid.getPort("density"))


    self.ports["positionsin"] =  list(all_positionsin);
    self.ports["positionout"] =  list(all_positionout);
    self.ports["velocity"] =  list(all_velocityout);
    self.ports["density"] =  list(all_densityout);


class GLDens(Module):
  "Module Gldens display the fluid and capture mouse events"

  def __init__(self, name):
    Module.__init__(self, name, cmdline = "gldens 0 0 1")
    self.addPort("positions", direction = 'out');
    self.addPort("density", direction = 'in');
    self.addPort("velocity", direction = 'in');    
    self.run.options += '-x DISPLAY'


    
# Main starts here ###########   
nx, ny = 256, 256

if len(sys.argv) >= 3:
  nx, ny = int(sys.argv[1]), int(sys.argv[2])
  

# Hostlist: comma separated for openmpi.  Add more hosts for more parallelism
fluidmpi = FluidMPI("localhost,localhost,localhost,localhost", nx, ny)
gldens = GLDens("gldens")

# POSITION DATA TRANSFER

# Broadcast positions to fluid
gldens.getPort("positions").link(fluidmpi.getPort("positionsin"))

# DENSITY DATA TRANSFER
# link fluid (port density)  to connection.   Comunication N to 1. Using MergeFilter to merge results

treeOut = generateNto1(prefix="comNto1DensityMerge", in_ports = fluidmpi.getPort("density"), arity = 2, node_class = FilterMerge2D)
treeOut.link(gldens.getPort("density"))

# VELOCITY DATA TRANSFER
# link fluid (port velocity) to connection

treeOut = generateNto1(prefix="comNto1VelocityMerge", in_ports = fluidmpi.getPort("velocity"), arity = 2, node_class = FilterMerge2D)
treeOut.link(gldens.getPort("velocity"))

app.generate_xml("fluidmpi")
