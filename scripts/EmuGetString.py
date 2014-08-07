#!/usr/bin/python -B

formulae = {
    'GetString' : {
        'entries' : [ 'glGetString' ],
        'impl' : '''
// Regal interceptions
RegalAssert(_context->emuInfo);
switch (name)
{
  case GL_VENDOR:     return reinterpret_cast<const GLubyte *>(_context->emuInfo->vendor.c_str());
  case GL_RENDERER:   return reinterpret_cast<const GLubyte *>(_context->emuInfo->renderer.c_str());
  case GL_VERSION:    return reinterpret_cast<const GLubyte *>(_context->emuInfo->version.c_str());
  case GL_EXTENSIONS: return reinterpret_cast<const GLubyte *>(_context->extensions.c_str());
  default:
    break;
}'''
    },
    'GetStringi' : {
        'entries' : [ 'glGetStringi' ],
        'impl' : '''
// Regal interceptions
switch (name)
{
  case GL_EXTENSIONS:
  {
    std::set<std::string>::iterator it = _context->extensionsSet.begin();
    std::advance(it, index);
    return reinterpret_cast<const GLubyte *>(it->c_str());
  };
  default:
    break;
}'''
    },
    'Get' : {
        'entries' : [ 'glGet(Integer|Float|Double|Boolean|Integer64)v(EXT|)' ],
        'impl' : '''
// Regal interceptions
switch (pname)
{
  case GL_NUM_EXTENSIONS:
    _context->numExtensions( ${arg1} );
    return;

  default:
    break;
}'''
    }
}
