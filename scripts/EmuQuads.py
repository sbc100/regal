#!/usr/bin/python -B

quadsFormulae = {
  'PolygonMode' : {
    'entries'    : [ 'glPolygonMode' ],
    'prefix'     : [ '_context->quads->glPolygonMode( ${arg0plus} );', ],
  },
  'ShadeModel' : {
    'entries'    : [ 'glShadeModel' ],
    'prefix'     : [ '_context->quads->glShadeModel( ${arg0plus} );', ],
  },
  'ProvokingVertex' : {
    'entries'    : [ 'glProvokingVertex(EXT|)' ],
    'prefix'     : [ '_context->quads->glProvokingVertex( ${arg0plus} );', ],
  },
  'quads'       : {
    'entries' : [ 'glDrawArrays(EXT|)' ],
    'impl'    : [
'''
if ( ! _context->quads->glDrawArrays( _context, ${arg0plus} ) ) {
  DispatchTableGL *_next = _context->dispatcher.emulation.next();
  RegalAssert(_next);
  return _next->call(&_next->glDrawArrays)( ${arg0plus} );
}
'''
    ],
  },
}
