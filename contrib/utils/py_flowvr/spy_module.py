"""
This module can be connected on an ouput port, and reports some stats
about the messages it receives. It expects to run in an xterm window.
"""

import sys, time

import flowvr


clear_screen = "\033[H\033[2J"


ports = flowvr.vectorPort()
port = flowvr.InputPort('in')
ports.push_back(port)
module = flowvr.initModule(ports);

t0 = time.time()
n = 0
  
while module.wait():
  message = port.get()   

  print clear_screen

  print "Spy", module.getID()
  print "Received message %d of %d + %d bytes at t = %.3f s" % (
    n, 
    message.data.getSize(),
    message.stamps.getSize(),
    time.time() - t0) 
  print

  print "Stamps:"
  stamps = message.getStamps()
  for name in sorted(stamps.keys()):
    print "%-30s %s" % (name, stamps[name])
    
  n+=1
    


module.close()
