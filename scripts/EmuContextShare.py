#!/usr/bin/python -B

formulae = {

   # GLX

  'glXCreateNewContext' : {
    'entries' : [ 'glXCreateNewContext' ],
    'suffix' : [
      'if (ret && share_list)',
      '  Init::shareContext(ret,share_list);'
    ]
  },

  'glXCreateContext' : {
    'entries' : [ 'glXCreateContext' ],
    'suffix' : [
      'if (ret && shareList)',
      '  Init::shareContext(ret,shareList);'
    ]
  },

  'glXCreateContextAttribsARB' : {
    'entries' : [ 'glXCreateContextAttribsARB' ],
    'suffix' : [
      'if (ret && share_context)',
      '  Init::shareContext(ret,share_context);'
    ]
  },

}
