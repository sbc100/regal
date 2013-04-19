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

 Regal push / pop attrib
 Nigel Stewart, Scott Nations

 */

#ifndef __REGAL_PPA_H__
#define __REGAL_PPA_H__

#include "RegalUtil.h"

#if REGAL_EMULATION

REGAL_GLOBAL_BEGIN

#include <vector>

#include <GL/Regal.h>

#include "RegalState.h"
#include "RegalEmu.h"
#include "RegalLog.h"
#include "RegalToken.h"
#include "RegalContext.h"
#include "RegalContextInfo.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

namespace Emu {

// Work in progress...

struct Ppa : public State::Stencil, State::Depth, State::Polygon, State::Transform, State::Hint,
             State::Enable, State::List, State::AccumBuffer, State::Scissor, State::Viewport, State::Line,
             State::Multisample, State::Eval, State::Fog, State::Point, State::PolygonStipple
{
  void Init(RegalContext &ctx)
  {
    UNUSED_PARAMETER(ctx);
    activeTextureUnit = 0;
  }

  void Cleanup(RegalContext &ctx)
  {
    UNUSED_PARAMETER(ctx);
  }

  void PushAttrib(RegalContext *ctx, GLbitfield mask)
  {
    maskStack.push_back(mask);

    if (mask&GL_DEPTH_BUFFER_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_DEPTH_BUFFER_BIT ",State::Depth::toString());
      depthStack.push_back(State::Depth());
      depthStack.back() = *this;
      mask &= ~GL_DEPTH_BUFFER_BIT;
    }

    if (mask&GL_STENCIL_BUFFER_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_STENCIL_BUFFER_BIT ",State::Stencil::toString());
      stencilStack.push_back(State::Stencil());
      stencilStack.back() = *this;
      mask &= ~GL_STENCIL_BUFFER_BIT;
    }

    if (mask&GL_POLYGON_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_POLYGON_BIT ",State::Polygon::toString());
      polygonStack.push_back(State::Polygon());
      polygonStack.back() = *this;
      mask &= ~GL_POLYGON_BIT;
    }

    if (mask&GL_TRANSFORM_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_TRANSFORM_BIT ",State::Transform::toString());
      transformStack.push_back(State::Transform());
      transformStack.back() = *this;
      mask &= ~GL_TRANSFORM_BIT;
    }

    if (mask&GL_HINT_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_HINT_BIT ",State::Hint::toString());
      hintStack.push_back(State::Hint());
      hintStack.back() = *this;
      mask &= ~GL_HINT_BIT;
    }

    if (mask&GL_ENABLE_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_ENABLE_BIT ",State::Enable::toString());
      enableStack.push_back(State::Enable());
      enableStack.back() = *this;
      mask &= ~GL_ENABLE_BIT;
    }

    if (mask&GL_LIST_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_LIST_BIT ",State::List::toString());
      listStack.push_back(State::List());
      listStack.back() = *this;
      mask &= ~GL_LIST_BIT;
    }

    if (mask&GL_ACCUM_BUFFER_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_ACCUM_BUFFER_BIT ",State::AccumBuffer::toString());
      accumBufferStack.push_back(State::AccumBuffer());
      accumBufferStack.back() = *this;
      mask &= ~GL_ACCUM_BUFFER_BIT;
    }

    if (mask&GL_SCISSOR_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_SCISSOR_BIT ",State::Scissor::toString());
      if (!State::Scissor::defined())
        State::Scissor::define(ctx->dispatcher.emulation);
      scissorStack.push_back(State::Scissor());
      scissorStack.back() = *this;
      mask &= ~GL_SCISSOR_BIT;
    }

    if (mask&GL_VIEWPORT_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_VIEWPORT_BIT ",State::Viewport::toString());
      if (!State::Viewport::defined())
        State::Viewport::define(ctx->dispatcher.emulation);
      viewportStack.push_back(State::Viewport());
      viewportStack.back() = *this;
      mask &= ~GL_VIEWPORT_BIT;
    }

    if (mask&GL_LINE_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_LINE_BIT ",State::Line::toString());
      lineStack.push_back(State::Line());
      lineStack.back() = *this;
      mask &= ~GL_LINE_BIT;
    }

    if (mask&GL_MULTISAMPLE_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_MULTISAMPLE_BIT ",State::Multisample::toString());
      multisampleStack.push_back(State::Multisample());
      multisampleStack.back() = *this;
      mask &= ~GL_MULTISAMPLE_BIT;
    }

    if (mask&GL_EVAL_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_EVAL_BIT ",State::Eval::toString());
      evalStack.push_back(State::Eval());
      evalStack.back() = *this;
      mask &= ~GL_EVAL_BIT;
    }

    if (mask&GL_FOG_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_FOG_BIT ",State::Fog::toString());
      fogStack.push_back(State::Fog());
      fogStack.back() = *this;
      mask &= ~GL_FOG_BIT;
    }

    if (mask&GL_POINT_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_POINT_BIT ",State::Point::toString());
      pointStack.push_back(State::Point());
      pointStack.back() = *this;
      mask &= ~GL_POINT_BIT;
    }

    if (mask&GL_POLYGON_STIPPLE_BIT)
    {
      Internal("Regal::Ppa::PushAttrib GL_POLYGON_STIPPLE_BIT ",State::PolygonStipple::toString());
      polygonStippleStack.push_back(State::PolygonStipple());
      polygonStippleStack.back() = *this;
      mask &= ~GL_POLYGON_STIPPLE_BIT;
    }

    // Pass the rest through, for now

    RegalAssert(ctx);

    if (ctx->info->core || ctx->info->es1 || ctx->info->es2)
      return;

    if (mask)
      ctx->dispatcher.emulation.glPushAttrib(mask);
  }

  void PopAttrib(RegalContext *ctx)
  {
    RegalAssert(ctx);
    RegalAssert(maskStack.size());

    if (maskStack.size())
    {
      GLbitfield mask = maskStack.back();
      maskStack.pop_back();

      if (mask&GL_DEPTH_BUFFER_BIT)
      {
        RegalAssert(depthStack.size());
        State::Depth::swap(depthStack.back());
        depthStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_DEPTH_BUFFER_BIT ",State::Depth::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::Depth::set(ctx->dispatcher.emulation);

        mask &= ~GL_DEPTH_BUFFER_BIT;
      }

      if (mask&GL_STENCIL_BUFFER_BIT)
      {
        RegalAssert(stencilStack.size());
        State::Stencil::swap(stencilStack.back());
        stencilStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_STENCIL_BUFFER_BIT ",State::Stencil::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::Stencil::set(ctx->dispatcher.emulation);

        mask &= ~GL_STENCIL_BUFFER_BIT;
      }

      if (mask&GL_POLYGON_BIT)
      {
        RegalAssert(polygonStack.size());
        State::Polygon::swap(polygonStack.back());
        polygonStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_POLYGON_BIT ",State::Polygon::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::Polygon::set(ctx->dispatcher.emulation);

        mask &= ~GL_POLYGON_BIT;
      }

      if (mask&GL_TRANSFORM_BIT)
      {
        RegalAssert(transformStack.size());
        State::Transform::swap(transformStack.back());

        Internal("Regal::Ppa::PopAttrib GL_TRANSFORM_BIT ",State::Transform::toString());

        State::Transform::transition(ctx->dispatcher.emulation, transformStack.back());
        transformStack.pop_back();

        mask &= ~GL_TRANSFORM_BIT;
      }

      if (mask&GL_HINT_BIT)
      {
        RegalAssert(hintStack.size());
        State::Hint::swap(hintStack.back());
        hintStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_HINT_BIT ",State::Hint::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::Hint::set(ctx->dispatcher.emulation);

        mask &= ~GL_HINT_BIT;
      }

      if (mask&GL_ENABLE_BIT)
      {
        RegalAssert(enableStack.size());
        State::Enable::swap(enableStack.back());
        enableStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_ENABLE_BIT ",State::Enable::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::Enable::set(ctx->dispatcher.emulation);

        mask &= ~GL_ENABLE_BIT;
      }

      if (mask&GL_LIST_BIT)
      {
        RegalAssert(listStack.size());
        State::List::swap(listStack.back());
        listStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_LIST_BIT ",State::List::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::List::set(ctx->dispatcher.emulation);

        mask &= ~GL_LIST_BIT;
      }

      if (mask&GL_ACCUM_BUFFER_BIT)
      {
        RegalAssert(accumBufferStack.size());
        State::AccumBuffer::swap(accumBufferStack.back());
        accumBufferStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_ACCUM_BUFFER_BIT ",State::AccumBuffer::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::AccumBuffer::set(ctx->dispatcher.emulation);

        mask &= ~GL_ACCUM_BUFFER_BIT;
      }

      if (mask&GL_SCISSOR_BIT)
      {
        RegalAssert(scissorStack.size());
        State::Scissor::swap(scissorStack.back());
        scissorStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_SCISSOR_BIT ",State::Scissor::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        if (!State::Scissor::defined())
          State::Scissor::define(ctx->dispatcher.emulation);
        State::Scissor::set(ctx->dispatcher.emulation);

        mask &= ~GL_SCISSOR_BIT;
      }

      if (mask&GL_VIEWPORT_BIT)
      {
        RegalAssert(viewportStack.size());
        State::Viewport::swap(viewportStack.back());
        viewportStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_VIEWPORT_BIT ",State::Viewport::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        if (!State::Viewport::defined())
          State::Viewport::define(ctx->dispatcher.emulation);
        State::Viewport::set(ctx->dispatcher.emulation);

        mask &= ~GL_VIEWPORT_BIT;
      }

      if (mask&GL_LINE_BIT)
      {
        RegalAssert(lineStack.size());
        State::Line::swap(lineStack.back());
        lineStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_LINE_BIT ",State::Line::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::Line::set(ctx->dispatcher.emulation);

        mask &= ~GL_LINE_BIT;
      }

      if (mask&GL_MULTISAMPLE_BIT)
      {
        RegalAssert(multisampleStack.size());
        State::Multisample::swap(multisampleStack.back());
        multisampleStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_MULTISAMPLE_BIT ",State::Multisample::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::Multisample::set(ctx->dispatcher.emulation);

        mask &= ~GL_MULTISAMPLE_BIT;
      }

      if (mask&GL_EVAL_BIT)
      {
        RegalAssert(evalStack.size());
        State::Eval::swap(evalStack.back());
        evalStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_EVAL_BIT ",State::Eval::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::Eval::set(ctx->dispatcher.emulation);

        mask &= ~GL_EVAL_BIT;
      }

      if (mask&GL_FOG_BIT)
      {
        RegalAssert(fogStack.size());
        State::Fog::swap(fogStack.back());
        fogStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_FOG_BIT ",State::Fog::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::Fog::set(ctx->dispatcher.emulation);

        mask &= ~GL_FOG_BIT;
      }

      if (mask&GL_POINT_BIT)
      {
        RegalAssert(pointStack.size());
        State::Point::swap(pointStack.back());
        pointStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_POINT_BIT ",State::Point::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::Point::set(ctx->dispatcher.emulation);

        mask &= ~GL_POINT_BIT;
      }

      if (mask&GL_POLYGON_STIPPLE_BIT)
      {
        RegalAssert(polygonStippleStack.size());
        State::PolygonStipple::swap(polygonStippleStack.back());
        polygonStippleStack.pop_back();

        Internal("Regal::Ppa::PopAttrib GL_POLYGON_STIPPLE_BIT ",State::PolygonStipple::toString());

        // Ideally we'd only set the state that has changed
        // since the glPushAttrib() - revisit

        State::PolygonStipple::set(ctx->dispatcher.emulation);

        mask &= ~GL_POLYGON_STIPPLE_BIT;
      }

      // Pass the rest through, for now

      if (ctx->info->core || ctx->info->es1 || ctx->info->es2)
        return;

      if (mask)
        ctx->dispatcher.emulation.glPopAttrib();
    }
  }

  template <typename T> bool Get(RegalContext *ctx, GLenum pname, T *params)
  {
    RegalAssert(ctx);

    if (ctx->info->core || ctx->info->es1 || ctx->info->es2)
    {
      switch (pname)
      {
        case GL_ACCUM_RED_BITS:
        case GL_ACCUM_GREEN_BITS:
        case GL_ACCUM_BLUE_BITS:
        case GL_ACCUM_ALPHA_BITS:
          if (params) {
            params[0] = 32;
          }
          break;

        case GL_RED_BITS:
        case GL_GREEN_BITS:
        case GL_BLUE_BITS:
        case GL_ALPHA_BITS:
        case GL_INDEX_BITS:
        case GL_STENCIL_BITS:
          if (params) {
            params[0] = 8;
          }
          break;

        case GL_DEPTH_BITS:
          if (params) {
            params[0] = 24;
          }
          break;

        case GL_AUX_BUFFERS:
        case GL_LINE_STIPPLE:
        case GL_MAX_PIXEL_MAP_TABLE:
        case GL_MAX_NAME_STACK_DEPTH:
        case GL_MAX_LIST_NESTING:
        case GL_MAX_EVAL_ORDER:
        case GL_MAX_ATTRIB_STACK_DEPTH:
        case GL_MAX_CLIENT_ATTRIB_STACK_DEPTH:
          if (params)
            params[0] = 0;
          break;
        default:
          return false;
      }
      return true;
    }
    return false;
  }

  bool SetEnable(RegalContext *ctx, GLenum cap, GLboolean enabled)
  {
    switch (cap)
    {
      case GL_ALPHA_TEST:          State::Enable::alphaTest = enabled; break;
      case GL_AUTO_NORMAL:         State::Enable::autoNormal = enabled; break;
      case GL_CLIP_DISTANCE0:
      case GL_CLIP_DISTANCE1:
      case GL_CLIP_DISTANCE2:
      case GL_CLIP_DISTANCE3:
      case GL_CLIP_DISTANCE4:
      case GL_CLIP_DISTANCE5:
      case GL_CLIP_DISTANCE6:
      case GL_CLIP_DISTANCE7:      State::Enable::clipDistance[cap-GL_CLIP_DISTANCE0] = enabled; break;
      case GL_COLOR_LOGIC_OP:      State::Enable::colorLogicOp    = enabled; break;
      case GL_COLOR_MATERIAL:      State::Enable::colorMaterial   = enabled; break;
      case GL_COLOR_SUM:           State::Enable::colorSum        = enabled; break;
      case GL_COLOR_TABLE:         State::Enable::colorTable      = enabled; break;
      case GL_CONVOLUTION_1D:      State::Enable::convolution1d   = enabled; break;
      case GL_CONVOLUTION_2D:      State::Enable::convolution2d   = enabled; break;
      case GL_CULL_FACE:           State::Enable::cullFace        = enabled;
                                   State::Polygon::cullEnable     = enabled; break;
      case GL_DEPTH_CLAMP:         State::Enable::depthClamp      = enabled;
                                   State::Transform::depthClamp   = enabled; break;
      case GL_DEPTH_TEST:          State::Depth::enable           = enabled;
                                   State::Enable::depthTest       = enabled; break;
      case GL_DITHER:              State::Enable::dither          = enabled; break;
      case GL_FOG:                 State::Enable::fog             = enabled; break;
      case GL_FRAMEBUFFER_SRGB:    State::Enable::framebufferSRGB = enabled; break;
      case GL_HISTOGRAM:           State::Enable::histogram       = enabled; break;
      case GL_INDEX_LOGIC_OP:      State::Enable::indexLogicOp    = enabled; break;
      case GL_LIGHT0:
      case GL_LIGHT1:
      case GL_LIGHT2:
      case GL_LIGHT3:
      case GL_LIGHT4:
      case GL_LIGHT5:
      case GL_LIGHT6:
      case GL_LIGHT7:              State::Enable::light[cap-GL_LIGHT0] = enabled; break;
      case GL_LIGHTING:            State::Enable::lighting             = enabled; break;
      case GL_LINE_SMOOTH:         State::Enable::lineSmooth  = State::Line::smooth  = enabled; break;
      case GL_LINE_STIPPLE:        State::Enable::lineStipple = State::Line::stipple = enabled; break;
      case GL_MAP1_COLOR_4:         State::Enable::map1Color4        = State::Eval::map1dEnables[cap-GL_MAP1_COLOR_4] = enabled; break;
      case GL_MAP1_INDEX:           State::Enable::map1Index         = State::Eval::map1dEnables[cap-GL_MAP1_COLOR_4] = enabled; break;
      case GL_MAP1_NORMAL:          State::Enable::map1Normal        = State::Eval::map1dEnables[cap-GL_MAP1_COLOR_4] = enabled; break;
      case GL_MAP1_TEXTURE_COORD_1: State::Enable::map1TextureCoord1 = State::Eval::map1dEnables[cap-GL_MAP1_COLOR_4] = enabled; break;
      case GL_MAP1_TEXTURE_COORD_2: State::Enable::map1TextureCoord2 = State::Eval::map1dEnables[cap-GL_MAP1_COLOR_4] = enabled; break;
      case GL_MAP1_TEXTURE_COORD_3: State::Enable::map1TextureCoord3 = State::Eval::map1dEnables[cap-GL_MAP1_COLOR_4] = enabled; break;
      case GL_MAP1_TEXTURE_COORD_4: State::Enable::map1TextureCoord4 = State::Eval::map1dEnables[cap-GL_MAP1_COLOR_4] = enabled; break;
      case GL_MAP1_VERTEX_3:        State::Enable::map1Vertex3       = State::Eval::map1dEnables[cap-GL_MAP1_COLOR_4] = enabled; break;
      case GL_MAP1_VERTEX_4:        State::Enable::map1Vertex4       = State::Eval::map1dEnables[cap-GL_MAP1_COLOR_4] = enabled; break;
      case GL_MAP2_COLOR_4:         State::Enable::map2Color4        = State::Eval::map2dEnables[cap-GL_MAP2_COLOR_4] = enabled; break;
      case GL_MAP2_INDEX:           State::Enable::map2Index         = State::Eval::map2dEnables[cap-GL_MAP2_COLOR_4] = enabled; break;
      case GL_MAP2_NORMAL:          State::Enable::map2Normal        = State::Eval::map2dEnables[cap-GL_MAP2_COLOR_4] = enabled; break;
      case GL_MAP2_TEXTURE_COORD_1: State::Enable::map2TextureCoord1 = State::Eval::map2dEnables[cap-GL_MAP2_COLOR_4] = enabled; break;
      case GL_MAP2_TEXTURE_COORD_2: State::Enable::map2TextureCoord2 = State::Eval::map2dEnables[cap-GL_MAP2_COLOR_4] = enabled; break;
      case GL_MAP2_TEXTURE_COORD_3: State::Enable::map2TextureCoord3 = State::Eval::map2dEnables[cap-GL_MAP2_COLOR_4] = enabled; break;
      case GL_MAP2_TEXTURE_COORD_4: State::Enable::map2TextureCoord4 = State::Eval::map2dEnables[cap-GL_MAP2_COLOR_4] = enabled; break;
      case GL_MAP2_VERTEX_3:        State::Enable::map2Vertex3       = State::Eval::map2dEnables[cap-GL_MAP2_COLOR_4] = enabled; break;
      case GL_MAP2_VERTEX_4:        State::Enable::map2Vertex4       = State::Eval::map2dEnables[cap-GL_MAP2_COLOR_4] = enabled; break;
      case GL_MINMAX:              State::Enable::minmax            = enabled; break;
      case GL_MULTISAMPLE:         State::Enable::multisample       = enabled; break;
      case GL_NORMALIZE:           State::Enable::normalize         = enabled;
                                   State::Transform::normalize      = enabled; break;
      case GL_POINT_SMOOTH:        State::Enable::pointSmooth = State::Point::smooth = enabled; break;
      case GL_POINT_SPRITE:        State::Enable::pointSprite = State::Point::sprite = enabled; break;
      case GL_POLYGON_OFFSET_FILL: State::Enable::polygonOffsetFill = enabled;
                                   State::Polygon::offsetFill       = enabled; break;
      case GL_POLYGON_OFFSET_LINE: State::Enable::polygonOffsetLine = enabled;
                                   State::Polygon::offsetLine       = enabled; break;
      case GL_POLYGON_OFFSET_POINT: State::Enable::polygonOffsetPoint = enabled;
                                   State::Polygon::offsetPoint       = enabled; break;
      case GL_POLYGON_SMOOTH:      State::Enable::polygonSmooth      = enabled;
                                   State::Polygon::smoothEnable      = enabled; break;
      case GL_POLYGON_STIPPLE:     State::Polygon::stippleEnable     = enabled;
                                   State::Enable::polygonStipple     = enabled; break;
      case GL_POST_COLOR_MATRIX_COLOR_TABLE: State::Enable::postColorMatrixColorTable = enabled; break;
      case GL_POST_CONVOLUTION_COLOR_TABLE: State::Enable::postConvolutionColorTable  = enabled; break;
      case GL_PROGRAM_POINT_SIZE:  State::Enable::programPointSize   = enabled; break;
      case GL_RESCALE_NORMAL:      State::Enable::rescaleNormal      = enabled;
                                   State::Transform::rescaleNormal   = enabled; break;
      case GL_SAMPLE_ALPHA_TO_COVERAGE: State::Enable::sampleAlphaToCoverage = enabled; break;
      case GL_SAMPLE_ALPHA_TO_ONE: State::Enable::sampleAlphaToOne   = enabled; break;
      case GL_SAMPLE_COVERAGE:     State::Enable::sampleCoverage     = enabled; break;
      case GL_SAMPLE_SHADING:      State::Enable::sampleShading      = enabled; break;
      case GL_SCISSOR_TEST:
          {
            for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
              State::Enable::scissorTest[ii] = State::Scissor::scissorTest[ii] = enabled;
          }
          break;
      case GL_SEPARABLE_2D:      State::Enable::separable2d                       = enabled; break;
      case GL_STENCIL_TEST:      State::Enable::stencilTest                       = enabled;
                                 State::Stencil::enable                           = enabled; break;
      case GL_TEXTURE_1D:        State::Enable::texture1d[activeTextureUnit]      = enabled; break;
      case GL_TEXTURE_2D:        State::Enable::texture2d[activeTextureUnit]      = enabled; break;
      case GL_TEXTURE_3D:        State::Enable::texture3d[activeTextureUnit]      = enabled; break;
      case GL_TEXTURE_CUBE_MAP:  State::Enable::textureCubeMap[activeTextureUnit] = enabled; break;
      case GL_TEXTURE_GEN_S:     State::Enable::textureGenS[activeTextureUnit]    = enabled; break;
      case GL_TEXTURE_GEN_T:     State::Enable::textureGenT[activeTextureUnit]    = enabled; break;
      case GL_TEXTURE_GEN_R:     State::Enable::textureGenR[activeTextureUnit]    = enabled; break;
      case GL_TEXTURE_GEN_Q:     State::Enable::textureGenQ[activeTextureUnit]    = enabled; break;
      case GL_VERTEX_PROGRAM_TWO_SIDE: State::Enable::vertexProgramTwoSide        = enabled; break;

      default: break;
    }

    if (ctx->info->core || ctx->info->es1 || ctx->info->es2)
    {
      switch( cap )
      {
        case GL_POINT_SMOOTH:
        case GL_LINE_STIPPLE:
          return true;
        default:
          break;
      }
    }

    return false;
  }

  bool SetEnablei(RegalContext *ctx, GLenum cap, GLuint index, GLboolean enabled)
  {
    UNUSED_PARAMETER(ctx);
    switch (cap)
    {
      case GL_BLEND:
        if (index < REGAL_MAX_DRAW_BUFFERS)
          State::Enable::blend[index] = enabled;
        break;
      case GL_SCISSOR_TEST:
        if (index < REGAL_MAX_VIEWPORTS)
        {
          State::Enable::scissorTest[index] = State::Scissor::scissorTest[index] = enabled;
        }
        break;

      default: break;
    }

    return false;
  }

  bool Enable(RegalContext *ctx, GLenum cap)
  {
    Internal("Regal::Ppa::Enable ",Token::toString(cap));
    return SetEnable(ctx, cap, GL_TRUE);
  }

  bool Disable(RegalContext * ctx, GLenum cap)
  {
    Internal("Regal::Ppa::Disable ",Token::toString(cap));
    return SetEnable(ctx, cap, GL_FALSE);
  }

  bool Enablei(RegalContext *ctx, GLenum cap, GLuint index)
  {
    Internal("Regal::Ppa::Enablei ",Token::toString(cap),index);
    return SetEnablei(ctx, cap, index, GL_TRUE);
  }

  bool Disablei(RegalContext * ctx, GLenum cap, GLuint index)
  {
    Internal("Regal::Ppa::Disablei ",Token::toString(cap),index);
    return SetEnablei(ctx, cap, index, GL_FALSE);
  }

  inline void glClampColor( GLenum target, GLenum clamp )
  {
    switch (target)
    {
      case GL_CLAMP_FRAGMENT_COLOR: State::Enable::clampFragmentColor = clamp; break;
      case GL_CLAMP_READ_COLOR:     State::Enable::clampReadColor     = clamp; break;
      case GL_CLAMP_VERTEX_COLOR:   State::Enable::clampVertexColor   = clamp; break;
      default: break;
    }
  }

  void glActiveTexture( GLenum tex )
  {
    GLuint unit = tex - GL_TEXTURE0;
    if (unit < REGAL_EMU_MAX_TEXTURE_UNITS)
      activeTextureUnit = unit;
    else
    {
      Warning( "Active texture out of range: ", Token::GLtextureToString(tex), " > ", Token::GLtextureToString(GL_TEXTURE0 + REGAL_EMU_MAX_TEXTURE_UNITS - 1));
      return;
    }
  }

  template <typename T> void glTexEnv(GLenum target, GLenum pname, T param)
  {
    if ((target == GL_POINT_SPRITE) && (pname == GL_COORD_REPLACE))
      glMultiTexEnv(static_cast<GLenum>(GL_TEXTURE0+activeTextureUnit),target,pname,param);
  }

  template <typename T> void glTexEnvv(GLenum target, GLenum pname, const T *params)
  {
    if ((target == GL_POINT_SPRITE) && (pname == GL_COORD_REPLACE))
      glMultiTexEnvv(static_cast<GLenum>(GL_TEXTURE0+activeTextureUnit),target,pname,params);
  }

  std::vector<GLbitfield>            maskStack;
  std::vector<State::Depth>          depthStack;
  std::vector<State::Stencil>        stencilStack;
  std::vector<State::Polygon>        polygonStack;
  std::vector<State::Transform>      transformStack;
  std::vector<State::Hint>           hintStack;
  std::vector<State::Enable>         enableStack;
  std::vector<State::List>           listStack;
  std::vector<State::AccumBuffer>    accumBufferStack;
  std::vector<State::Scissor>        scissorStack;
  std::vector<State::Viewport>       viewportStack;
  std::vector<State::Line>           lineStack;
  std::vector<State::Multisample>    multisampleStack;
  std::vector<State::Eval>           evalStack;
  std::vector<State::Fog>            fogStack;
  std::vector<State::Point>          pointStack;
  std::vector<State::PolygonStipple> polygonStippleStack;

  GLuint activeTextureUnit;
};

}

REGAL_NAMESPACE_END

#endif // REGAL_EMULATION

#endif // ! __REGAL_PPA_H__
