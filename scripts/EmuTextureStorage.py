#!/usr/bin/python -B

texstoFormulae = {
    'TextureStorage' : { 
        'entries' : [ 'glTextureStorage(1|2|3)D(EXT|)' ],
        'impl' : [ '_context->texsto->TextureStorage( _context, ${arg0plus} );', ],
    },
    #'TexImage' : { # disallow these if the object was specified with TextureStorage
    #    'entries' : [ 'gl(Copy|)TexImage(1|2|3)D(ARB|)' ],
    #    'impl' : [ '_context->texsto->${m1}TexImage( _context, ${arg0plus} );', ],
    #}
}
