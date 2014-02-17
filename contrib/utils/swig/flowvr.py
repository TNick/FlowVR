"""
Adds a few methods to existing classes. Hopefully simplifies interface. 
"""
import array
import cflowvr

def pytype_to_flowvr_type(ty):
  if ty == type(1): 
    return cflowvr.TypeInt.create()
  elif ty == type(''): 
    return cflowvr.TypeString.create()
  elif ty == type(1.0):
    return cflowvr.TypeFloat.create()
  assert False, "unhandled type %s" % ty
  
  

def StampList_add(self, name, ty, nb = -1):
  if nb == -1:
    si = cflowvr.StampInfo(name, pytype_to_flowvr_type(ty))  
  else:
    si = cflowvr.StampInfo(name, cflowvr.TypeArray.create(nb, pytype_to_flowvr_type(ty)))
  si.this.own(False) # ownership transferred to stamplist
  self.add(si)
 
cflowvr.StampList.addStamp = StampList_add

class OutputPort(cflowvr.OutputPort):

  def addStamp(self, name, ty, nb = -1):
    
    if nb == -1:
      si = cflowvr.StampInfo(name, pytype_to_flowvr_type(ty))  
    else:
      si = cflowvr.StampInfo(name, cflowvr.TypeArray.create(nb, pytype_to_flowvr_type(ty)))
    si.this.own(False) # ownership transferred to stamplist
    self.stamps.add(si)

  def put(self, message):
    self.getModule().put(self, message)

StampList = cflowvr.StampList
vectorPort = cflowvr.vectorPort
initModule = cflowvr.initModule


def stamp_getStamp(self, name, index = -1):
  si = self.stampList[name]
  assert si, "could not find stamp named " + name
  s = self.stamps.asString()
  ty = si.getType()
  typename = ty.typeName()
  ss = s[si.getOffset() :  si.getOffset() + si.getSize()]
  if typename in ("int", "float"):
    a = array.array(typename[0])
    a.fromstring(ss)
    return a[0]
  elif typename == "string":
    a = array.array('i')
    a.fromstring(ss)
    return s[a[0]:a[0]+a[1]]
  elif typename == "binary":
    return ss
  elif typename == "array":
    ta = ty.toTypeArray()
    subt = ta.t.typeName()
    # works only for int and float?    
    if subt not in ("int", "float"): return None
    a = array.array(subt[0])
    a.fromstring(ss)
    if index == -1: 
      return list(a)
    else:
      return a[index]
  else: assert False
    
def stamp_getStamps(self):
  ret = {}
  for i in range(self.stampList.nbStamp()):
    name = self.stampList[i].getName()
    ret[name] = self.getStamp(name)
  return ret

class MessageWrite(cflowvr.MessageWrite):
  " inherit to guarantee that messages are typed "

  def __init__(self, stampList = None):
    cflowvr.MessageWrite.__init__(self)
    if stampList == None:
      stampList = cflowvr.StampList()
    self.stampList = stampList

  def getStamp(*args):
    return the_getStamp(*args)

  def setStamp(self, name, value):
    " sets stamp value "
    if type(name) == type(()):
      name, index = name
      si = self.stampList[name]
      self.stamps.writeArray(si, index, value)
    else:
      si = self.stampList[name]      
      self.stamps.write1(si, value)
    

cflowvr.Message.getStamp = stamp_getStamp
cflowvr.Message.getStamps = stamp_getStamps

class InputPort(cflowvr.InputPort):

  def getMessage(self):
    message = cflowvr.Message()
    self.getModule().get(self, message)    
    message.stampList = self.stamps
    return message
    
