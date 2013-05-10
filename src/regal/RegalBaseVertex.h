/*
  Copyright (c) 2011-2013 NVIDIA Corporation
  Copyright (c) 2011-2012 Cass Everitt
  Copyright (c) 2012 Scott Nations
  Copyright (c) 2012 Mathias Schott
  Copyright (c) 2012 Nigel Stewart
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

/*

 Regal Draw Elements Base Vertex emu layer
 Nigel Stewart, Scott Nations

 */

#ifndef __REGAL_BASEVERTEX_H__
#define __REGAL_BASEVERTEX_H__

#include "RegalUtil.h"

#if REGAL_EMULATION

REGAL_GLOBAL_BEGIN

#include <vector>

#include <GL/Regal.h>

#include "RegalEmu.h"
#include "RegalContext.h"
#include "RegalContextInfo.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

namespace Emu {

// Work in progress...

struct BaseVertex
{
  void Init(RegalContext &ctx)
  {
    UNUSED_PARAMETER(ctx);
  }

  void Cleanup(RegalContext &ctx)
  {
    UNUSED_PARAMETER(ctx);
  }

  bool glDrawElementsBaseVertex(RegalContext &ctx, GLenum mode, GLsizei count, GLenum type, GLvoid *indices, GLint basevertex)
  {
    UNUSED_PARAMETER(ctx);
    UNUSED_PARAMETER(mode);
    UNUSED_PARAMETER(count);
    UNUSED_PARAMETER(type);
    UNUSED_PARAMETER(indices);
    UNUSED_PARAMETER(basevertex);
    return false;
  }

  bool glDrawRangeElementsBaseVertex(RegalContext &ctx, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLvoid *indices, GLint basevertex)
  {
    UNUSED_PARAMETER(ctx);
    UNUSED_PARAMETER(mode);
    UNUSED_PARAMETER(start);
    UNUSED_PARAMETER(end);
    UNUSED_PARAMETER(count);
    UNUSED_PARAMETER(type);
    UNUSED_PARAMETER(indices);
    UNUSED_PARAMETER(basevertex);
    return false;
  }

  bool glDrawElementsInstancedBaseVertex(RegalContext &ctx, GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount, GLint basevertex)
  {
    UNUSED_PARAMETER(ctx);
    UNUSED_PARAMETER(mode);
    UNUSED_PARAMETER(count);
    UNUSED_PARAMETER(type);
    UNUSED_PARAMETER(indices);
    UNUSED_PARAMETER(primcount);
    UNUSED_PARAMETER(basevertex);
    return false;
  }

  bool glMultiDrawElementsBaseVertex(RegalContext &ctx, GLenum mode, GLsizei *count, GLenum type, GLvoid **indices, GLsizei primcount, GLint *basevertex)
  {
    UNUSED_PARAMETER(ctx);
    UNUSED_PARAMETER(mode);
    UNUSED_PARAMETER(count);
    UNUSED_PARAMETER(type);
    UNUSED_PARAMETER(indices);
    UNUSED_PARAMETER(primcount);
    UNUSED_PARAMETER(basevertex);
    return false;
  }

};

}

REGAL_NAMESPACE_END

#endif // REGAL_EMULATION

#endif // ! __REGAL_BASEVERTEX_H__
