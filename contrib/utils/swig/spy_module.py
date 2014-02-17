import sys, time

import flowvr


clear_screen = "\033[H\033[2J"


ports = flowvr.vectorPort()
port = flowvr.InputPort('in')
ports.push_back(port)
module = flowvr.initModule(ports);

t0 = time.time()
  
while module.wait():
  message = port.getMessage()   

  print clear_screen

  print "Received message of %d + %d bytes at t = %.3f s" % (
    message.data.getSize(),
    message.stamps.getSize(),
    time.time() - t0) 
  print

  print "Stamps:"
  stamps = message.getStamps()
  for name in sorted(stamps.keys()):
    print "%-30s %s" % (name, stamps[name])
    

print "SPY closing!"

module.close()
