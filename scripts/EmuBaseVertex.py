#!/usr/bin/python -B

baseVertexFormulae = {
  'DrawElements'       : {
    'entries' : [ 'gl(Multi|)Draw(Range|)Elements(Instanced|)BaseVertex' ],
    'impl'    : [
      'if( ! _context->bv->gl${m1}Draw${m2}Elements${m3}BaseVertex( *_context, ${arg0plus} ) ) {',
      '  _context->dispatcher.emulation.gl${m1}Draw${m2}Elements${m3}BaseVertex( ${arg0plus} );',
      '}',
    ],
  },
}
