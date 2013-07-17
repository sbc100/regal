#!/usr/bin/python -B

#
# Selectively initialize Regal for known initialization calls
# in addition to Regal API entry points.
#
# CGLGetCurrentContext is needed for Mac OS X/GLUT
# eglGetDisplay is needed for apitrace eglretrace tool.
# glXGetProcAddress is needed for Linux chromium
# glXQueryExtension is needed for freeglut X11
# glXGetProcAddressARB is needed for Linux Minecraft 1.6.1

formulae = {
  'EmuInit' : {
    'entries' : [ 
      'CGLChoosePixelFormat', 'CGLGetCurrentContext', 
      'eglGetDisplay', 
      'glXGetProcAddress', 'glXQueryExtension', 'glXGetProcAddressARB'
 #     'glX.*' 
    ],
    'prefix'  : [ 'Init::init();' ]
  }
}

#
# Hook into the MakeCurrent and DestroyContext functions in
# order to manage Regal contexts.
#

formulaeGlobal = {

    # WGL

    'wglMakeCurrent' : {
        'entries' : [ 'wglMakeCurrent' ],
        'init' : [ 'Init::makeCurrent(RegalSystemContext(hglrc));' ]
    },

    'wglDeleteContext' : {
        'entries' : [ 'wglDeleteContext' ],
        'init' : [ 'Init::destroyContext(RegalSystemContext(hglrc));' ]
    },

    # GLX

    'glXMakeCurrent' : {
        'entries' : [ 'glXMakeCurrent' ],
        'init' : [ 'Init::makeCurrent(RegalSystemContext(ctx));' ]
    },

    'glXMakeContextCurrent' : {
        'entries' : [ 'glXMakeContextCurrent' ],
        'init' : [ 'Init::makeCurrent(RegalSystemContext(ctx));' ]
    },

    'glXDestroyContext' : {
        'entries' : [ 'glXDestroyContext' ],
        'init' : [ 'Init::destroyContext(RegalSystemContext(ctx));' ]
    },

    # EGL

    'eglMakeCurrent' : {
        'entries' : [ 'eglMakeCurrent' ],
        'init' : [ 'Init::makeCurrent(ctx);' ]
    },

    'eglDestroyContext' : {
        'entries' : [ 'eglDestroyContext' ],
        'init' : [ 'Init::destroyContext(RegalSystemContext(ctx));' ]
    },

    # CGL

    'CGLSetCurrentContext' : {
        'entries' : [ 'CGLSetCurrentContext' ],
        'init' : [ 'Init::makeCurrent(ctx);' ]
    },

    'CGLDestroyContext' : {
        'entries' : [ 'CGLDestroyContext' ],
        'init' : [ 'Init::destroyContext(RegalSystemContext(ctx));' ]
    },
}
