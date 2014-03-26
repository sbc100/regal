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

#ifndef __REGAL_SHADER_INSTANCE_H__
#define __REGAL_SHADER_INSTANCE_H__

#if REGAL_EMULATION

#include "RegalUtil.h"

REGAL_GLOBAL_BEGIN

#include <map>
#include <string>
#include <vector>
#include <functional>

#include "RegalEmu.h"
#include "RegalPrivate.h"
#include "RegalContext.h"


REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

namespace ShaderInstance {

  struct ShaderSource {
    GLenum type;
    std::string src;
  };

  struct Uniform {
    std::string name;
    GLint loc;
    GLint count;
    GLenum type;
    int offset; // byte offset into a uniform store
    GLuint64 ver;
  };

  struct Program {
    Program() : prog(0), ver(0) {}
    void AddUniform( const std::string & name, GLint loc, GLint count, GLenum type );
    void UpdateUniformStore( GLint location, GLint count, const void * ptr );

    GLint prog;
    GLuint64 ver;
    typedef std::map<GLint,Uniform> Map;
    typedef std::map<GLint,Uniform>::iterator uniforms_iter;
    Map uniforms;
    std::vector<GLuint> uniformStore;
  };

  struct UniformInstance {
    UniformInstance() : location(-1), instanceLocation(-1), ver(~ GLuint64(0)) {}
    GLint location;
    GLint instanceLocation;
    GLuint64 ver;
  };

  struct ProgramInstance {
    ProgramInstance() : prog(0), ver(~ GLuint64(0)) {}
    GLint prog;
    GLuint64 ver;
    std::vector<UniformInstance> uniforms;
    void InitializeUniforms( DispatchTableGL & tbl, const Program & p );
    void UpdateUniforms( DispatchTableGL & tbl, const Program & p );
  };

  void GetShaderSource( DispatchTableGL & tbl, GLuint shader, ShaderSource & ss );
  void GetProgramSources( DispatchTableGL & tbl, GLuint prog, std::vector<ShaderSource> & sources );
  void InitProgram( DispatchTableGL & tbl, GLuint prog, Program & p );
  void InitProgramInstance( DispatchTableGL & tbl, const Program & p, GLuint inst, ProgramInstance & pi );
  void CreateProgramInstance( DispatchTableGL & tbl, const Program & p, const std::vector<ShaderSource> & sources, ProgramInstance & pi );

  int GetTypeSize( GLenum type );
  void GetUniform( DispatchTableGL & tbl, GLuint program, GLint location, GLsizei count, GLenum type, void *value );
  void SetUniform( DispatchTableGL & tbl, GLint location, GLsizei count, GLenum type, const void *value );
}

REGAL_NAMESPACE_END

#endif // REGAL_EMULATION

#endif // ! __REGAL_SHADER_INSTANCE_H__
