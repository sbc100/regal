/*
  Copyright (c) 2011-2013 NVIDIA Corporation
  Copyright (c) 2011-2013 Cass Everitt
  Copyright (c) 2012-2013 Scott Nations
  Copyright (c) 2012-2013 Mathias Schott
  Copyright (c) 2012-2013 Nigel Stewart
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

#include "pch.h" /* For MS precompiled header support */

#include "RegalUtil.h"

#if REGAL_EMULATION

REGAL_GLOBAL_BEGIN

#include "RegalLog.h"
#include "RegalFilt.h"
#include "RegalToken.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

namespace Emu {

  using namespace ::REGAL_NAMESPACE_INTERNAL::Logging;
  using namespace ::REGAL_NAMESPACE_INTERNAL::Token;

  void Filt::BindTexture(const RegalContext &ctx, GLenum target, GLuint name)
  {
    UNUSED_PARAMETER(ctx);
    UNUSED_PARAMETER(target);
    UNUSED_PARAMETER(name);

    if (ctx.isES2())
    {
      switch( target )
      {
        case GL_TEXTURE_1D:
        case GL_TEXTURE_3D:
          Warning( "Regal does not support ", GLenumToString( target ), " as target for glBindTexture for ES 2.0 profile - skipping." );
          filtered = true;
        default:
          break;
      }
    }
  }

  void Filt::RenderMode(const RegalContext &ctx, GLenum mode)
  {
    UNUSED_PARAMETER(ctx);
    UNUSED_PARAMETER(mode);

    if (ctx.isCore() || ctx.isES2())
      if (mode!=GL_RENDER)
      {
        Warning("Regal does not support ", GLenumToString(mode), " render mode for core or ES 2.0 profiles, only GL_RENDER is supported in those profiles - skipping.");
        filtered = true;
      }
  }

  void Filt::TexParameter(const RegalContext &ctx, GLenum target, GLenum pname, GLint param)
  {
    TexParameter(ctx,target,pname,GLfloat(param));
  }

  void Filt::TexParameter(const RegalContext &ctx, GLenum target, GLenum pname, GLfloat param)
  {
    UNUSED_PARAMETER(ctx);
    UNUSED_PARAMETER(target);
    UNUSED_PARAMETER(pname);
    UNUSED_PARAMETER(param);

    if (ctx.isCore() || ctx.isES2())
      switch(pname)
      {
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
        case GL_TEXTURE_WRAP_R:
          switch(int(param))
          {
            case GL_CLAMP:
              Warning("Regal does not support GL_CLAMP wrap mode for core or ES 2.0 profiles - skipping.");
              filtered = true;
            default: break;
          }
        default: break;
      }
  }

  void Filt::PixelStorei(const RegalContext &ctx, GLenum pname, GLint param)
  {
    UNUSED_PARAMETER(ctx);
    UNUSED_PARAMETER(pname);
    UNUSED_PARAMETER(param);

    if (ctx.isES2())
    {
      switch(pname)
      {
        case GL_PACK_ALIGNMENT:
        case GL_UNPACK_ALIGNMENT:
          break;

        case GL_UNPACK_ROW_LENGTH_EXT:
        case GL_UNPACK_SKIP_ROWS_EXT:
        case GL_UNPACK_SKIP_PIXELS_EXT:
          if (!ctx.info->gl_ext_unpack_subimage)
          {
            Warning("glPixelStorei ", GLenumToString(pname),
                    " not supported for ES 2.0 without EXT_unpack_subimage.");
            filtered = true;
          }
          break;

        case GL_UNPACK_SKIP_IMAGES_NV:
        case GL_UNPACK_IMAGE_HEIGHT_NV:
          if (!ctx.info->gl_ext_unpack_subimage || !ctx.info->gl_nv_texture_array)
          {
            Warning("glPixelStorei ", GLenumToString(pname),
                    " not supported for ES 2.0 without EXT_unpack_subimage and NV_texture_array.");
            filtered = true;
          }
          break;

        case GL_PACK_ROW_LENGTH_NV:
        case GL_PACK_SKIP_ROWS_NV:
        case GL_PACK_SKIP_PIXELS_NV:
          if (!ctx.info->gl_nv_pack_subimage)
          {
            Warning("glPixelStorei ", GLenumToString(pname),
                    " not supported for ES 2.0 without NV_pack_subimage.");
            filtered = true;
          }
          break;

        case GL_PACK_IMAGE_HEIGHT:
        case GL_PACK_SKIP_IMAGES:
          if (!ctx.info->gl_nv_pack_subimage || !ctx.info->gl_nv_texture_array)
          {
            Warning("glPixelStorei ", GLenumToString(pname),
                    " not supported for ES 2.0 without NV_pack_subimage and NV_texture_array.");
            filtered = true;
          }
          break;

        default:
          Warning("glPixelStorei ", GLenumToString(pname), " not supported for ES 2.0.");
          filtered = true;
          break;
      }
    }
  }

  void Filt::PolygonMode(const RegalContext &ctx, GLenum face, GLenum mode)
  {
    UNUSED_PARAMETER(ctx);
    UNUSED_PARAMETER(face);
    UNUSED_PARAMETER(mode);

    if (ctx.isCore())
    {
      if (face!=GL_FRONT_AND_BACK)
      {
        Warning("Regal does not support ", GLenumToString(face), " in glPolygonMode for core profile, only GL_FRONT_AND_BACK is supported - skipping.");
        filtered = true;
      }
    }

    if (ctx.isES2())
    {
      Warning("Regal does not support glPolygonMode for ES 2.0 - skipping.");
      filtered = true;
    }
  }

  void Filt::FilterGet(const RegalContext &ctx, GLenum pname)
  {
    UNUSED_PARAMETER(ctx);
    UNUSED_PARAMETER(pname);

    if (ctx.isCore() || ctx.isES2())
    {
      filtered = true;
      switch (pname )
      {
        case GL_MAX_PIXEL_MAP_TABLE:           retVal = 256;  break;
        case GL_MAX_NAME_STACK_DEPTH:          retVal = 128;  break;
        case GL_MAX_LIST_NESTING:              retVal = 64;   break;
        case GL_MAX_CLIENT_ATTRIB_STACK_DEPTH:
        case GL_MAX_ATTRIB_STACK_DEPTH:        retVal = 16;   break;

        case GL_DEPTH_BITS:                    retVal = 24;   break;

        case GL_RED_BITS:
        case GL_GREEN_BITS:
        case GL_BLUE_BITS:
        case GL_ALPHA_BITS:
        case GL_STENCIL_BITS:
          retVal = 8;
          break;

        case GL_INDEX_MODE:
        case GL_UNPACK_LSB_FIRST:
        case GL_UNPACK_SWAP_BYTES:
        case GL_PACK_LSB_FIRST:
        case GL_PACK_SWAP_BYTES:
          retVal = 0;
          break;
        default:
          filtered = false;
          break;
      }
      if (filtered)
      {
        Warning( "Regal does not support ", GLenumToString(pname), " as pname for glGet for core or ES 2.0 profiles - skipping." );
        return;
      }
    }

    if (ctx.isCore())
    {
      filtered = true;
#if 0
      switch (pname )
      {
          /* just a test
           case GL_UNPACK_ALIGNMENT:
           retVal = 0;
           break;
           */
        default:
          filtered = false;
          break;
      }
#else
      filtered = false;
#endif
      if (filtered)
      {
        Warning( "Regal does not support ", GLenumToString(pname), " as pname for glGet for core profile - skipping." );
        return;
      }
    }

    if (ctx.isES2())
    {
      filtered = true;
      switch (pname) {
        case GL_PACK_ROW_LENGTH_NV:
        case GL_PACK_SKIP_ROWS_NV:
        case GL_PACK_SKIP_PIXELS_NV:
          if (ctx.info->gl_nv_pack_subimage)
            filtered = false;
          else
            retVal = 0;
          break;

        case GL_PACK_IMAGE_HEIGHT:
        case GL_PACK_SKIP_IMAGES:
          if (ctx.info->gl_nv_pack_subimage && ctx.info->gl_nv_texture_array)
            filtered = false;
          else
            retVal = 0;
          break;

        case GL_UNPACK_ROW_LENGTH_EXT:
        case GL_UNPACK_SKIP_ROWS_EXT:
        case GL_UNPACK_SKIP_PIXELS_EXT:
          if (ctx.info->gl_ext_unpack_subimage)
            filtered = false;
          else
            retVal = 0;
          break;

        case GL_UNPACK_IMAGE_HEIGHT_NV:
        case GL_UNPACK_SKIP_IMAGES_NV:
          if (ctx.info->gl_ext_unpack_subimage && ctx.info->gl_nv_texture_array)
            filtered = false;
          else
            retVal = 0;
          break;

        default:
          filtered = false;
          break;
      }
      if (filtered)
      {
        Warning( "Regal does not support ", GLenumToString(pname), " as pname for glGet for ES 2.0 profile - skipping." );
        return;
      }
    }
  }
}

REGAL_NAMESPACE_END

#endif // REGAL_EMULATION
