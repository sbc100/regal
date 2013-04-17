#!/usr/bin/python -B

ppaFormulae = {
  'PushAtrrib' : {
    'entries'  : [ 'glPushAttrib' ],
    'impl'     : [ '_context->ppa->PushAttrib( _context, ${arg0} );', ],
  },
  'PopAttrib' : {
    'entries' : [ 'glPopAttrib' ],
    'impl'    : [ '_context->ppa->PopAttrib( _context );', ],
  },
  'Get'       : {
    'entries' : [ 'glGet(Integer|Float|Boolean)v' ],
    'impl'    : [
      'if( ! _context->ppa->Get( _context, ${arg0plus} ) ) {',
      '  _context->dispatcher.emulation.glGet${m1}v( ${arg0plus} );',
      '}',
    ],
  },
  'Enable'    : {
    'entries' : [ 'gl(Enable|Disable)(i|)' ],
    'impl'    : [
      'if( ! _context->ppa->${m1}${m2}( _context, ${arg0plus} ) ) {',
      '  _context->dispatcher.emulation.gl${m1}${m2}( ${arg0plus} );',
      '}',
    ],
  },
  'TrackDepth' : {
    'entries'    : [ '(glClearDepth|glDepthFunc|glDepthMask)(f|)' ],
    'prefix'     : [ '_context->ppa->${m1}${m2}( ${arg0plus} );', ],
  },
  'TrackStencil' : {
    'entries'    : [ '(glClearStencil|glStencilFunc|glStencilFuncSeparate|glStencilMask|glStencilMaskSeparate|glStencilOp|glStencilOpSeparate)' ],
    'prefix'     : [ '_context->ppa->${m1}( ${arg0plus} );', ],
  },
  'TrackPolygon' : {
    'entries'    : [ '(glCullFace|glFrontFace|glPolygonMode|glPolygonOffset)' ],
    'prefix'     : [ '_context->ppa->${m1}( ${arg0plus} );', ],
  },
  'TrackDrawBuffer' : {
    'entries'    : [ 'glDrawBuffer(s|)(ARB|)' ],
    'impl'    : [
      'if( !_context->info->es2 ) {',
      '  _context->dispatcher.emulation.glDrawBuffer${m1}( ${arg0plus} );',
      '}',
    ],
  },
  'TrackMatrixMode' : {
    'entries'    : [ 'glMatrixMode' ],
    'prefix'     : [ '_context->ppa->glMatrixMode( ${arg0plus} );', ],
  },
  'TrackClipPlane' : {
    'entries'    : [ 'glClipPlane' ],
    'prefix'     : [ '_context->ppa->glClipPlane( ${arg0plus} );', ],
  },
  'TrackHint' : {
    'entries'    : [ 'glHint' ],
    'prefix'     : [ '_context->ppa->glHint( ${arg0plus} );', ],
  },
  'TrackClampColor': {
    'entries'    : [ 'glClampColor' ],
    'prefix'     : [ '_context->ppa->glClampColor( ${arg0plus} );',
    ],
  },
  'TrackActiveTexture' : {
    'entries'    : [ 'glActiveTexture(ARB|)' ],
    'prefix'     : [ '_context->ppa->glActiveTexture( ${arg0plus} );', ],
  },
  'TrackListBase' : {
    'entries'    : [ 'glListBase' ],
    'prefix'     : [ '_context->ppa->glListBase( ${arg0plus} );', ],
  },
  'TrackClearAccum' : {
    'entries'    : [ 'glClearAccum' ],
    'prefix'     : [ '_context->ppa->glClearAccum( ${arg0plus} );', ],
  },
  'TrackScissor' : {
    'entries'    : [ 'glScissor(Array|Indexed|)(v|)' ],
    'prefix'     : [ '_context->ppa->glScissor${m1}${m2}( ${arg0plus} );', ],
  },
  'TrackViewport' : {
    'entries'    : [ 'glViewport(Array|Indexedf|)(v|)' ],
    'prefix'     : [ '_context->ppa->glViewport${m1}${m2}( ${arg0plus} );', ],
  },
  'TrackDepthRange' : {
    'entries'    : [ 'glDepthRange(Array|Indexed|)(f|v|)' ],
    'prefix'     : [ '_context->ppa->glDepthRange${m1}${m2}( ${arg0plus} );', ],
  },
  'TrackLine' : {
    'entries'    : [ 'glLine(Width|Stipple|)' ],
    'prefix'     : [ '_context->ppa->glLine${m1}( ${arg0plus} );', ],
  },
  'TrackSampleCoverage' : {
    'entries'    : [ 'glSampleCoverage' ],
    'prefix'     : [ '_context->ppa->glSampleCoverage( ${arg0plus} );', ],
  },
  'TrackMinSampleShading' : {
    'entries'    : [ 'glMinSampleShading' ],
    'prefix'     : [ '_context->ppa->glMinSampleShading( ${arg0plus} );', ],
  },
  'TrackMapGrid' : {
    'entries'    : [ 'glMapGrid(1|2)(f|d)' ],
    'prefix'     : [ '_context->ppa->glMapGrid${m1}( ${arg0plus} );', ],
  },
  'TrackPointSize' : {
    'entries'    : [ 'glPointSize' ],
    'prefix'     : [ '_context->ppa->glPointSize( ${arg0plus} );', ],
  },
  'TrackPointParameter' : {
    'entries'    : [ 'glPointParameter(i|f)(v|)' ],
    'prefix'     : [ '_context->ppa->glPointParameter${m2}( ${arg0plus} );', ],
  },
  'TrackTexEnv' : {
    'entries'    : [ 'gl(Multi|)TexEnv(i|f)(v|)(EXT|)' ],
    'prefix'     : [ '_context->ppa->gl${m1}TexEnv${m3}( ${arg0plus} );', ],
  },
  'TrackPolygonStipple' : {
    'entries'    : [ 'glPolygonStipple' ],
    'prefix'     : [ '_context->ppa->glPolygonStipple( ${arg0plus} );', ],
  },

}
