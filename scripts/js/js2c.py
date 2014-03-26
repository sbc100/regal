#!/usr/bin/python -B

import re

def main():
  files = [ 'regalweb.js', 'jquery.min.js', 'jquery-ui.min.js', 'jquery-ui.min.css' ]
  outfile = open( '../../src/regal/RegalWebJs.h', 'w' )
  outfile.write( "/* do not edit - generated via js2c.py from regalweb.js */\n" )
  outfile.write( "namespace {\n\n" )
  esc = re.compile(r'(\\|\")', re.VERBOSE)
  rm = re.compile(r'(\.|-)', re.VERBOSE)
  for f in files:
    varname = rm.sub( '', f )
    infile = open( f, 'r' )
    lines = infile.readlines()
    outfile.write( "  const char * %s = \n" % varname )
    for l in lines:
      lp = esc.sub( r'\\\1', l.rstrip('\n') )
      outfile.write( "    \"%s\\n\"\n" % lp )
    outfile.write( "  ;\n\n" )
    infile.close()
    if f == 'regalweb.js':
      outfile.write( "#if REGAL_HTTP_LOCAL_JQUERY\n" )
  outfile.write( "#endif // REGAL_LOCAL_JQUERY\n\n" )
  outfile.write( "}\n" )
  outfile.close()


if __name__ == '__main__':
  main()

