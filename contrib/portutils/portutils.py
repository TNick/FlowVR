"""
Portutils is way of using the same code for the module and its flowvr
app side. The information for a given module is spread across two
files:

- an XML configuration file, that can be parsed in Python

- a DLL compiled from the module code. It contains a function called
"getParameters" that outputs a parameter block that can be changed by
flowvrapp. Since its strucutre is complex, we use a helper function in
portutils to convert it to a string.

In addition, the module parameters are converted to a string, which is
encoded in base64. However, the base64 encoding is not standard (at
least it is not compatible with Python's base64 module), so we also
use the portutils function for this.

"""

import ctypes, pdb, os
import xml.dom.minidom

import flowvrapp

#############################################################################
# import a few functions from DLLs
#
# the portutils dll + the .so's corresponding to the plugins should be
# in the LD_LIBRARY_PATH. We use fixed-size string buffers, assuming that
# parameter blocks will never be huge. 

def getParametersFromSo(soName, fctName, moduleName):
  dll = ctypes.cdll.LoadLibrary('libflowvr-portmodule.comp.so')

  f = dll.port_interface_get_params_from_so

  print "loading DLL", soName

  # port_interface_get_params_from_so(const char *strSoName, const char *strFctName, const char *strModuleName,
  # char *buf, int sz) {

  f.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p,
                ctypes.c_char_p, ctypes.c_int]
  f.restype = ctypes.c_int

  buf = ctypes.create_string_buffer(10000)
  sz_out = f(soName, fctName, moduleName, buf, 10000)

  # print "   -> gives", buf.raw[:sz_out]
  return buf.raw[:sz_out]

def toBase64(s):
  dll = ctypes.cdll.LoadLibrary('libflowvr-portmodule.comp.so')

  f = dll.encode_string_to_base64
   
  f.argtypes = [ctypes.c_char_p, 
                ctypes.c_char_p, ctypes.c_int]
  f.restype = ctypes.c_int

  buf = ctypes.create_string_buffer(10000)
  sz_out = f(s, buf, 10000)

  if sz_out > 9900: RuntimeError("too large string to encode")

  return buf.raw[:sz_out]

def fromBase64(s):
  dll = ctypes.cdll.LoadLibrary('libflowvr-portmodule.comp.so')

  f = dll.decode_string_from_base64
   
  f.argtypes = [ctypes.c_char_p, 
                ctypes.c_char_p, ctypes.c_int]
  f.restype = ctypes.c_int

  buf = ctypes.create_string_buffer(10000)
  sz_out = f(s, buf, 10000)

  if sz_out > 9900: RuntimeError("too large string to decode") 

  return buf.raw[:sz_out]
  


#############################################################################
# argument tree i/o

# from portparameter.h

class eType: 
  P_NONE = -1; P_STRING = 0; P_NUMBER = 1; P_BOOLEAN = 2
  P_LIST = 3; P_PARAMETERCONTAINER = 4

class eSubType:
  PS_NONE = -1; PS_STRING = 0; PS_NUMBER = 1; PS_BOOLEAN = 2;
  PS_LIST = 3;

class eSetType:
  ST_NONE = -1; ST_NOVALUE = 0; ST_DEFAULT = 1; ST_CODE = 2;
  ST_FILE = 3

class eMode:
  MD_NONE = 0; MD_OPTIONAL = 1; MD_MANDATORY = 2; MD_NONNULL = 4

def get_int(s, i):
  # print "get_int s=", s[i:i+10]
  j = s.find(' ', i)
  return int(s[i:j]), j+1

def unserialize_params_rec(s, i):
  """ s = string to unserialize, i = current read index. For now we parse
  only numerical and boolean parameters """
  if s[i] == 'p':
    ptype, i = get_int(s, i + 2)
    psubtype, i = get_int(s, i)
    pgetsettype, i = get_int(s, i)
    pgetmodetype, i = get_int(s, i)
    namesize, i = get_int(s, i)
    name = s[i:i+namesize]
    i += namesize+1
    valsize, i = get_int(s, i)
    valstr = s[i:i+valsize]
    i += valsize+1
    if (ptype, psubtype) == (eType.P_NUMBER, eSubType.PS_NONE):
      return name, (float(valstr), pgetsettype, pgetmodetype), i
    elif (ptype, psubtype) == (eType.P_BOOLEAN, eSubType.PS_NONE):
      return name, (valstr == 'true', pgetsettype, pgetmodetype), i
    elif (ptype, psubtype) == (eType.P_LIST, eSubType.PS_NUMBER):
      l = [float(x) for x in valstr.replace(',', ' ').split()]
      return name, (l, pgetsettype, pgetmodetype), i    
    elif (ptype, psubtype) == (eType.P_STRING, eSubType.PS_NONE):
      return name, (valstr, pgetsettype, pgetmodetype), i
    else: # not parsed, pass on unchanged
      return name, (ptype, psubtype, valstr, pgetsettype, pgetmodetype), i

  elif s[i] == 'c':   
    namesize, i = get_int(s, i + 2)
    name = s[i:i+namesize]
    i += namesize + 1
    nchildren, i = get_int(s, i)
    tree = {}
    for j in range(nchildren):
      name, child, i = unserialize_params_rec(s, i)
      tree[name] = child

    return name, tree, i
  else:
    assert False

def unserialize_params(s):
  name, params, i = unserialize_params_rec(s, 0)
  assert len(params) == 1 # single root node
  return params.values()[0] 
  
def serialize_params(name, params):

  if type(params) == type({}):
    # internal node
    s = 'c %d %s %d ' % (len(name), name, len(params))
    for childname in params:
      s += serialize_params(childname, params[childname])
    return s

  elif type(params) == type(()):
    # leaf
    if len(params) == 3: # parsed
      val, pgetsettype, pgetmodetype = params
      if type(val) == type(True):
        ptype, psubtype, valstr = eType.P_BOOLEAN, eSubType.PS_NONE, ('true' if val else 'false')
      elif type(val) in (type(0), type(0.0)):
        ptype, psubtype, valstr = eType.P_NUMBER, eSubType.PS_NONE, ('%g' % val)
      elif type(val) == type(''):
        ptype, psubtype, valstr = eType.P_STRING, eSubType.PS_NONE, val
        pgetsettype = eSetType.ST_DEFAULT # else ignored(?)
      elif type(val) == type([]):
        ptype, psubtype, valstr = eType.P_LIST, eSubType.PS_NUMBER, ','.join(['%g' % x for x in val])
      else: assert False
    else: # not parsed
      (ptype, psubtype, valstr, pgetsettype, pgetmodetype) = params
      
    if pgetsettype == eSetType.ST_NOVALUE: return ''
    
    return 'p %d %d %d %d %d %s %d %s ' % (
      ptype, psubtype, pgetsettype, pgetmodetype,
      len(name), name, len(valstr), valstr)
  else: assert False



class PortInterface:
  """ Reads an XML port file and:
  
  - adds the ports it defines to a primitive object

  - manipulates the arguments of the port module, and encodes them to
  a argstring to put in the .run.xml

  We parse the XML file lazily: most parameters remain in string ->
  something maps (Python dict's) unless we do something useful with them.

  """
  
  def __init__(self, portFile):    
    # maps port names -> PortDescriptor
    # aka  m_mpDescriptors
    self.ports = {}
    
    # argument tree. Each node is a dictionary. Leaves (actual
    # parameters) are as described in unserialize_params
    self.args = {}

    resolved_file_name = self.resolve_path(portFile)

    print "loading", resolved_file_name

    doc = xml.dom.minidom.parse(resolved_file_name)    

    # parse ports
    
    portsnode = doc.getElementsByTagName('ports')[0]

    for portnode in portsnode.getElementsByTagName('port'):
      port = dict(portnode.attributes.items())
      self.ports[port['name']] = port
    
    infonodes = doc.getElementsByTagName('info')
    # don't parse

    # parse args from services, plugins, module
    
    services = {}
    servicesnode = doc.getElementsByTagName('services')[0]
    for servicenode in servicesnode.getElementsByTagName('service'):
      service = dict(servicenode.attributes.items())
      if ('so' in service) and ('name' in service):
        param_str = getParametersFromSo(service['so'], "getParameters", service['name'])
        services[service['name']] = unserialize_params(param_str)

    self.args['services'] = services

    plugins = {}
    pluginsnode = doc.getElementsByTagName('plugins')[0]
    for pluginnode in pluginsnode.getElementsByTagName('plugin'):
      plugin = dict(pluginnode.attributes.items())
      if ('so' in plugin) and ('name' in plugin):
        param_str = getParametersFromSo(plugin['so'], "getParameters", plugin['name'])
        plugins[plugin['name']] = unserialize_params(param_str)

    self.args['plugins'] = plugins

    # does not seem to have children usually
    self.args['module'] = {} 
    

  def resolve_path(self, name):
    """ find the XML file using an environment variable """
    if os.access(name, os.R_OK): return name
    flowvr_portfile_path = os.getenv('FLOWVR_PORTFILE_PATH')
    if flowvr_portfile_path:
      for prefix in flowvr_portfile_path.split(':'):
        if os.access(prefix + '/' + name, os.R_OK):
          return prefix + '/' + name
    raise RuntimeError('XML file %s not found' % name)

  def ports_to_primitve(self, primitive):
    for name, pd in self.ports.items():
      # FIXME don't know how stamps ports are defined
      primitive.addPort(name, direction = pd['direction'], messagetype = 'full')


class RunWithArgtree(flowvrapp.FlowvrRunSSH):
  """ has an additional args field that is encoded to the proper
  format when the command line is written to the .run.xml """
  
  def get_cmdline(self):
    basic_cmdline = flowvrapp.FlowvrRunSSH.get_cmdline(self)
    sb64 = toBase64( serialize_params('root', self.args) )
    return  basic_cmdline + ' --argtree %d:%s' % (len(sb64), sb64)
    


class PortModule(flowvrapp.Module):

  def __init__(self, name, interfacefile,
               cmdline = 'flowvr-portbinary', host = '', 
               dbgWait = False, initWait = False):
    
    run = RunWithArgtree(cmdline + ' --portfile ' + interfacefile)

    flowvrapp.Module.__init__(self, name, run = run, host = host)    
  
    interface = PortInterface(interfacefile)
    interface.ports_to_primitve(self)

    if dbgWait: run.cmdline += " --dbgWait"
    if initWait: run.cmdline += " --initWait"

    # the args field can be changed to alter the argtree
    self.args = run.args = interface.args

    # print self.args

  def get_arg(self, name):
    """ get a value in the argtree. Name should be like "plugins-setcamparams-CAMID" """
    node = self.args
    for pathcomp in name.split('-'):
      node = node[pathcomp]
    assert len(node) == 3, "cannot handle arguments of this type"
    return node[0]    

  def set_arg(self, name, newval):
    node = self.args
    for pathcomp in name.split('-'):
      parent = node
      node = node[pathcomp]
    assert len(node) == 3, "cannot handle arguments of this type"
    # replace whole tuple
    val, pgetsettype, pgetmodetype = node
    parent[pathcomp] = newval, pgetsettype, pgetmodetype
