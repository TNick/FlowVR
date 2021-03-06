import sys

from flowvrapp import *
from filters import *

# parameters
example = 4 if len(sys.argv) < 2 else int(sys.argv[1])

# used only for example 8
compute_hosts = 'localhost' if len(sys.argv) < 3 else sys.argv[2]

if example == 1:
  # plain examples without composites 
  # A FlowvrRunSSHMultiple object is required for the compute module
  # because it is designed to be run in several instances
  computerun = FlowvrRunSSHMultiple('bin/compute', hosts = 'localhost', prefix = 'compute')
  compute = Module("compute/0", run = computerun)
  compute.addPort("primesOut", direction = "out")
  visu = Module("visu", cmdline = "bin/visu")
  visu.addPort("primesIn", direction = "in")
  visu.addPort("keysIn", direction = "in")
  compute.getPort("primesOut").link(visu.getPort("primesIn"))
  
  # in order to limit the throughput of compute, visu's endIt can be
  # linked to compute's beginIt. This way, compute will not start an
  # iteration before visu has ended processing the previous:
  visu.getPort("endIt").link(compute.getPort("beginIt"))
  
  
if example == 2:
  # plain examples without composites 
  # A FlowvrRunSSHMultiple object is required for the compute module
  # because it is designed to be run in several instances
  computerun = FlowvrRunSSHMultiple('bin/compute', hosts = 'localhost', prefix = 'compute')
  compute = Module("compute/0", run = computerun)
  compute.addPort("primesOut", direction = "out")
  visu = Module("visu", cmdline = "bin/visu")
  visu.addPort("primesIn", direction = "in")
  visu.addPort("keysIn", direction = "in")
  compute.getPort("primesOut").link(visu.getPort("primesIn"))
  
  ### presignal that synchronizes compute with visu
  # nb is the number of messages to send before the first wait
  presignal = FilterPreSignal("presignal", nb = 1)
  visu.getPort("endIt").link(presignal.getPort('in'))
  presignal.getPort('out').link(compute.getPort("beginIt"))
  
if example == 3:
  # plain examples without composites 
  # A FlowvrRunSSHMultiple object is required for the compute module
  # because it is designed to be run in several instances
  computerun = FlowvrRunSSHMultiple('bin/compute', hosts = 'localhost', prefix = 'compute')
  compute = Module("compute/0", run = computerun)
  compute.addPort("primesOut", direction = "out")
  visu = Module("visu", cmdline = "bin/visu")
  visu.addPort("primesIn", direction = "in")
  visu.addPort("keysIn", direction = "in")
  compute.getPort("primesOut").link(visu.getPort("primesIn"))
  
  ### presignal that synchronizes compute with visu
  # nb is the number of messages to send before the first wait
  presignal = FilterPreSignal("presignal", nb = 1)
  visu.getPort("endIt").link(presignal.getPort('in'))
  presignal.getPort('out').link(compute.getPort("beginIt"))
  
  ### capture module to control the visu
  capture = Module("capture", cmdline = "bin/capture")
  capture.addPort("keysOut")
  
  # In this case, capture is producing messages faster than
  # compute. Therefore, the capture messages are queueing up in
  # flowvrd, eventually crashing it.
  capture.getPort("keysOut").link(visu.getPort("keysIn"))

if example == 4:
  # plain examples without composites 
  # A FlowvrRunSSHMultiple object is required for the compute module
  # because it is designed to be run in several instances
  computerun = FlowvrRunSSHMultiple('bin/compute', hosts = 'localhost', prefix = 'compute')
  compute = Module("compute/0", run = computerun)
  compute.addPort("primesOut", direction = "out")
  visu = Module("visu", cmdline = "bin/visu")
  visu.addPort("primesIn", direction = "in")
  visu.addPort("keysIn", direction = "in")
  compute.getPort("primesOut").link(visu.getPort("primesIn"))
  
  ### presignal that synchronizes compute with visu
  # nb is the number of messages to send before the first wait
  presignal = FilterPreSignal("presignal", nb = 1)
  visu.getPort("endIt").link(presignal.getPort('in'))
  presignal.getPort('out').link(compute.getPort("beginIt"))
  
  ### capture module to control the visu
  capture = Module("capture", cmdline = "bin/capture")
  capture.addPort("keysOut")
  
  # Synchronize capture messages
  sync = GreedySynchronizor("sync")
  presignal.getPort("out").link(sync.getPort("endIt"))
  capture.getPort("keysOut").link(sync.getPort("stamps"))
  filterit = FilterIt("filterit")
  capture.getPort("keysOut").link(filterit.getPort("in"))
  sync.getPort("order").link(filterit.getPort("order"))
  filterit.getPort("out").link(visu.getPort('keysIn'))

    
#####################################################################
# With composites, to improve readablility

class Display(Composite):  
  """ Displays primes in a window. The user can control the view with
  another window."""

  def __init__(self, prefix):
    Component.__init__(self)
    
    visu = Module(prefix + "/visu", cmdline = "bin/visu")
    visu.addPort("primesIn", direction = "in")
    visu.addPort("keysIn", direction = "in")

    ### capture module to control the visu

    capture = Module(prefix + "/capture", cmdline = "bin/capture")
    capture.addPort("keysOut")

    ### make sure the DISPLAY environment variable is available 
    
    visu.run.options += "-x DISPLAY "
    capture.run.options += "-x DISPLAY "        

    ### the presignal has to be duplicated 

    presignal = FilterPreSignal(prefix + "/presignal", nb = 1)
    visu.getPort("endIt").link(presignal.getPort('in'))

    # greedy that samples captures's keysOut at the speed of visu's endIt
    greedy = Greedy(prefix + "/greedy")

    visu.getPort("endIt").link(greedy.getPort("sync"))
    capture.getPort("keysOut").link(greedy.getPort("in"))
    greedy.getPort("out").link(visu.getPort("keysIn"))

    ### expose input and output ports

    self.ports["endIt"] = visu.getPort("endIt")
    self.ports["primesIn"] = visu.getPort("primesIn")


if example == 6:
  computerun = FlowvrRunSSHMultiple('bin/compute', hosts = 'localhost', prefix = 'compute')
  compute = Module("compute/0", run = computerun)
  compute.addPort("primesOut", direction = "out")

  display = Display("display")
  compute.getPort("primesOut").link(display.getPort("primesIn"))

  ### presignal that synchronizes compute with visu

  # nb is the number of messages to send before the first wait
  presignal = FilterPreSignal("presignal", nb = 1)

  display.getPort("endIt").link(presignal.getPort('in'))
  presignal.getPort('out').link(compute.getPort("beginIt"))



#####################################################################
# Several instances of compute

class Compute(Composite):
  """ several instances of a compute module, that compute primes
  together, and output them to the primesOut port""" 
  
  def __init__(self, prefix, hosts):
    Composite.__init__(self)
    
    # hosts: string with host names, separated by spaces
    computerun = FlowvrRunSSHMultiple('bin/compute', hosts = hosts, prefix = prefix)

    # hosts_list: convert hosts to a list
    hosts_list = hosts.split()
    
    # nb of instances
    ninstance = len(hosts_list) 

    merge = FilterMerge(prefix + '/merge')

    # collect beginIt's 
    all_beginIts = []

    for i in range(ninstance): 
      compute = Module(prefix + "/" + str(i), run = computerun, host = hosts_list[i])
      compute.addPort("primesOut", direction = "out")

      compute.getPort("primesOut").link(merge.newInputPort())
      all_beginIts.append(compute.getPort("beginIt"))
      
    self.ports["primesOut"] = merge.getPort("out")
    self.ports["beginIt"] = list(all_beginIts)


if example == 7:
  compute = Compute("compute", hosts = "localhost " * 4)
  display = Display("display")
  compute.getPort("primesOut").link(display.getPort("primesIn"))

  # presignal that synchronizes compute with visu

  presignal = FilterPreSignal("presignal", nb = 1)
  
  display.getPort("endIt").link(presignal.getPort('in'))
  presignal.getPort('out').link(compute.getPort("beginIt"))

class ComputeWithTree(Composite):
  """ several instances of a compute module, that compute primes
  together, and output them to the primesOut port. This version
  merges the computer's results with a tree. """ 
  
  def __init__(self, prefix, hosts, out_port):
    Composite.__init__(self)
    
    # hosts: string with host names, separated by spaces
    computerun = FlowvrRunSSHMultiple('bin/compute', hosts = hosts, prefix = prefix)

    # hosts_list: convert hosts to a list
    hosts_list = hosts.split()
    
    # nb of instances
    ninstance = len(hosts_list) 

    all_beginIts = []
    all_primesOut = []

    for i in range(ninstance): 
      compute = Module(prefix + "/" + str(i), run = computerun, host = hosts_list[i])
      compute.addPort("primesOut", direction = "out")

      all_beginIts.append(compute.getPort("beginIt"))
      all_primesOut.append(compute.getPort("primesOut"))
      
    treeOut = generateNto1(prefix + '/tree', all_primesOut)
    treeOut.link(out_port)

    self.ports["beginIt"] = list(all_beginIts)


if example == 8:
  # take first host as the one on which we display...
  app.default_host = compute_hosts.split()[0]

  display = Display("display")

  compute = ComputeWithTree("compute", hosts = compute_hosts, out_port = display.getPort("primesIn"))

  ### presignal that synchronizes compute with visu

  presignal = FilterPreSignal("presignal", nb = 1)

  display.getPort("endIt").link(presignal.getPort('in'))
  presignal.getPort('out').link(compute.getPort("beginIt"))


app.generate_xml('primes')
