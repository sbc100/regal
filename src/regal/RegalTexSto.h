/*
  Copyright (c) 2011-2013 NVIDIA Corporation
  Copyright (c) 2011-2013 Cass Everitt
  Copyright (c) 2012 Scott Nations
  Copyright (c) 2012 Mathias Schott
  Copyright (c) 2012 Nigel Stewart
  Copyright (c) 2013 Jason Allen
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

 Regal texture storage support
 Jason Allen

 */

#ifndef __REGAL_BIN_H__
#define __REGAL_BIN_H__

#include "RegalUtil.h"

REGAL_GLOBAL_BEGIN

#include "RegalEmu.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

namespace Emu {

struct TexSto : public RegalEmu
{
  void Init( RegalContext &ctx )
  {
    UNUSED_PARAMETER(ctx);
  }

  void TextureStorage( RegalContext * ctx, GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width ) {
    DispatchTable & tbl = ctx->dispatcher.emulation;
    tbl.call(&tbl.glTextureStorage1D)( texture, target, levels, internalFormat, width );
  }
  void TextureStorage( RegalContext * ctx, GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height ) {
    DispatchTable & tbl = ctx->dispatcher.emulation;
    tbl.call(&tbl.glTextureStorage2D)( texture, target, levels, internalFormat, width, height );
  }
  void TextureStorage( RegalContext * ctx, GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth ) {
    DispatchTable & tbl = ctx->dispatcher.emulation;
    tbl.call(&tbl.glTextureStorage3D)( texture, target, levels, internalFormat, width, height, depth );
  }
};

}

REGAL_NAMESPACE_END

#endif // ! __REGAL_BIN_H__
