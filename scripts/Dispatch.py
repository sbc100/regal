#!/usr/bin/python -B

import re
from string import Template
from string import join
from copy import deepcopy

#
# Apply per-section substitutions
#
# Inputs:
#
#   code     - the substituted code snippets for each section
#   formula  - formula dictionary
#   subs     - substitutions for string.Template.substitute
#

def substitute(entry, formula, section, subs):

  if not section in formula:
    return

  # Turn a string into a list, if necessary

  tmp = formula[section]
  if isinstance(tmp,str) or isinstance(tmp,unicode):
    tmp = tmp.split('\n')

  entry[section] = [ Template(i).substitute(subs) for i in tmp ]



def dispatchGenCode(func, formulae):

  if formulae==None:
    return None

  name = func.name

  # list of function parameter names

  arglist = [ i.name.strip() for i in func.parameters ]

  # arg is a mapping from arg0 to function parameter name...

  arg = {}
  for i in range(len(arglist)):
    arg['arg%d' % i] = arglist[i]

  # ... and mappings from arg0plus to lists of function parameters

  for i in range(0,5):
    label = 'arg%dplus' % i;
    if len(arglist) > 0 :
      arg[label] = ', '.join(arglist)
      arglist.pop(0)
    else :
      arg[label] = ''

  # Iterate over the formulae
  #
  # k is the key
  # i is the formula

  for k,i in formulae.iteritems():

    # Cache the compiled regular expressions, as needed

    if 'entries_re' not in i:
      i['entries_re'] = [ re.compile( '^%s$' % j ) for j in i['entries'] ]

  # A list of matches containing (match object, formula name, formula)
  # Look for matches, ideally only one

  m = [ [j.match(name),k,i] for k,i in formulae.iteritems() for j in i['entries_re'] ]
  m = [ j for j in m if j[0] ]

  assert len(m)<=1, 'Ambiguous match (%s) for %s - giving up.'%(', '.join([j[1] for j in m]),name)

  if len(m):
    match   = m[0][0]
    formula = m[0][2]
    code = { 'name' : name }
    subs = deepcopy(arg)
    for l in range( len(match.groups()) + 1):
      subs['m%d' % l] = match.group( l )
    subs['name'] = name
    substitute( code, formula, 'pre',    subs )
    substitute( code, formula, 'post',   subs )

    return code

  return None

