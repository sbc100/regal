/*
 Copyright (c) 2013 NVIDIA Corporation
 Copyright (c) 2013 Cass Everitt
 Copyright (c) 2014 Seth Williams
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Create an instance of an app shader, which still has the same uniforms as
// the original program but includes support for things like fixed-function
// alpha test.  The idea is that Regal will swap between these instances
// based on API state, and the app won't need to know or care.

#include "pch.h" /* For MS precompiled header support */

#include "RegalUtil.h"

#if REGAL_EMULATION

REGAL_GLOBAL_BEGIN

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include <string>
#include <vector>
#include <map>

using namespace std;

#include "RegalShaderInstance.h"

#include "RegalLog.h"
#include "RegalToken.h"
#include "RegalHelper.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

namespace ShaderInstance {

  struct GlslType {
    GLenum numericType;
    enum BaseType {
      Variable, Sampler, Image
    };
    BaseType baseType;

  };

  enum UniformApiType {
    UAT_Invalid, UAT_Double, UAT_Float, UAT_Int, UAT_UnsignedInt, UAT_DoubleMatrix, UAT_FloatMatrix
  };

  UniformApiType GetUniformApiType( GLenum type ) {
    switch( type ) {
      case GL_FLOAT:
      case GL_FLOAT_VEC2:
      case GL_FLOAT_VEC3:
      case GL_FLOAT_VEC4:
        return UAT_Float;
      case GL_DOUBLE:
      case GL_DOUBLE_VEC2:
      case GL_DOUBLE_VEC3:
      case GL_DOUBLE_VEC4:
        return UAT_Double;
      case GL_INT:
      case GL_INT_VEC2:
      case GL_INT_VEC3:
      case GL_INT_VEC4:
        return UAT_Int;
      case GL_UNSIGNED_INT:
      case GL_UNSIGNED_INT_VEC2:
      case GL_UNSIGNED_INT_VEC3:
      case GL_UNSIGNED_INT_VEC4:
        return UAT_UnsignedInt;
      case GL_BOOL:
      case GL_BOOL_VEC2:
      case GL_BOOL_VEC3:
      case GL_BOOL_VEC4:
        return UAT_Int;
      case GL_FLOAT_MAT2:
      case GL_FLOAT_MAT3:
      case GL_FLOAT_MAT4:
      case GL_FLOAT_MAT2x3:
      case GL_FLOAT_MAT2x4:
      case GL_FLOAT_MAT3x2:
      case GL_FLOAT_MAT3x4:
      case GL_FLOAT_MAT4x2:
      case GL_FLOAT_MAT4x3:
        return UAT_FloatMatrix;
      case GL_DOUBLE_MAT2:
      case GL_DOUBLE_MAT3:
      case GL_DOUBLE_MAT4:
      case GL_DOUBLE_MAT2x3:
      case GL_DOUBLE_MAT2x4:
      case GL_DOUBLE_MAT3x2:
      case GL_DOUBLE_MAT3x4:
      case GL_DOUBLE_MAT4x2:
      case GL_DOUBLE_MAT4x3:
        return UAT_DoubleMatrix;
      case GL_SAMPLER_1D:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_3D:
      case GL_SAMPLER_CUBE:
      case GL_SAMPLER_1D_SHADOW:
      case GL_SAMPLER_2D_SHADOW:
      case GL_SAMPLER_1D_ARRAY:
      case GL_SAMPLER_2D_ARRAY:
      case GL_SAMPLER_1D_ARRAY_SHADOW:
      case GL_SAMPLER_2D_ARRAY_SHADOW:
      case GL_SAMPLER_2D_MULTISAMPLE:
      case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
      case GL_SAMPLER_CUBE_SHADOW:
      case GL_SAMPLER_BUFFER:
      case GL_SAMPLER_2D_RECT:
      case GL_SAMPLER_2D_RECT_SHADOW:
      case GL_INT_SAMPLER_1D:
      case GL_INT_SAMPLER_2D:
      case GL_INT_SAMPLER_3D:
      case GL_INT_SAMPLER_CUBE:
      case GL_INT_SAMPLER_1D_ARRAY:
      case GL_INT_SAMPLER_2D_ARRAY:
      case GL_INT_SAMPLER_2D_MULTISAMPLE:
      case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
      case GL_INT_SAMPLER_BUFFER:
      case GL_INT_SAMPLER_2D_RECT:
      case GL_UNSIGNED_INT_SAMPLER_1D:
      case GL_UNSIGNED_INT_SAMPLER_2D:
      case GL_UNSIGNED_INT_SAMPLER_3D:
      case GL_UNSIGNED_INT_SAMPLER_CUBE:
      case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
      case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
      case GL_UNSIGNED_INT_SAMPLER_BUFFER:
      case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
      case GL_IMAGE_1D:
      case GL_IMAGE_2D:
      case GL_IMAGE_3D:
      case GL_IMAGE_2D_RECT:
      case GL_IMAGE_CUBE:
      case GL_IMAGE_BUFFER:
      case GL_IMAGE_1D_ARRAY:
      case GL_IMAGE_2D_ARRAY:
      case GL_IMAGE_2D_MULTISAMPLE:
      case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
      case GL_INT_IMAGE_1D:
      case GL_INT_IMAGE_2D:
      case GL_INT_IMAGE_3D:
      case GL_INT_IMAGE_2D_RECT:
      case GL_INT_IMAGE_CUBE:
      case GL_INT_IMAGE_BUFFER:
      case GL_INT_IMAGE_1D_ARRAY:
      case GL_INT_IMAGE_2D_ARRAY:
      case GL_INT_IMAGE_2D_MULTISAMPLE:
      case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
      case GL_UNSIGNED_INT_IMAGE_1D:
      case GL_UNSIGNED_INT_IMAGE_2D:
      case GL_UNSIGNED_INT_IMAGE_3D:
      case GL_UNSIGNED_INT_IMAGE_2D_RECT:
      case GL_UNSIGNED_INT_IMAGE_CUBE:
      case GL_UNSIGNED_INT_IMAGE_BUFFER:
      case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
      case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
      case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
      case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
      case GL_UNSIGNED_INT_ATOMIC_COUNTER:
        return UAT_Int;

      default: break;
    }
    return UAT_Invalid;
  }

  struct MatrixDims {
    MatrixDims( int r, int c ) : rows( r ), cols( c ) {}
    MatrixDims( const MatrixDims & rhs ) : rows( rhs.rows ), cols( rhs.cols ) {}
    int rows;
    int cols;
  };

  MatrixDims GetMatrixDims( GLenum type ) {
    switch( type ) {
      case GL_DOUBLE_MAT2: case GL_FLOAT_MAT2: return MatrixDims( 2, 2 );
      case GL_DOUBLE_MAT3: case GL_FLOAT_MAT3: return MatrixDims( 3, 3 );
      case GL_DOUBLE_MAT4: case GL_FLOAT_MAT4: return MatrixDims( 4, 4 );
      case GL_DOUBLE_MAT2x3: case GL_FLOAT_MAT2x3: return MatrixDims( 2, 3 );
      case GL_DOUBLE_MAT2x4: case GL_FLOAT_MAT2x4: return MatrixDims( 2, 4 );
      case GL_DOUBLE_MAT3x2: case GL_FLOAT_MAT3x2: return MatrixDims( 3, 2 );
      case GL_DOUBLE_MAT3x4: case GL_FLOAT_MAT3x4: return MatrixDims( 3, 4 );
      case GL_DOUBLE_MAT4x2: case GL_FLOAT_MAT4x2: return MatrixDims( 4, 2 );
      case GL_DOUBLE_MAT4x3: case GL_FLOAT_MAT4x3: return MatrixDims( 4, 3 );
      default: break;
    }
    return MatrixDims( 0, 0 );
  }

  int GetMatrixElementSize( GLenum type ) {
    switch( type ) {
      case GL_FLOAT_MAT2: case GL_FLOAT_MAT3: case GL_FLOAT_MAT4:
      case GL_FLOAT_MAT2x3: case GL_FLOAT_MAT2x4: case GL_FLOAT_MAT3x2:
      case GL_FLOAT_MAT3x4:  case GL_FLOAT_MAT4x2: case GL_FLOAT_MAT4x3:
        return sizeof( GLfloat );
      case GL_DOUBLE_MAT2: case GL_DOUBLE_MAT3: case GL_DOUBLE_MAT4:
      case GL_DOUBLE_MAT2x3: case GL_DOUBLE_MAT2x4: case GL_DOUBLE_MAT3x2:
      case GL_DOUBLE_MAT3x4: case GL_DOUBLE_MAT4x2: case GL_DOUBLE_MAT4x3:
        return sizeof( GLdouble );
      default: break;
    }
    return 0;
  }

  int GetTypeSize( GLenum type ) {
    switch( type ) {
      case GL_FLOAT:
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_BOOL:
        return 4;
      case GL_FLOAT_VEC2:
      case GL_INT_VEC2:
      case GL_UNSIGNED_INT_VEC2:
      case GL_BOOL_VEC2:
        return 4 * 2;
      case GL_FLOAT_VEC3:
      case GL_INT_VEC3:
      case GL_UNSIGNED_INT_VEC3:
      case GL_BOOL_VEC3:
        return 4 * 3;
      case GL_FLOAT_VEC4:
      case GL_INT_VEC4:
      case GL_UNSIGNED_INT_VEC4:
      case GL_BOOL_VEC4:
        return 4 * 4;
      case GL_DOUBLE: return 8;
      case GL_DOUBLE_VEC2: return 8 * 2;
      case GL_DOUBLE_VEC3: return 8 * 3;
      case GL_DOUBLE_VEC4: return 8 * 4;

      case GL_FLOAT_MAT2: case GL_FLOAT_MAT3: case GL_FLOAT_MAT4:
      case GL_FLOAT_MAT2x3:case GL_FLOAT_MAT2x4: case GL_FLOAT_MAT3x2:
      case GL_FLOAT_MAT3x4: case GL_FLOAT_MAT4x2: case GL_FLOAT_MAT4x3:
      case GL_DOUBLE_MAT2: case GL_DOUBLE_MAT3: case GL_DOUBLE_MAT4:
      case GL_DOUBLE_MAT2x3: case GL_DOUBLE_MAT2x4: case GL_DOUBLE_MAT3x2:
      case GL_DOUBLE_MAT3x4: case GL_DOUBLE_MAT4x2: case GL_DOUBLE_MAT4x3: {
        MatrixDims md = GetMatrixDims( type );
        return md.rows * md.cols * GetMatrixElementSize( type );
      }

      case GL_SAMPLER_1D:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_3D:
      case GL_SAMPLER_CUBE:
      case GL_SAMPLER_1D_SHADOW:
      case GL_SAMPLER_2D_SHADOW:
      case GL_SAMPLER_1D_ARRAY:
      case GL_SAMPLER_2D_ARRAY:
      case GL_SAMPLER_1D_ARRAY_SHADOW:
      case GL_SAMPLER_2D_ARRAY_SHADOW:
      case GL_SAMPLER_2D_MULTISAMPLE:
      case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
      case GL_SAMPLER_CUBE_SHADOW:
      case GL_SAMPLER_BUFFER:
      case GL_SAMPLER_2D_RECT:
      case GL_SAMPLER_2D_RECT_SHADOW:
      case GL_INT_SAMPLER_1D:
      case GL_INT_SAMPLER_2D:
      case GL_INT_SAMPLER_3D:
      case GL_INT_SAMPLER_CUBE:
      case GL_INT_SAMPLER_1D_ARRAY:
      case GL_INT_SAMPLER_2D_ARRAY:
      case GL_INT_SAMPLER_2D_MULTISAMPLE:
      case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
      case GL_INT_SAMPLER_BUFFER:
      case GL_INT_SAMPLER_2D_RECT:
      case GL_UNSIGNED_INT_SAMPLER_1D:
      case GL_UNSIGNED_INT_SAMPLER_2D:
      case GL_UNSIGNED_INT_SAMPLER_3D:
      case GL_UNSIGNED_INT_SAMPLER_CUBE:
      case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
      case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
      case GL_UNSIGNED_INT_SAMPLER_BUFFER:
      case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
      case GL_IMAGE_1D:
      case GL_IMAGE_2D:
      case GL_IMAGE_3D:
      case GL_IMAGE_2D_RECT:
      case GL_IMAGE_CUBE:
      case GL_IMAGE_BUFFER:
      case GL_IMAGE_1D_ARRAY:
      case GL_IMAGE_2D_ARRAY:
      case GL_IMAGE_2D_MULTISAMPLE:
      case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
      case GL_INT_IMAGE_1D:
      case GL_INT_IMAGE_2D:
      case GL_INT_IMAGE_3D:
      case GL_INT_IMAGE_2D_RECT:
      case GL_INT_IMAGE_CUBE:
      case GL_INT_IMAGE_BUFFER:
      case GL_INT_IMAGE_1D_ARRAY:
      case GL_INT_IMAGE_2D_ARRAY:
      case GL_INT_IMAGE_2D_MULTISAMPLE:
      case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
      case GL_UNSIGNED_INT_IMAGE_1D:
      case GL_UNSIGNED_INT_IMAGE_2D:
      case GL_UNSIGNED_INT_IMAGE_3D:
      case GL_UNSIGNED_INT_IMAGE_2D_RECT:
      case GL_UNSIGNED_INT_IMAGE_CUBE:
      case GL_UNSIGNED_INT_IMAGE_BUFFER:
      case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
      case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
      case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
      case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
      case GL_UNSIGNED_INT_ATOMIC_COUNTER:
        return 4;
      default: break;
    }
    return 0;
  }

  void GetUniform( DispatchTableGL & tbl, GLuint program, GLint location, GLsizei count, GLenum type, void *value ) {
    GLint size = GetTypeSize( type );
    UniformApiType uat = GetUniformApiType( type );
    char * val = reinterpret_cast<char *>( value );
    switch( uat ) {
      case UAT_Double:
      case UAT_DoubleMatrix: {
        for( int j = 0; j < count; j++ ) {
          tbl.call(&tbl.glGetUniformdv)( program, location + j, reinterpret_cast<GLdouble *>( val + j * size ) );
        }
      } break;
      case UAT_Float:
      case UAT_FloatMatrix: {
        for( int j = 0; j < count; j++ ) {
          tbl.call(&tbl.glGetUniformfv)( program, location + j, reinterpret_cast<GLfloat *>( val + j * size ) );
        }
      } break;
      case UAT_Int: {
        for( int j = 0; j < count; j++ ) {
          tbl.call(&tbl.glGetUniformiv)( program, location + j, reinterpret_cast<GLint *>( val + j * size ) );
        }
      } break;
      case UAT_UnsignedInt: {
        for( int j = 0; j < count; j++ ) {
          tbl.call(&tbl.glGetUniformuiv)( program, location + j, reinterpret_cast<GLuint *>( val + j * size ) );
        }
      } break;
      default:
        break;
    }
  }

  void SetUniform( DispatchTableGL & tbl, GLint location, GLsizei count, GLenum type, const void *value ) {
    GLint size = GetTypeSize( type );
    UniformApiType uat = GetUniformApiType( type );
    const char * val = reinterpret_cast<const char *>( value );
    switch( uat ) {
      case UAT_Double: {
        typedef GLdouble Type;
        typedef void (REGAL_CALL *Uniform)( GLint, GLsizei, const Type *);
        Uniform uniformv[] = { tbl.call(&tbl.glUniform1dv), tbl.call(&tbl.glUniform2dv), tbl.call(&tbl.glUniform3dv), tbl.call(&tbl.glUniform4dv) };
        int idx = ( size / sizeof(Type) ) - 1;
        uniformv[idx]( location, count, reinterpret_cast<const Type *>(val) );
      } break;
      case UAT_Float: {
        typedef GLfloat Type;
        typedef void (REGAL_CALL *Uniform)( GLint, GLsizei, const Type *);
        Uniform uniformv[] = { tbl.call(&tbl.glUniform1fv), tbl.call(&tbl.glUniform2fv), tbl.call(&tbl.glUniform3fv), tbl.call(&tbl.glUniform4fv) };
        int idx = ( size / sizeof(Type) ) - 1;
        uniformv[idx]( location, count, reinterpret_cast<const Type *>(val) );
      } break;
      case UAT_Int: {
        typedef GLint Type;
        typedef void (REGAL_CALL *Uniform)( GLint, GLsizei, const Type *);
        Uniform uniformv[] = { tbl.call(&tbl.glUniform1iv), tbl.call(&tbl.glUniform2iv), tbl.call(&tbl.glUniform3iv), tbl.call(&tbl.glUniform4iv) };
        int idx = ( size / sizeof(Type) ) - 1;
        uniformv[idx]( location, count, reinterpret_cast<const Type *>(val) );
      } break;
      case UAT_UnsignedInt: {
        typedef GLuint Type;
        typedef void (REGAL_CALL *Uniform)( GLint, GLsizei, const Type *);
        Uniform uniformv[] = { tbl.call(&tbl.glUniform1uiv), tbl.call(&tbl.glUniform2uiv), tbl.call(&tbl.glUniform3uiv), tbl.call(&tbl.glUniform4uiv) };
        int idx = ( size / sizeof(Type) ) - 1;
        uniformv[idx]( location, count, reinterpret_cast<const Type *>(val) );
      } break;
      case UAT_DoubleMatrix: {
        typedef GLdouble Type;
        typedef void (REGAL_CALL *Uniform)( GLint, GLsizei, GLboolean, const Type *);
        Uniform uniformv[3][3] = {
          { tbl.call(&tbl.glUniformMatrix2dv),   tbl.call(&tbl.glUniformMatrix2x3dv), tbl.call(&tbl.glUniformMatrix2x4dv) },
          { tbl.call(&tbl.glUniformMatrix3x2dv), tbl.call(&tbl.glUniformMatrix3dv),   tbl.call(&tbl.glUniformMatrix3x4dv) },
          { tbl.call(&tbl.glUniformMatrix4x2dv), tbl.call(&tbl.glUniformMatrix4x3dv), tbl.call(&tbl.glUniformMatrix4dv)   }
        };
        MatrixDims md = GetMatrixDims( type );
        uniformv[ md.rows - 2 ][ md.cols - 2 ]( location, count, GL_TRUE, reinterpret_cast<const Type *>(val) );
      } break;
      case UAT_FloatMatrix: {
        typedef GLfloat Type;
        typedef void (REGAL_CALL *Uniform)( GLint, GLsizei, GLboolean, const Type *);
        Uniform uniformv[3][3] = {
          { tbl.call(&tbl.glUniformMatrix2fv),   tbl.call(&tbl.glUniformMatrix2x3fv), tbl.call(&tbl.glUniformMatrix2x4fv) },
          { tbl.call(&tbl.glUniformMatrix3x2fv), tbl.call(&tbl.glUniformMatrix3fv),   tbl.call(&tbl.glUniformMatrix3x4fv) },
          { tbl.call(&tbl.glUniformMatrix4x2fv), tbl.call(&tbl.glUniformMatrix4x3fv), tbl.call(&tbl.glUniformMatrix4fv)   }
        };
        MatrixDims md = GetMatrixDims( type );
        uniformv[ md.rows - 2 ][ md.cols - 2 ]( location, count, GL_TRUE, reinterpret_cast<const Type *>(val) );
      } break;
      default:
        break;
    }

  }

  void Program::AddUniform( const string & name, GLint loc, GLint count, GLenum type ) {
    Uniform u;
    u.name = name;
    u.loc = loc;
    u.count = count;
    u.type = type;
    u.offset = int( uniformStore.size() * sizeof( GLuint ) );
    u.ver = 0;
    // FIXME: deal with alignment
    int sz = GetTypeSize( u.type );
    uniformStore.insert( uniformStore.end(), ( sz * u.count ) / sizeof( GLint ), 0 );
    uniforms[ loc ] =  u;
  }

  void Program::UpdateUniformStore( GLint location, GLint count, const void * ptr ) {
    int found_location = -1;
    if( uniforms.find( location ) == uniforms.end() ) {

      // check if the location is within a uniform array
      for( Program::uniforms_iter iter = uniforms.begin(); iter != uniforms.end(); iter++ ) {
        Uniform &test = iter->second;

        // if the test uniform is not an array, skip
        if ( test.count < 2 )
        {
          continue;
        }

        // if the location is before this array, skip
        if ( location - test.loc < 0 )
        {
          continue;
        }

        // if the location is after the array, skip
        if ( location > test.loc + test.count)
        {
          continue;
        }

        found_location = test.loc;
        break;
      }

      // if the location is not found, then bail on the update
      if (found_location == -1) {
        Warning( "UpdateUniformStore() skipping update to unknown uniform location ", location );
        return;
      }
    } else {
      found_location = location;
    }

    Uniform &u = uniforms[ found_location ];
    GLint locoffset = location - u.loc;
    if( location + count > u.loc + u.count ) {
      // abort update.  trying to update past the end of allocated storage
      Error( "UpdateUniformStore() trying to update past the end of allocated storage." );
      return;
    }
    int sz = GetTypeSize( u.type );
    memcpy( reinterpret_cast<char *>(&uniformStore[0]) + u.offset + sz * locoffset, ptr, sz * count );
    u.ver = ++ver;
  }

  void ProgramInstance::InitializeUniforms( DispatchTableGL & tbl, const Program & p ) {
    uniforms.clear();
    for( Program::Map::const_reverse_iterator it = p.uniforms.rbegin(); it != p.uniforms.rend(); ++it ) {
      const Uniform & u = it->second;
      UniformInstance ui;
      ui.location = u.loc;
      ui.instanceLocation = tbl.call(&tbl.glGetUniformLocation)( prog, u.name.c_str() );
      ui.ver = ~ GLuint64(0);

      uniforms.push_back( ui );
    }
  }

  void ProgramInstance::UpdateUniforms( DispatchTableGL & tbl, const Program & p ) {
    if( ver == p.ver ) {
      return;
    }
    for( size_t i = 0; i < uniforms.size(); i++ ) {
      UniformInstance & ui = uniforms[i];
      const Uniform & u = p.uniforms.find( ui.location )->second;
      if( ui.ver != u.ver ) {
        SetUniform( tbl, ui.instanceLocation, u.count, u.type, reinterpret_cast<const char *>(&p.uniformStore[0]) + u.offset );
        ui.ver = u.ver;
      }
    }
    ver = p.ver;
  }


  void GetShaderSource( DispatchTableGL & tbl, GLuint shader, ShaderSource & ss ) {
    tbl.call(&tbl.glGetShaderiv)( shader, GL_SHADER_TYPE, reinterpret_cast<GLint *>(&ss.type) );
    GLint sz = 0;
    tbl.call(&tbl.glGetShaderiv)( shader, GL_SHADER_SOURCE_LENGTH, &sz );
    GLchar *src = new GLchar[ sz + 1 ];
    GLsizei sz1 = 0;
    tbl.call(&tbl.glGetShaderSource)( shader, sz + 1, &sz1, src );
    src[sz] = 0;
    ss.src = src;
    delete [] src;
  }

  void GetProgramSources( DispatchTableGL & tbl, GLuint prog, std::vector<ShaderSource> & sources ) {
    sources.clear();
    GLuint shaders[8];
    GLsizei numShaders = 0;
    tbl.call(&tbl.glGetAttachedShaders)( prog, (GLsizei)array_size(shaders), &numShaders, shaders );

    for( int i = 0; i < numShaders; i++ ) {
      sources.push_back( ShaderSource() );
      ShaderSource & ss = sources.back();
      GetShaderSource( tbl, shaders[i], ss );
    }
  }

  GLuint CreateShader( DispatchTableGL & tbl, const ShaderSource & ss ) {
    GLuint s = tbl.call(&tbl.glCreateShader)( ss.type );
    const GLchar *dumb[] =  { ss.src.c_str(), NULL };
    GLsizei sizes[] = { (GLsizei)ss.src.size(), 0 };
    tbl.call(&tbl.glShaderSource)( s, 1, dumb, sizes );
    tbl.call(&tbl.glCompileShader)( s );
    char dbgLog[1<<15];
    int dbgLogLen = 0;
    tbl.call(&tbl.glGetShaderInfoLog)( s, (1<<15) - 2, &dbgLogLen, dbgLog );
    dbgLog[ dbgLogLen ] = 0;
    return s;
  }


  void InitProgram( DispatchTableGL & tbl, GLuint prog, Program & p ) {
    GLint activeUniforms = 0;
    tbl.call(&tbl.glGetProgramiv)( prog, GL_ACTIVE_UNIFORMS, &activeUniforms );
    tbl.call(&tbl.glGetError)();
    p = Program();
    p.prog = prog;
    p.ver = 0;
    GLsizei nameLen = 0;
    GLint count;
    GLenum type;
    GLsizei bufSize;
    tbl.call(&tbl.glGetProgramiv)( prog, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufSize);
    GLchar *name = (GLchar*)malloc( bufSize * sizeof(GLchar) );
    for( int i = 0; i < activeUniforms; i++ ) {
      tbl.call(&tbl.glGetActiveUniform)( prog, i, 80, &nameLen, &count, &type, name );
      name[nameLen] = 0;
      if( strncmp( name, "rgl", 3 ) == 0 ) {
        continue; // rgl namespace is reserved
      }
      GLint loc = tbl.call(&tbl.glGetUniformLocation)( prog, name );
      p.AddUniform( name, loc, count, type );
      char buf[4096];
      GetUniform( tbl, prog, loc, count, type, buf );
      p.UpdateUniformStore( loc, count, buf );
    }
    free(name);
  }

  void InitProgramInstance( DispatchTableGL & tbl, const Program & p, GLuint inst, ProgramInstance &pi ) {

    pi.prog = inst;

    tbl.call(&tbl.glLinkProgram)( pi.prog );

    GLint activeAttributes = 0;
    tbl.call(&tbl.glGetProgramiv)( p.prog, GL_ACTIVE_ATTRIBUTES, &activeAttributes );

    for( int i = 0; i < activeAttributes; i++ ) {
      GLchar name[80];
      GLsizei nameLen = 0;
      GLint size;
      GLenum type;
      tbl.call(&tbl.glGetActiveAttrib)( p.prog, i, 80, &nameLen, &size, &type, name );
      name[nameLen] = 0;
      GLint loc = tbl.call(&tbl.glGetAttribLocation)( p.prog, name );
      tbl.call(&tbl.glBindAttribLocation)( pi.prog, loc, name );
    }

    tbl.call(&tbl.glLinkProgram)( pi.prog );
    {
      char dbgLog[1<<15];
      int dbgLogLen = 0;
      tbl.call(&tbl.glGetProgramInfoLog)( pi.prog, (1<<15) - 2, &dbgLogLen, dbgLog );
      dbgLog[ dbgLogLen ] = 0;
    }

    pi.InitializeUniforms( tbl, p );
    // now the uniforms only need to be updated for the instance to be used
  }

  void CreateProgramInstance( DispatchTableGL & tbl, const Program & p, const std::vector<ShaderSource> & sources, ProgramInstance &pi ) {
    GLuint inst = tbl.call(&tbl.glCreateProgram)();
    for( int i = 0; i < (int)sources.size(); i++ ) {
      GLuint si = CreateShader( tbl, sources[i] );
      tbl.call(&tbl.glAttachShader)( inst, si );
    }
    InitProgramInstance( tbl, p, inst, pi );
  }

}

REGAL_NAMESPACE_END

#endif // REGAL_EMULATION



