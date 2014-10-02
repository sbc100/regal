#!/usr/bin/python -B

#
# Utility script for converting XML extension
# specifications into .py Python-format API
# database.
#
# See also:
#   https://bitbucket.org/alfonse/gl-xml-specs
#   https://github.com/nigels-com/glfixes
#
# For help:
#   scripts/xml2py.py -help
#
# Example usage:
#   scripts/xml/khronosxml2py.py -a gl -o gl.py -i scripts/xml/khronos/gl.xml

import sys
sys.path.insert(0,'scripts')
sys.path.insert(0,'scripts/api')
sys.path.insert(0,'scripts/xml/regal')

from types import *

from Api import *
from ApiUtil import importAttr
import ApiWrite

from optparse import OptionParser
from xml.dom import minidom
import re

options = None

def getTextList(nodelist):
  rc = []
  for node in nodelist:
    if node.nodeType == node.TEXT_NODE:
      rc.append(node.data)
  return rc

def getText(nodelist):
  return ''.join(getTextList(nodelist))

def checkReturnType(t):
  if t in [ 'GLbitfield', 'GLboolean', 'GLenum', 'GLfloat', 'GLhandleARB', 'GLint', 'GLintptr', 'GLsync', 'GLubyte', 'GLuint', 'GLuint64', 'void', 'void *']:
    return t
  print 'checkReturnType(): unknown return type %s. using void instead'%t
  return 'void'

def getReturnType(proto):
  ptype = proto.getElementsByTagName('ptype')
  if len(ptype):
    rtype = ptype[0].firstChild.nodeValue
  else:
    rtype = getTextList(proto.childNodes)[0]
  return checkReturnType(rtype.strip())

def getType(t):
  # if t == 'GLclampf':
    # return 'GLfloat'
  # if t == 'GLclampd':
    # return 'GLdouble'
  # if t == 'GLclampx':
    # return 'GLfixed'
  return t

def getGLVersion(v):
  if v.startswith('GL_VERSION_') and v[11:12].isdigit():
    s = v[-3:]
    s = s.replace('_','.')
    return s
  return ''

def needToReplaceAttr(thisAttr):
  if isinstance(thisAttr, NoneType):
    return False
  if isinstance(thisAttr, StringType) and thisAttr == '':
    return False
  if isinstance(thisAttr, UnicodeType) and thisAttr.encode('ascii', 'ignore') == '':
    return False
  if isinstance(thisAttr, ListType) and len(thisAttr) < 1:
    return False
  return True

def replaceParameters(original,new):
  if len(original.parameters) == 1 and len(new.parameters) == 1:
    original.parameters[0] = new.parameters[0]
    return
  for p in new.parameters:
    ii = 0
    foundVoid = -1
    while ii < len(original.parameters):
      if original.parameters[ii].name == 'void':
        if foundVoid < 0:
          foundVoid = ii
        ii += 1
        continue
      if original.parameters[ii].name == p.name:
        original.parameters[ii] = p
        foundVoid = -1
        break
      ii += 1
    if foundVoid >= 0:
      original.parameters[foundVoid] = p
  return

def updateApi(originalApi, apiUpdates):

  # update enumerants

  if len(originalApi.enums) > 1:
    print "Uh-oh.  updateApi() doesn't handle multiple api.enums correctly"

  enumDict = {}
  for e in originalApi.enums:
    for enum in e.enumerants:
      enumDict[enum.name] = enum

  for e in apiUpdates.enums:
    for enum in e.enumerants:
      oEnum = enumDict.get(enum.name)
      if oEnum:
        for attr in vars(enum):
          setattr(oEnum,attr,getattr(enum,attr))
      else:
        originalApi.enums[0].add(enum)

  # update functions

  funcDict = {}
  for func in originalApi.functions:
    funcDict[func.name] = func

  for f in apiUpdates.functions:
    oFunc = funcDict.get(f.name)
    if oFunc:
      for a in vars(f):
        if a == 'parameters':
          replaceParameters(oFunc,f)
        else:
          thisAttr = getattr(f,a)
          if (a == 'version' and f.version != '') or needToReplaceAttr(thisAttr):
            setattr(oFunc,a,getattr(f,a))
    else:
      if f.ret == None:
        f.ret = Return()
      originalApi.add(f)

  # update extensions

  extsnDict = {}
  for extsn in originalApi.extensions:
    extsnDict[extsn.name] = extsn

  for extsn in apiUpdates.extensions:
    oExtsn = extsnDict.get(extsn.name)
    if oExtsn:
      for a in vars(extsn):
        thisAttr = getattr(extsn,a)
        if needToReplaceAttr(thisAttr):
          setattr(oExtsn,a,getattr(extsn,a))
    else:
      originalApi.add(extsn)

#
# main
#

if __name__ == "__main__":

  parser = OptionParser('usage: %prog [options] [SOURCES...]')
  parser.add_option('-a', '--api',    dest = 'api',                      help = 'API name',                    default = 'gl')
  parser.add_option('-i', '--input',  dest = 'input',  metavar = 'FILE', help = 'input file in Khronos XML format', default = '-' )
  parser.add_option('-o', '--output', dest = 'output', metavar = 'FILE', help = 'Python output file',          default = [])
  (options, args) = parser.parse_args()

  # Read input XML
  
  if options.input!='-':
    xmldoc = minidom.parse(options.input)
  else:
    xmldoc = minidom.parse(sys.stdout)

  api = Api()

  # typedefs

  for type in xmldoc.getElementsByTagName('type'):
    n = type.getElementsByTagName('name')
    if len(n):
      name = n[0].firstChild.nodeValue
      str = getText(type.childNodes).strip()
      if str.startswith('typedef '):
        str = str[8:]
      if str.endswith(';'):
        str = str[:-1]
      ee = Typedef(name, str.strip())
      if name == 'GLboolean':
        ee.default = 'GL_FALSE'
      else:
        ee.default = 0
      api.add(ee)

  # enums

  defines = Enum('defines')
  api.add(defines)

  for e in xmldoc.getElementsByTagName('enum'):
    name = '%s'%(e.getAttribute('name'))
    # ugh... don't do this search...
    l = [eee for eee in defines.enumerants if eee.name == name]
    if len(l):
      ee = l[0]
    else:
      ee = Enumerant()
      defines.add(ee)
      ee.name = name

    if e.getAttribute('value'):
      ee.value = e.getAttribute('value')
    else:
      if e.parentNode.localName == 'group':
        ee.group = e.parentNode.getAttribute('group')
      else:
        if e.parentNode.localName == 'require':
          if e.parentNode.parentNode.localName == 'feature' or e.parentNode.parentNode.localName == 'extension':
            if ee.category == '':
              ee.category = e.parentNode.parentNode.getAttribute('name')

  # functions

  for c in xmldoc.getElementsByTagName('command'):

    pp = c.getElementsByTagName('proto')

    if len(pp):

      ff = Function()
      api.add(ff)

      for p in pp:
        n = p.getElementsByTagName('name')
        ff.name = n[0].firstChild.nodeValue

        rtype = getReturnType(p)
        ff.ret = Return(rtype)

        for param in c.getElementsByTagName('param'):
          t = param.getElementsByTagName('ptype')
          if len(t):
            n = param.getElementsByTagName('name')[0].firstChild.nodeValue
            nodeStrings = getTextList(param.childNodes)
            str = ''
            if len(nodeStrings):
              str += nodeStrings[0]
            str += getType(t[0].firstChild.nodeValue)
            if len(nodeStrings) > 1:
              str += ''.join(nodeStrings[1:])
            t = str.strip()
          else:
            n = 'void'
            t = ''
          ff.parameters.append( Input(n,t))

    else:

      if c.parentNode.localName == 'require':
        if c.parentNode.parentNode.localName == 'feature' or c.parentNode.parentNode.localName == 'extension':
          name = '%s'%(c.getAttribute('name'))
          l = [fff for fff in api.functions if fff.name == name]
          if len(l):
            ff = l[0]
            if ff.category == '':
              ff.category = c.parentNode.parentNode.getAttribute('name')
            if ff.version == '' or ff.category.startswith('GL_VERSION_') and ff.category[11:11].isdigit():
              ff.version = getGLVersion(ff.category)

  # extensions

  for e in xmldoc.getElementsByTagName('extension'):

    name = '%s'%(e.getAttribute('name'))
    ee = Extension(name)
    api.add(ee)

    u = name[3:]
    u = u.replace('_','/',1)
    ee.url = 'http://www.opengl.org/registry/specs/' + u + '.txt'

    for r in e.getElementsByTagName('require'):
      for command in r.getElementsByTagName('command'):
        ee.functions.append('%s'%(command.getAttribute('name')))
      for enum in r.getElementsByTagName('enum'):
        ee.enumerants.append('%s'%(enum.getAttribute('name')))

    ee.functions  = sorted(ee.functions)
    ee.enumerants = sorted(ee.enumerants)
    
  # process extra-regal-info file

  regal_api = 'regal_' + options.api
  api_tweaks = getattr(__import__(regal_api), regal_api)
  if api_tweaks:
    updateApi(api, api_tweaks)

  # output file

  if options.output:
    file = open(options.output, 'w')
  else:
    file = sys.stdout

  ApiWrite.writeHeader(file,options.api)
  ApiWrite.writeTypedefs(file,options.api,api.typedefs)
  ApiWrite.writeEnums(file,options.api,api.enums)
  ApiWrite.writeFunctions(file,options.api,api.functions)
  ApiWrite.writeExtensions(file,options.api,api.extensions)

