#!/usr/bin/python -B

# Regal.cpp implementation of GL_REGAL_proc_address extension

formulae = {
    'GetProcAddress' : {
        'entries' : [ 'glGetProcAddressREGAL' ],
        'impl' : '''
void *ret;

ret = Lookup::gl_Lookup<void *>(${arg0});
if (ret)
  return ret;

#if REGAL_SYS_WGL
ret = Lookup::wgl_Lookup<void *>(${arg0});
if (ret)
  return ret;
#endif

#if REGAL_SYS_GLX
ret = Lookup::glx_Lookup<void *>(${arg0});
if (ret)
  return ret;
#endif

#if REGAL_SYS_EGL
ret = Lookup::egl_Lookup<void *>(${arg0});
if (ret)
  return ret;
#endif

#if REGAL_SYS_OSX
ret = Lookup::cgl_Lookup<void *>(${arg0});
if (ret)
  return ret;
#endif

return NULL;
'''
    }
}
