/*
  Copyright (c) 2011-2012 NVIDIA Corporation
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

#ifndef __REGAL_STATE_H__
#define __REGAL_STATE_H__

#include "RegalUtil.h"

REGAL_GLOBAL_BEGIN

#include <cstring>    // For memset, memcpy

#include <string>
#include <algorithm>  // For std::swap

#include <boost/print/print_string.hpp>
#include <boost/print/string_list.hpp>

#include "RegalEmu.h"
#include "RegalIff.h"
#include "RegalToken.h"
#include "RegalDispatch.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

//
// OpenGL State Tracking
//
// Motivating requirements:
//
//  - Emulation of glPushAttrib and glPopAttrib for OpenGL ES and
//    core OpenGL that lack the functionality.
//
//  - OpenGL state capture, browsing, diff and serialization.
//
// See also:
//
//  - Gallium3D
//    http://wiki.freedesktop.org/wiki/Software/gallium
//    http://dri.freedesktop.org/doxygen/gallium/p__state_8h-source.html
//
//  - Tracking Graphics State For Networked Rendering
//    Ian Buck, Greg Humphreys and Pat Hanrahan.
//    Stanford University
//    Proceedings of the 2000 Eurographics/SIGGRAPH Workshop on Graphics Hardware
//    http://graphics.stanford.edu/papers/state_tracking/
//
//  - Chromium: A Stream Processing Framework for Interactive Rendering on Clusters
//    Greg Humphreys, Mike Houston, Ren Ng
//    Stanford University
//    SIGGRAPH 2002
//    http://graphics.stanford.edu/papers/cr/
//

#ifndef REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES
#define REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES 8
#endif

#ifndef REGAL_MAX_VIEWPORTS
#define REGAL_MAX_VIEWPORTS 16
#endif

#ifndef REGAL_MAX_DRAW_BUFFERS
#define REGAL_MAX_DRAW_BUFFERS 8
#endif

namespace State {

  using   ::boost::print::hex;
  using   ::boost::print::print_string;
  typedef ::boost::print::string_list<std::string> string_list;

  inline static void setEnable(DispatchTable &dt, const GLenum cap, const GLboolean enable)
  {
    if (enable)
      dt.call(&dt.glEnable)(cap);
    else
      dt.call(&dt.glDisable)(cap);
  }

  inline static void setEnableIndexedEXT(DispatchTable &dt, const GLenum cap, const GLuint index, const GLboolean enable)
  {
    if (enable)
      dt.call(&dt.glEnableIndexedEXT)(cap,index);
    else
      dt.call(&dt.glDisableIndexedEXT)(cap,index);
  }

  inline static void setEnablei(DispatchTable &dt, const GLenum cap, const GLuint index, const GLboolean enable)
  {
    if (enable)
      dt.call(&dt.glEnablei)(cap,index);
    else
      dt.call(&dt.glDisablei)(cap,index);
  }

  inline static void enableToString(string_list& tmp, const GLboolean b, const char* bEnum, const char *delim = "\n")
  {
      tmp << print_string(b ? "glEnable(" : "glDisable(",bEnum,");",delim);
  }

  inline static void enableiToString(string_list& tmp, const GLboolean b, const char* bEnum, const GLuint index, const char *delim = "\n")
  {
      tmp << print_string(b ? "glEnablei(" : "glDisablei(",bEnum,",",index,");",delim);
  }

  //
  // glPushAttrib(GL_ENABLE_BIT)
  //

  struct Enable
  {
    GLboolean   alphaTest;                 // GL_ALPHA_TEST
    GLboolean   autoNormal;                // GL_AUTO_NORMAL
    GLboolean   blend[REGAL_MAX_DRAW_BUFFERS]; // GL_BLEND
    GLboolean   clipDistance[REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES]; // GL_CLIP_DISTANCEi
    GLenum      clampFragmentColor;        // GL_CLAMP_FRAGMENT_COLOR
    GLenum      clampReadColor;            // GL_CLAMP_READ_COLOR
    GLenum      clampVertexColor;          // GL_CLAMP_VERTEX_COLOR
    GLboolean   colorLogicOp;              // GL_COLOR_LOGIC_OP
    GLboolean   colorMaterial;             // GL_COLOR_MATERIAL
    GLboolean   colorSum;                  // GL_COLOR_SUM
    GLboolean   colorTable;                // GL_COLOR_TABLE
    GLboolean   convolution1d;             // GL_CONVOLUTION_1D
    GLboolean   convolution2d;             // GL_CONVOLUTION_2D
    GLboolean   cullFace;                  // GL_CULL_FACE
    GLboolean   depthClamp;                // GL_DEPTH_CLAMP
    GLboolean   depthTest;                 // GL_DEPTH_TEST
    GLboolean   dither;                    // GL_DITHER
    GLboolean   fog;                       // GL_FOG
    GLboolean   framebufferSRGB;           // GL_FRAMEBUFFER_SRGB
    GLboolean   histogram;                 // GL_HISTOGRAM
    GLboolean   indexLogicOp;              // GL_INDEX_LOGIC_OP
    GLboolean   light[REGAL_FIXED_FUNCTION_MAX_LIGHTS]; // GL_LIGHTi
    GLboolean   lighting;                  // GL_LIGHTING
    GLboolean   lineSmooth;                // GL_LINE_SMOOTH
    GLboolean   lineStipple;               // GL_LINE_STIPPLE
    GLboolean   map1Color4;                // GL_MAP1_COLOR_4
    GLboolean   map1Index;                 // GL_MAP1_INDEX
    GLboolean   map1Normal;                // GL_MAP1_NORMAL
    GLboolean   map1TextureCoord1;         // GL_MAP1_TEXTURE_COORD_1
    GLboolean   map1TextureCoord2;         // GL_MAP1_TEXTURE_COORD_2
    GLboolean   map1TextureCoord3;         // GL_MAP1_TEXTURE_COORD_3
    GLboolean   map1TextureCoord4;         // GL_MAP1_TEXTURE_COORD_4
    GLboolean   map1Vertex3;               // GL_MAP1_VERTEX_3
    GLboolean   map1Vertex4;               // GL_MAP1_VERTEX_4
    GLboolean   map2Color4;                // GL_MAP2_COLOR_4
    GLboolean   map2Index;                 // GL_MAP2_INDEX
    GLboolean   map2Normal;                // GL_MAP2_NORMAL
    GLboolean   map2TextureCoord1;         // GL_MAP2_TEXTURE_COORD_1
    GLboolean   map2TextureCoord2;         // GL_MAP2_TEXTURE_COORD_2
    GLboolean   map2TextureCoord3;         // GL_MAP2_TEXTURE_COORD_3
    GLboolean   map2TextureCoord4;         // GL_MAP2_TEXTURE_COORD_4
    GLboolean   map2Vertex3;               // GL_MAP2_VERTEX_3
    GLboolean   map2Vertex4;               // GL_MAP2_VERTEX_4
    GLboolean   minmax;                    // GL_MINMAX
    GLboolean   multisample;               // GL_MULTISAMPLE
    GLboolean   normalize;                 // GL_NORMALIZE
    GLboolean   pointSmooth;               // GL_POINT_SMOOTH
    GLboolean   pointSprite;               // GL_POINT_SPRITE
    GLboolean   polygonOffsetFill;         // GL_POLYGON_OFFSET_FILL
    GLboolean   polygonOffsetLine;         // GL_POLYGON_OFFSET_LINE
    GLboolean   polygonOffsetPoint;        // GL_POLYGON_OFFSET_POINT
    GLboolean   polygonSmooth;             // GL_POLYGON_SMOOTH
    GLboolean   polygonStipple;            // GL_POLYGON_STIPPLE
    GLboolean   postColorMatrixColorTable; // GL_POST_COLOR_MATRIX_COLOR_TABLE
    GLboolean   postConvolutionColorTable; // GL_POST_CONVOLUTION_COLOR_TABLE
    GLboolean   programPointSize;          // GL_PROGRAM_POINT_SIZE
    GLboolean   rescaleNormal;             // GL_RESCALE_NORMAL
    GLboolean   sampleAlphaToCoverage;     // GL_SAMPLE_ALPHA_TO_COVERAGE
    GLboolean   sampleAlphaToOne;          // GL_SAMPLE_ALPHA_TO_ONE
    GLboolean   sampleCoverage;            // GL_SAMPLE_COVERAGE
    GLboolean   sampleShading;             // GL_SAMPLE_SHADING
    GLboolean   separable2d;               // GL_SEPARABLE_2D
    GLboolean   scissorTest[REGAL_MAX_VIEWPORTS]; // GL_SCISSOR_TEST
    GLboolean   stencilTest;               // GL_STENCIL_TEST
    GLboolean   texture1d[REGAL_EMU_MAX_TEXTURE_UNITS];      // GL_TEXTURE_1D
    GLboolean   texture2d[REGAL_EMU_MAX_TEXTURE_UNITS];      // GL_TEXTURE_2D
    GLboolean   texture3d[REGAL_EMU_MAX_TEXTURE_UNITS];      // GL_TEXTURE_3D
    GLboolean   textureCubeMap[REGAL_EMU_MAX_TEXTURE_UNITS]; // GL_TEXTURE_CUBE_MAP
    GLboolean   textureGenS[REGAL_EMU_MAX_TEXTURE_UNITS];    // GL_TEXTURE_GEN_S
    GLboolean   textureGenT[REGAL_EMU_MAX_TEXTURE_UNITS];    // GL_TEXTURE_GEN_T
    GLboolean   textureGenR[REGAL_EMU_MAX_TEXTURE_UNITS];    // GL_TEXTURE_GEN_R
    GLboolean   textureGenQ[REGAL_EMU_MAX_TEXTURE_UNITS];    // GL_TEXTURE_GEN_Q
    GLboolean   vertexProgramTwoSide;      // GL_VERTEX_PROGRAM_TWO_SIDE

    inline Enable()
    : alphaTest(GL_FALSE)
    , autoNormal(GL_FALSE)
    , clampFragmentColor(GL_FIXED_ONLY)
    , clampReadColor(GL_FIXED_ONLY)
    , clampVertexColor(GL_TRUE)
    , colorLogicOp(GL_FALSE)
    , colorMaterial(GL_FALSE)
    , colorSum(GL_FALSE)
    , colorTable(GL_FALSE)
    , convolution1d(GL_FALSE)
    , convolution2d(GL_FALSE)
    , cullFace(GL_FALSE)
    , depthClamp(GL_FALSE)
    , depthTest(GL_FALSE)
    , dither(GL_TRUE)
    , fog(GL_FALSE)
    , framebufferSRGB(GL_FALSE)
    , histogram(GL_FALSE)
    , indexLogicOp(GL_FALSE)
    , lighting(GL_FALSE)
    , lineSmooth(GL_FALSE)
    , lineStipple(GL_FALSE)
    , map1Color4(GL_FALSE)
    , map1Index(GL_FALSE)
    , map1Normal(GL_FALSE)
    , map1TextureCoord1(GL_FALSE)
    , map1TextureCoord2(GL_FALSE)
    , map1TextureCoord3(GL_FALSE)
    , map1TextureCoord4(GL_FALSE)
    , map1Vertex3(GL_FALSE)
    , map1Vertex4(GL_FALSE)
    , map2Color4(GL_FALSE)
    , map2Index(GL_FALSE)
    , map2Normal(GL_FALSE)
    , map2TextureCoord1(GL_FALSE)
    , map2TextureCoord2(GL_FALSE)
    , map2TextureCoord3(GL_FALSE)
    , map2TextureCoord4(GL_FALSE)
    , map2Vertex3(GL_FALSE)
    , map2Vertex4(GL_FALSE)
    , minmax(GL_FALSE)
    , multisample(GL_TRUE)
    , normalize(GL_FALSE)
    , pointSmooth(GL_FALSE)
    , pointSprite(GL_FALSE)
    , polygonOffsetFill(GL_FALSE)
    , polygonOffsetLine(GL_FALSE)
    , polygonOffsetPoint(GL_FALSE)
    , polygonSmooth(GL_FALSE)
    , polygonStipple(GL_FALSE)
    , postColorMatrixColorTable(GL_FALSE)
    , postConvolutionColorTable(GL_FALSE)
    , programPointSize(GL_FALSE)
    , rescaleNormal(GL_FALSE)
    , sampleAlphaToCoverage(GL_FALSE)
    , sampleAlphaToOne(GL_FALSE)
    , sampleCoverage(GL_FALSE)
    , sampleShading(GL_FALSE)
    , separable2d(GL_FALSE)
    , stencilTest(GL_FALSE)
    , vertexProgramTwoSide(GL_FALSE)
    {
      std::memset(blend,GL_FALSE,sizeof(blend));
      std::memset(clipDistance,GL_FALSE,sizeof(clipDistance));
      std::memset(light,GL_FALSE,sizeof(light));
      std::memset(scissorTest,GL_FALSE,sizeof(scissorTest));
      std::memset(texture1d,GL_FALSE,sizeof(texture1d));
      std::memset(texture2d,GL_FALSE,sizeof(texture2d));
      std::memset(texture3d,GL_FALSE,sizeof(texture3d));
      std::memset(textureCubeMap,GL_FALSE,sizeof(textureCubeMap));
      std::memset(textureGenQ,GL_FALSE,sizeof(textureGenQ));
      std::memset(textureGenR,GL_FALSE,sizeof(textureGenR));
      std::memset(textureGenS,GL_FALSE,sizeof(textureGenS));
      std::memset(textureGenT,GL_FALSE,sizeof(textureGenT));
    }

    inline Enable(const Enable &other)
    {
      if (this!=&other)
        std::memcpy(this,&other,sizeof(Enable));
    }

    inline Enable &swap(Enable &other)
    {
      std::swap(alphaTest,other.alphaTest);
      std::swap(autoNormal,other.autoNormal);
      std::swap_ranges(blend,blend+REGAL_MAX_DRAW_BUFFERS,other.blend);
      std::swap(clampFragmentColor,other.clampFragmentColor);
      std::swap(clampReadColor,other.clampReadColor);
      std::swap(clampVertexColor,other.clampVertexColor);
      std::swap_ranges(clipDistance,clipDistance+REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES,other.clipDistance);
      std::swap(colorLogicOp,other.colorLogicOp);
      std::swap(colorMaterial,other.colorMaterial);
      std::swap(colorSum,other.colorSum);
      std::swap(colorTable,other.colorTable);
      std::swap(convolution1d,other.convolution1d);
      std::swap(convolution2d,other.convolution2d);
      std::swap(cullFace,other.cullFace);
      std::swap(depthClamp,other.depthClamp);
      std::swap(depthTest,other.depthTest);
      std::swap(dither,other.dither);
      std::swap(fog,other.fog);
      std::swap(framebufferSRGB,other.framebufferSRGB);
      std::swap(histogram,other.histogram);
      std::swap(indexLogicOp,other.indexLogicOp);
      std::swap_ranges(light,light+REGAL_FIXED_FUNCTION_MAX_LIGHTS,other.light);
      std::swap(lighting,other.lighting);
      std::swap(lineSmooth,other.lineSmooth);
      std::swap(lineStipple,other.lineStipple);
      std::swap(map1Color4,other.map1Color4);
      std::swap(map1Index,other.map1Index);
      std::swap(map1Normal,other.map1Normal);
      std::swap(map1TextureCoord1,other.map1TextureCoord1);
      std::swap(map1TextureCoord2,other.map1TextureCoord2);
      std::swap(map1TextureCoord3,other.map1TextureCoord3);
      std::swap(map1TextureCoord4,other.map1TextureCoord4);
      std::swap(map1Vertex3,other.map1Vertex3);
      std::swap(map1Vertex4,other.map1Vertex4);
      std::swap(map2Color4,other.map2Color4);
      std::swap(map2Index,other.map2Index);
      std::swap(map2Normal,other.map2Normal);
      std::swap(map2TextureCoord1,other.map2TextureCoord1);
      std::swap(map2TextureCoord2,other.map2TextureCoord2);
      std::swap(map2TextureCoord3,other.map2TextureCoord3);
      std::swap(map2TextureCoord4,other.map2TextureCoord4);
      std::swap(map2Vertex3,other.map2Vertex3);
      std::swap(map2Vertex4,other.map2Vertex4);
      std::swap(minmax,other.minmax);
      std::swap(multisample,other.multisample);
      std::swap(normalize,other.normalize);
      std::swap(pointSmooth,other.pointSmooth);
      std::swap(pointSprite,other.pointSprite);
      std::swap(polygonOffsetFill,other.polygonOffsetFill);
      std::swap(polygonOffsetLine,other.polygonOffsetLine);
      std::swap(polygonOffsetPoint,other.polygonOffsetPoint);
      std::swap(polygonSmooth,other.polygonSmooth);
      std::swap(polygonStipple,other.polygonStipple);
      std::swap(postColorMatrixColorTable,other.postColorMatrixColorTable);
      std::swap(postConvolutionColorTable,other.postConvolutionColorTable);
      std::swap(programPointSize,other.programPointSize);
      std::swap(rescaleNormal,other.rescaleNormal);
      std::swap(sampleAlphaToCoverage,other.sampleAlphaToCoverage);
      std::swap(sampleAlphaToOne,other.sampleAlphaToOne);
      std::swap(sampleCoverage,other.sampleCoverage);
      std::swap(sampleShading,other.sampleShading);
      std::swap_ranges(scissorTest,scissorTest+REGAL_MAX_VIEWPORTS,other.scissorTest);
      std::swap(separable2d,other.separable2d);
      std::swap(stencilTest,other.stencilTest);
      std::swap_ranges(texture1d,texture1d+REGAL_EMU_MAX_TEXTURE_UNITS,other.texture1d);
      std::swap_ranges(texture2d,texture2d+REGAL_EMU_MAX_TEXTURE_UNITS,other.texture2d);
      std::swap_ranges(texture3d,texture3d+REGAL_EMU_MAX_TEXTURE_UNITS,other.texture3d);
      std::swap_ranges(textureCubeMap,textureCubeMap+REGAL_EMU_MAX_TEXTURE_UNITS,other.textureCubeMap);
      std::swap_ranges(textureGenQ,textureGenQ+REGAL_EMU_MAX_TEXTURE_UNITS,other.textureGenQ);
      std::swap_ranges(textureGenR,textureGenR+REGAL_EMU_MAX_TEXTURE_UNITS,other.textureGenR);
      std::swap_ranges(textureGenS,textureGenS+REGAL_EMU_MAX_TEXTURE_UNITS,other.textureGenS);
      std::swap_ranges(textureGenT,textureGenT+REGAL_EMU_MAX_TEXTURE_UNITS,other.textureGenT);
      std::swap(vertexProgramTwoSide,other.vertexProgramTwoSide);
      return *this;
    }

    inline Enable &get(DispatchTable &dt)
    {
      alphaTest = dt.call(&dt.glIsEnabled)(GL_ALPHA_TEST);
      autoNormal = dt.call(&dt.glIsEnabled)(GL_AUTO_NORMAL);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        blend[ii] = dt.call(&dt.glIsEnabledi)(GL_BLEND, ii);
      dt.call(&dt.glGetIntegerv)(GL_CLAMP_FRAGMENT_COLOR,reinterpret_cast<GLint*>(&clampFragmentColor));
      dt.call(&dt.glGetIntegerv)(GL_CLAMP_READ_COLOR,reinterpret_cast<GLint*>(&clampReadColor));
      dt.call(&dt.glGetIntegerv)(GL_CLAMP_VERTEX_COLOR,reinterpret_cast<GLint*>(&clampVertexColor));
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES; ii++)
        clipDistance[ii] = dt.call(&dt.glIsEnabled)(GL_CLIP_DISTANCE0+ii);
      colorLogicOp  = dt.call(&dt.glIsEnabled)(GL_COLOR_LOGIC_OP);
      colorMaterial = dt.call(&dt.glIsEnabled)(GL_COLOR_MATERIAL);
      colorMaterial = dt.call(&dt.glIsEnabled)(GL_COLOR_MATERIAL);
      colorSum = dt.call(&dt.glIsEnabled)(GL_COLOR_SUM);
      colorTable = dt.call(&dt.glIsEnabled)(GL_COLOR_TABLE);
      convolution1d = dt.call(&dt.glIsEnabled)(GL_CONVOLUTION_1D);
      convolution2d = dt.call(&dt.glIsEnabled)(GL_CONVOLUTION_2D);
      cullFace = dt.call(&dt.glIsEnabled)(GL_CULL_FACE);
      depthClamp = dt.call(&dt.glIsEnabled)(GL_DEPTH_CLAMP);
      depthTest = dt.call(&dt.glIsEnabled)(GL_DEPTH_TEST);
      dither = dt.call(&dt.glIsEnabled)(GL_DITHER);
      fog = dt.call(&dt.glIsEnabled)(GL_FOG);
      framebufferSRGB = dt.call(&dt.glIsEnabled)(GL_FRAMEBUFFER_SRGB);
      histogram = dt.call(&dt.glIsEnabled)(GL_HISTOGRAM);
      indexLogicOp = dt.call(&dt.glIsEnabled)(GL_INDEX_LOGIC_OP);
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_LIGHTS; ii++)
        light[ii] = dt.call(&dt.glIsEnabled)(GL_LIGHT0+ii);
      lighting = dt.call(&dt.glIsEnabled)(GL_LIGHTING);
      lineSmooth = dt.call(&dt.glIsEnabled)(GL_LINE_SMOOTH);
      lineStipple = dt.call(&dt.glIsEnabled)(GL_LINE_STIPPLE);
      map1Color4 = dt.call(&dt.glIsEnabled)(GL_MAP1_COLOR_4);
      map1Index = dt.call(&dt.glIsEnabled)(GL_MAP1_INDEX);
      map1Normal = dt.call(&dt.glIsEnabled)(GL_MAP1_NORMAL);
      map1TextureCoord1 = dt.call(&dt.glIsEnabled)(GL_MAP1_TEXTURE_COORD_1);
      map1TextureCoord2 = dt.call(&dt.glIsEnabled)(GL_MAP1_TEXTURE_COORD_2);
      map1TextureCoord3 = dt.call(&dt.glIsEnabled)(GL_MAP1_TEXTURE_COORD_3);
      map1TextureCoord4 = dt.call(&dt.glIsEnabled)(GL_MAP1_TEXTURE_COORD_4);
      map1Vertex3 = dt.call(&dt.glIsEnabled)(GL_MAP1_VERTEX_3);
      map1Vertex4 = dt.call(&dt.glIsEnabled)(GL_MAP1_VERTEX_4);
      map2Color4 = dt.call(&dt.glIsEnabled)(GL_MAP2_COLOR_4);
      map2Index = dt.call(&dt.glIsEnabled)(GL_MAP2_INDEX);
      map2Normal = dt.call(&dt.glIsEnabled)(GL_MAP2_NORMAL);
      map2TextureCoord1 = dt.call(&dt.glIsEnabled)(GL_MAP2_TEXTURE_COORD_1);
      map2TextureCoord2 = dt.call(&dt.glIsEnabled)(GL_MAP2_TEXTURE_COORD_2);
      map2TextureCoord3 = dt.call(&dt.glIsEnabled)(GL_MAP2_TEXTURE_COORD_3);
      map2TextureCoord4 = dt.call(&dt.glIsEnabled)(GL_MAP2_TEXTURE_COORD_4);
      map2Vertex3 = dt.call(&dt.glIsEnabled)(GL_MAP2_VERTEX_3);
      map2Vertex4 = dt.call(&dt.glIsEnabled)(GL_MAP2_VERTEX_4);
      minmax = dt.call(&dt.glIsEnabled)(GL_MINMAX);
      multisample = dt.call(&dt.glIsEnabled)(GL_MULTISAMPLE);
      normalize = dt.call(&dt.glIsEnabled)(GL_NORMALIZE);
      pointSmooth = dt.call(&dt.glIsEnabled)(GL_POINT_SMOOTH);
      pointSprite = dt.call(&dt.glIsEnabled)(GL_POINT_SPRITE);
      polygonOffsetFill = dt.call(&dt.glIsEnabled)(GL_POLYGON_OFFSET_FILL);
      polygonOffsetLine = dt.call(&dt.glIsEnabled)(GL_POLYGON_OFFSET_LINE);
      polygonOffsetPoint = dt.call(&dt.glIsEnabled)(GL_POLYGON_OFFSET_POINT);
      polygonSmooth = dt.call(&dt.glIsEnabled)(GL_POLYGON_SMOOTH);
      polygonStipple = dt.call(&dt.glIsEnabled)(GL_POLYGON_STIPPLE);
      postColorMatrixColorTable = dt.call(&dt.glIsEnabled)(GL_POST_COLOR_MATRIX_COLOR_TABLE);
      postConvolutionColorTable = dt.call(&dt.glIsEnabled)(GL_POST_CONVOLUTION_COLOR_TABLE);
      programPointSize = dt.call(&dt.glIsEnabled)(GL_PROGRAM_POINT_SIZE);
      rescaleNormal = dt.call(&dt.glIsEnabled)(GL_RESCALE_NORMAL);
      sampleAlphaToCoverage = dt.call(&dt.glIsEnabled)(GL_SAMPLE_ALPHA_TO_COVERAGE);
      sampleAlphaToOne = dt.call(&dt.glIsEnabled)(GL_SAMPLE_ALPHA_TO_ONE);
      sampleCoverage = dt.call(&dt.glIsEnabled)(GL_SAMPLE_COVERAGE);
      sampleShading = dt.call(&dt.glIsEnabled)(GL_SAMPLE_SHADING);
      separable2d = dt.call(&dt.glIsEnabled)(GL_SEPARABLE_2D);
      stencilTest = dt.call(&dt.glIsEnabled)(GL_STENCIL_TEST);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        scissorTest[ii] = dt.call(&dt.glIsEnabledi)(GL_SCISSOR_TEST,ii);
      {
        for (int ii=0; ii<REGAL_EMU_MAX_TEXTURE_UNITS; ii++)
        {
          texture1d[ii]      = dt.call(&dt.glIsEnabledIndexedEXT)(GL_TEXTURE_1D,ii);
          texture2d[ii]      = dt.call(&dt.glIsEnabledIndexedEXT)(GL_TEXTURE_2D,ii);
          texture3d[ii]      = dt.call(&dt.glIsEnabledIndexedEXT)(GL_TEXTURE_3D,ii);
          textureCubeMap[ii] = dt.call(&dt.glIsEnabledIndexedEXT)(GL_TEXTURE_CUBE_MAP,ii);
          textureGenS[ii]    = dt.call(&dt.glIsEnabledIndexedEXT)(GL_TEXTURE_GEN_S,ii);
          textureGenT[ii]    = dt.call(&dt.glIsEnabledIndexedEXT)(GL_TEXTURE_GEN_T,ii);
          textureGenR[ii]    = dt.call(&dt.glIsEnabledIndexedEXT)(GL_TEXTURE_GEN_R,ii);
          textureGenQ[ii]    = dt.call(&dt.glIsEnabledIndexedEXT)(GL_TEXTURE_GEN_Q,ii);
        }
      }

      return *this;
    }
    inline const Enable &set(DispatchTable &dt) const
    {
      setEnable(dt,GL_ALPHA_TEST,alphaTest);
      setEnable(dt,GL_AUTO_NORMAL,autoNormal);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        setEnablei(dt,GL_BLEND,ii,blend[ii]);
      setEnable(dt,GL_COLOR_LOGIC_OP,colorLogicOp);
      setEnable(dt,GL_COLOR_MATERIAL,colorMaterial);
      dt.call(&dt.glClampColor)(GL_CLAMP_FRAGMENT_COLOR,clampFragmentColor);
      dt.call(&dt.glClampColor)(GL_CLAMP_READ_COLOR,clampReadColor);
      dt.call(&dt.glClampColor)(GL_CLAMP_VERTEX_COLOR,clampVertexColor);
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES; ii++)
        setEnable(dt,GL_CLIP_DISTANCE0+ii,clipDistance[ii]);
      setEnable(dt,GL_COLOR_MATERIAL,colorMaterial);
      setEnable(dt,GL_COLOR_SUM,colorSum);
      setEnable(dt,GL_COLOR_TABLE,colorTable);
      setEnable(dt,GL_CONVOLUTION_1D,convolution1d);
      setEnable(dt,GL_CONVOLUTION_2D,convolution2d);
      setEnable(dt,GL_CULL_FACE,cullFace);
      setEnable(dt,GL_DEPTH_CLAMP,depthClamp);
      setEnable(dt,GL_DEPTH_TEST,depthTest);
      setEnable(dt,GL_DITHER,dither);
      setEnable(dt,GL_FOG,fog);
      setEnable(dt,GL_FRAMEBUFFER_SRGB,framebufferSRGB);
      setEnable(dt,GL_HISTOGRAM,histogram);
      setEnable(dt,GL_INDEX_LOGIC_OP,indexLogicOp);
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_LIGHTS; ii++)
        setEnable(dt,GL_LIGHT0+ii,light[ii]);
      setEnable(dt,GL_LIGHTING,lighting);
      setEnable(dt,GL_LINE_SMOOTH,lineSmooth);
      setEnable(dt,GL_LINE_STIPPLE,lineStipple);
      setEnable(dt,GL_MAP1_COLOR_4,map1Color4);
      setEnable(dt,GL_MAP1_INDEX,map1Index);
      setEnable(dt,GL_MAP1_NORMAL,map1Normal);
      setEnable(dt,GL_MAP1_TEXTURE_COORD_1,map1TextureCoord1);
      setEnable(dt,GL_MAP1_TEXTURE_COORD_2,map1TextureCoord2);
      setEnable(dt,GL_MAP1_TEXTURE_COORD_3,map1TextureCoord3);
      setEnable(dt,GL_MAP1_TEXTURE_COORD_4,map1TextureCoord4);
      setEnable(dt,GL_MAP1_VERTEX_3,map1Vertex3);
      setEnable(dt,GL_MAP1_VERTEX_4,map1Vertex4);
      setEnable(dt,GL_MAP2_COLOR_4,map2Color4);
      setEnable(dt,GL_MAP2_INDEX,map2Index);
      setEnable(dt,GL_MAP2_NORMAL,map2Normal);
      setEnable(dt,GL_MAP2_TEXTURE_COORD_1,map2TextureCoord1);
      setEnable(dt,GL_MAP2_TEXTURE_COORD_2,map2TextureCoord2);
      setEnable(dt,GL_MAP2_TEXTURE_COORD_3,map2TextureCoord3);
      setEnable(dt,GL_MAP2_TEXTURE_COORD_4,map2TextureCoord4);
      setEnable(dt,GL_MAP2_VERTEX_3,map2Vertex3);
      setEnable(dt,GL_MAP2_VERTEX_4,map2Vertex4);
      setEnable(dt,GL_MINMAX,minmax);
      setEnable(dt,GL_MULTISAMPLE,multisample);
      setEnable(dt,GL_NORMALIZE,normalize);
      setEnable(dt,GL_POINT_SMOOTH,pointSmooth);
      setEnable(dt,GL_POINT_SPRITE,pointSprite);
      setEnable(dt,GL_POLYGON_OFFSET_FILL,polygonOffsetFill);
      setEnable(dt,GL_POLYGON_OFFSET_LINE,polygonOffsetLine);
      setEnable(dt,GL_POLYGON_OFFSET_POINT,polygonOffsetPoint);
      setEnable(dt,GL_POLYGON_SMOOTH,polygonSmooth);
      setEnable(dt,GL_POLYGON_STIPPLE,polygonStipple);
      setEnable(dt,GL_POST_COLOR_MATRIX_COLOR_TABLE,postColorMatrixColorTable);
      setEnable(dt,GL_POST_CONVOLUTION_COLOR_TABLE,postConvolutionColorTable);
      setEnable(dt,GL_PROGRAM_POINT_SIZE,programPointSize);
      setEnable(dt,GL_RESCALE_NORMAL,rescaleNormal);
      setEnable(dt,GL_SAMPLE_ALPHA_TO_COVERAGE,sampleAlphaToCoverage);
      setEnable(dt,GL_SAMPLE_ALPHA_TO_ONE,sampleAlphaToOne);
      setEnable(dt,GL_SAMPLE_COVERAGE,sampleCoverage);
      setEnable(dt,GL_SAMPLE_SHADING,sampleShading);
      setEnable(dt,GL_SEPARABLE_2D,separable2d);
      setEnable(dt,GL_STENCIL_TEST,stencilTest);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        setEnablei(dt,GL_SCISSOR_TEST,ii,scissorTest[ii]);
      {
        for (GLuint ii=0; ii<REGAL_EMU_MAX_TEXTURE_UNITS; ii++)
        {
          setEnableIndexedEXT(dt,GL_TEXTURE_1D,ii,texture1d[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_2D,ii,texture2d[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_3D,ii,texture3d[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_CUBE_MAP,ii,textureCubeMap[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_GEN_S,ii,textureGenS[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_GEN_T,ii,textureGenT[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_GEN_R,ii,textureGenR[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_GEN_Q,ii,textureGenQ[ii]);
        }
      }

      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, alphaTest, "GL_ALPHA_TEST",delim);
      enableToString(tmp, autoNormal, "GL_AUTO_NORMAL",delim);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        enableiToString(tmp, blend[ii], "GL_BLEND", ii,delim);
      tmp << print_string("glClampColor(GL_CLAMP_FRAGMENT_COLOR",Token::toString(clampFragmentColor),");",delim);
      tmp << print_string("glClampColor(GL_CLAMP_READ_COLOR",Token::toString(clampReadColor),");",delim);
      tmp << print_string("glClampColor(GL_CLAMP_VERTEX_COLOR",Token::toString(clampVertexColor),");",delim);
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES; ii++)
        tmp << print_string(clipDistance[ii] ? "glEnable" : "glDisable","(GL_CLIP_DISTANCE",ii,");",delim);
      enableToString(tmp, colorLogicOp, "GL_COLOR_LOGIC_OP",delim);
      enableToString(tmp, colorMaterial, "GL_COLOR_MATERIAL",delim);
      enableToString(tmp, colorSum, "GL_COLOR_SUM",delim);
      enableToString(tmp, colorTable, "GL_COLOR_TABLE",delim);
      enableToString(tmp, convolution1d, "GL_CONVOLUTION_1D",delim);
      enableToString(tmp, convolution2d, "GL_CONVOLUTION_2D",delim);
      enableToString(tmp, cullFace, "GL_CULL_FACE",delim);
      enableToString(tmp, depthTest, "GL_DEPTH_TEST",delim);
      enableToString(tmp, depthClamp, "GL_DEPTH_CLAMP",delim);
      enableToString(tmp, dither, "GL_DITHER",delim);
      enableToString(tmp, fog, "GL_FOG",delim);
      enableToString(tmp, framebufferSRGB, "GL_FRAMEBUFFER_SRGB",delim);
      enableToString(tmp, histogram, "GL_HISTOGRAM",delim);
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_LIGHTS; ii++)
        tmp << print_string(light[ii] ? "glEnable" : "glDisable","(GL_LIGHT",ii,");",delim);
      enableToString(tmp, lighting, "GL_LIGHTING",delim);
      enableToString(tmp, lineSmooth, "GL_LINE_SMOOTH",delim);
      enableToString(tmp, lineStipple, "GL_LINE_STIPPLE",delim);
      enableToString(tmp, indexLogicOp, "GL_INDEX_LOGIC_OP",delim);
      enableToString(tmp, map1Color4, "GL_MAP1_COLOR_4",delim);
      enableToString(tmp, map1Index, "GL_MAP1_INDEX",delim);
      enableToString(tmp, map1Normal, "GL_MAP1_NORMAL",delim);
      enableToString(tmp, map1TextureCoord1, "GL_MAP1_TEXTURE_COORD_1",delim);
      enableToString(tmp, map1TextureCoord2, "GL_MAP1_TEXTURE_COORD_2",delim);
      enableToString(tmp, map1TextureCoord3, "GL_MAP1_TEXTURE_COORD_3",delim);
      enableToString(tmp, map1TextureCoord4, "GL_MAP1_TEXTURE_COORD_4",delim);
      enableToString(tmp, map1Vertex3, "GL_MAP1_VERTEX_3",delim);
      enableToString(tmp, map1Vertex4, "GL_MAP1_VERTEX_4",delim);
      enableToString(tmp, map2Color4, "GL_MAP2_COLOR_4",delim);
      enableToString(tmp, map2Index, "GL_MAP2_INDEX",delim);
      enableToString(tmp, map2Normal, "GL_MAP2_NORMAL",delim);
      enableToString(tmp, map2TextureCoord1, "GL_MAP2_TEXTURE_COORD_1",delim);
      enableToString(tmp, map2TextureCoord2, "GL_MAP2_TEXTURE_COORD_2",delim);
      enableToString(tmp, map2TextureCoord3, "GL_MAP2_TEXTURE_COORD_3",delim);
      enableToString(tmp, map2TextureCoord4, "GL_MAP2_TEXTURE_COORD_4",delim);
      enableToString(tmp, map2Vertex3, "GL_MAP2_VERTEX_3",delim);
      enableToString(tmp, map2Vertex4, "GL_MAP2_VERTEX_4",delim);
      enableToString(tmp, minmax, "GL_MINMAX",delim);
      enableToString(tmp, multisample, "GL_MULTISAMPLE",delim);
      enableToString(tmp, normalize, "GL_NORMALIZE",delim);
      enableToString(tmp, pointSmooth, "GL_POINT_SMOOTH",delim);
      enableToString(tmp, pointSprite, "GL_POINT_SPRITE",delim);
      enableToString(tmp, polygonOffsetLine, "GL_POLYGON_OFFSET_LINE",delim);
      enableToString(tmp, polygonOffsetFill, "GL_POLYGON_OFFSET_FILL",delim);
      enableToString(tmp, polygonOffsetPoint, "GL_POLYGON_OFFSET_POINT",delim);

      enableToString(tmp, polygonSmooth, "GL_POLYGON_SMOOTH",delim);
      enableToString(tmp, polygonStipple, "GL_POLYGON_STIPPLE",delim);
      enableToString(tmp, postConvolutionColorTable, "GL_POST_CONVOLUTION_COLOR_TABLE",delim);
      enableToString(tmp, postColorMatrixColorTable, "GL_POST_COLOR_MATRIX_COLOR_TABLE",delim);
      enableToString(tmp, programPointSize, "GL_PROGRAM_POINT_SIZE",delim);
      enableToString(tmp, rescaleNormal, "GL_RESCALE_NORMAL",delim);
      enableToString(tmp, sampleAlphaToCoverage, "GL_SAMPLE_ALPHA_TO_COVERAGE",delim);

      enableToString(tmp, sampleAlphaToOne, "GL_SAMPLE_ALPHA_TO_ONE",delim);
      enableToString(tmp, sampleCoverage, "GL_SAMPLE_COVERAGE",delim);
      enableToString(tmp, sampleShading, "GL_SAMPLE_SHADING",delim);
      enableToString(tmp, separable2d, "GL_SEPARABLE_2D",delim);
      enableToString(tmp, stencilTest, "GL_STENCIL_TEST",delim);
      enableToString(tmp, vertexProgramTwoSide, "GL_VERTEX_PROGRAM_TWO_SIDE",delim);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        enableiToString(tmp, scissorTest[ii], "GL_SCISSOR_TEST", ii,delim);
      {
        for (int ii=0; ii<REGAL_EMU_MAX_TEXTURE_UNITS; ii++)
        {
          tmp << print_string("Texture unit ",ii,": ",texture1d[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_1D);",delim);
          tmp << print_string("Texture unit ",ii,": ",texture2d[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_2D);",delim);
          tmp << print_string("Texture unit ",ii,": ",texture3d[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_3D);",delim);
          tmp << print_string("Texture unit ",ii,": ",textureCubeMap[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_CUBE_MAP);",delim);
          tmp << print_string("Texture unit ",ii,": ",textureGenS[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_GEN_S);",delim);
          tmp << print_string("Texture unit ",ii,": ",textureGenT[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_GEN_T);",delim);
          tmp << print_string("Texture unit ",ii,": ",textureGenR[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_GEN_R);",delim);
          tmp << print_string("Texture unit ",ii,": ",textureGenQ[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_GEN_Q);",delim);
        }
      }
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_DEPTH_BUFFER_BIT)
  //

  struct Depth
  {
    GLboolean   enable;
    GLenum      func;
    GLclampd    clear;
    GLboolean   mask;

    inline Depth()
    : enable(GL_FALSE), func(GL_LESS), clear(1.0), mask(GL_TRUE)
    {
    }

    inline Depth &swap(Depth &other)
    {
      std::swap(enable,other.enable);
      std::swap(func,other.func);
      std::swap(clear,other.clear);
      std::swap(mask,other.mask);
      return *this;
    }

    inline void glClearDepthf(GLclampf depth)
    {
      clear = GLclampd(depth);
    }

    inline void glClearDepth(GLclampd depth)
    {
      clear = depth;
    }

    inline void glDepthFunc(GLenum f)
    {
      func = f;
    }

    inline void glDepthMask(GLboolean m)
    {
      mask = m;
    }

    inline Depth &get(DispatchTable &dt)
    {
      enable = dt.call(&dt.glIsEnabled)(GL_DEPTH_TEST);
      dt.call(&dt.glGetIntegerv)(GL_DEPTH_FUNC,reinterpret_cast<GLint *>(&func));
      dt.call(&dt.glGetFloatv)(GL_DEPTH_CLEAR_VALUE,reinterpret_cast<GLfloat *>(&clear));
      dt.call(&dt.glGetBooleanv)(GL_DEPTH_WRITEMASK,&mask);
      return *this;
    }

    inline const Depth &set(DispatchTable &dt) const
    {
      if (enable)
        dt.call(&dt.glEnable)(GL_DEPTH_TEST);
      else
        dt.call(&dt.glDisable)(GL_DEPTH_TEST);
      dt.call(&dt.glDepthFunc)(func);
      dt.call(&dt.glClearDepth)(clear);
      dt.call(&dt.glDepthMask)(mask);

      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, enable, "GL_DEPTH_TEST", delim);
      tmp << print_string("glDepthfunc(",Token::toString(func),");",delim);
      tmp << print_string("glClearDepth(",clear,");",delim);
      tmp << print_string("glDepthMask(",mask ? "GL_TRUE" : "GL_FALSE",");",delim);
      return tmp;
    }
  };

  struct StencilFace
  {
    GLenum func;      // glStencilFunc
    GLint  ref;       // glStencilFunc
    GLuint valueMask; // glStencilFunc
    GLuint writeMask; // glStencilMask
    GLenum fail;      // glStencilOp
    GLenum zfail;     // glStencilOp
    GLenum zpass;     // glStencilOp

    inline StencilFace()
    : func     (GL_ALWAYS),
      ref      (0),
      valueMask(~0u),
      writeMask(~0u),
      fail     (GL_KEEP),
      zfail    (GL_KEEP),
      zpass    (GL_KEEP)
    {
    }

    inline StencilFace &swap(StencilFace &other)
    {
      std::swap(func,     other.func);
      std::swap(ref,      other.ref);
      std::swap(valueMask,other.valueMask);
      std::swap(writeMask,other.writeMask);
      std::swap(fail,     other.fail);
      std::swap(zfail,    other.zfail);
      std::swap(zpass,    other.zpass);
      return *this;
    }

    inline StencilFace &get(DispatchTable &dt, GLenum face)
    {
      dt.call(&dt.glGetIntegerv)(face==GL_FRONT ? GL_STENCIL_FUNC            : GL_STENCIL_BACK_FUNC,            reinterpret_cast<GLint *>(&func)     );
      dt.call(&dt.glGetIntegerv)(face==GL_FRONT ? GL_STENCIL_REF             : GL_STENCIL_BACK_REF,             &ref                                 );
      dt.call(&dt.glGetIntegerv)(face==GL_FRONT ? GL_STENCIL_VALUE_MASK      : GL_STENCIL_BACK_VALUE_MASK,      reinterpret_cast<GLint *>(&valueMask));
      dt.call(&dt.glGetIntegerv)(face==GL_FRONT ? GL_STENCIL_WRITEMASK       : GL_STENCIL_BACK_WRITEMASK,       reinterpret_cast<GLint *>(&writeMask));
      dt.call(&dt.glGetIntegerv)(face==GL_FRONT ? GL_STENCIL_FAIL            : GL_STENCIL_BACK_FAIL,            reinterpret_cast<GLint *>(&fail)     );
      dt.call(&dt.glGetIntegerv)(face==GL_FRONT ? GL_STENCIL_PASS_DEPTH_FAIL : GL_STENCIL_BACK_PASS_DEPTH_FAIL, reinterpret_cast<GLint *>(&zfail)    );
      dt.call(&dt.glGetIntegerv)(face==GL_FRONT ? GL_STENCIL_PASS_DEPTH_PASS : GL_STENCIL_BACK_PASS_DEPTH_PASS, reinterpret_cast<GLint *>(&zpass)    );
      return *this;
    }

    inline const StencilFace &set(DispatchTable &dt, GLenum face) const
    {
      dt.call(&dt.glStencilFuncSeparate)(face,func,ref,valueMask);
      dt.call(&dt.glStencilMaskSeparate)(face,writeMask);
      dt.call(&dt.glStencilOpSeparate)(face,fail,zfail,zpass);
      return *this;
    }

    inline std::string toString(GLenum face,const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glStencilFuncSeparate(",Token::toString(face),",",Token::toString(func),",",ref,",0x",hex(valueMask),");",delim);
      tmp << print_string("glStencilMaskSeparate(",Token::toString(face),",0x",hex(writeMask),");",delim);
      tmp << print_string("glStencilOpSeparate(",Token::toString(face),",",Token::toString(fail),",",Token::toString(zfail),",",Token::toString(zpass),");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_STENCIL_BUFFER_BIT)
  //

  struct Stencil
  {
    GLboolean   enable;
    GLint       clear;
    StencilFace front;
    StencilFace back;

    inline Stencil()
    : enable(GL_FALSE), clear(0)
    {
    }

    inline Stencil &swap(Stencil &other)
    {
      std::swap(enable,other.enable);
      std::swap(clear,other.clear);
      front.swap(other.front);
      back.swap(other.back);
      return *this;
    }

    inline void glClearStencil(GLint s)
    {
      clear = s;
    }

    inline void glStencilFunc(GLenum func, GLint ref, GLuint mask)
    {
      front.func      = back.func      = func;
      front.ref       = back.ref       = ref;
      front.valueMask = back.valueMask = mask;
    }

    inline void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
    {
      switch (face)
      {
        case GL_FRONT:
          front.func      = func;
          front.ref       = ref;
          front.valueMask = mask;
          break;
        case GL_BACK:
          back.func      = func;
          back.ref       = ref;
          back.valueMask = mask;
          break;
        case GL_FRONT_AND_BACK:
          front.func      = back.func      = func;
          front.ref       = back.ref       = ref;
          front.valueMask = back.valueMask = mask;
          break;
        default:
          RegalAssert(face==GL_FRONT || face==GL_BACK || face==GL_FRONT_AND_BACK);
          break;
      }
    }

    inline void glStencilMask(GLuint mask)
    {
      front.writeMask = back.writeMask = mask;
    }

    inline void glStencilMaskSeparate(GLenum face, GLuint mask)
    {
      switch (face)
      {
        case GL_FRONT:            front.writeMask = mask;                  break;
        case GL_BACK:             back.writeMask  = mask;                  break;
        case GL_FRONT_AND_BACK:   front.writeMask = back.writeMask = mask; break;
        default:
          RegalAssert(face==GL_FRONT || face==GL_BACK || face==GL_FRONT_AND_BACK);
          break;
      }
    }

    inline void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
    {
      front.fail  = back.fail  = fail;
      front.zfail = back.zfail = zfail;
      front.zpass = back.zpass = zpass;
    }

    inline void glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
    {
      switch (face)
      {
        case GL_FRONT:
          front.fail  = fail;
          front.zfail = zfail;
          front.zpass = zpass;
          break;
        case GL_BACK:
          back.fail  = fail;
          back.zfail = zfail;
          back.zpass = zpass;
          break;
        case GL_FRONT_AND_BACK:
          front.fail  = back.fail  = fail;
          front.zfail = back.zfail = zfail;
          front.zpass = back.zpass = zpass;
          break;
        default:
          RegalAssert(face==GL_FRONT || face==GL_BACK || face==GL_FRONT_AND_BACK);
          break;
      }
    }

    inline Stencil &get(DispatchTable &dt)
    {
      enable = dt.call(&dt.glIsEnabled)(GL_STENCIL_TEST);
      dt.call(&dt.glGetIntegerv)(GL_STENCIL_CLEAR_VALUE,&clear);
      front.get(dt,GL_FRONT);
      back.get(dt,GL_BACK );   // What about GL < 2.0 ?
      return *this;
    }

    inline const Stencil &set(DispatchTable &dt) const
    {
      if (enable)
        dt.call(&dt.glEnable)(GL_STENCIL_TEST);
      else
        dt.call(&dt.glDisable)(GL_STENCIL_TEST);
      dt.call(&dt.glClearStencil)(clear);
      front.set(dt,GL_FRONT);
      back.set(dt,GL_BACK);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, enable, "GL_STENCIL_TEST", delim);
      tmp << print_string("glClearStencil(",clear,");",delim);
      tmp << front.toString(GL_FRONT,delim);
      tmp << front.toString(GL_BACK,delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_POLYGON_BIT)
  //

  struct Polygon
  {
    GLboolean   cullEnable;
    GLenum      cull;
    GLenum      frontFace;
    GLenum      frontMode;
    GLenum      backMode;
    GLboolean   smoothEnable;
    GLboolean   stippleEnable;
    GLboolean   offsetFill;
    GLboolean   offsetLine;
    GLboolean   offsetPoint;
    GLfloat     factor;
    GLfloat     units;

    inline Polygon()
    : cullEnable(GL_FALSE), cull(GL_BACK), frontFace(GL_CCW), frontMode(GL_FILL), backMode(GL_FILL),
      smoothEnable(GL_FALSE), stippleEnable(GL_FALSE), offsetFill(GL_FALSE), offsetLine(GL_FALSE), offsetPoint(GL_FALSE),
      factor(0), units(0)
    {
    }

    inline Polygon &swap(Polygon &other)
    {
      std::swap(cullEnable,other.cullEnable);
      std::swap(cull,other.cull);
      std::swap(frontFace,other.frontFace);
      std::swap(frontMode,other.frontMode);
      std::swap(backMode,other.backMode);
      std::swap(smoothEnable,other.smoothEnable);
      std::swap(stippleEnable,other.stippleEnable);
      std::swap(offsetFill,other.offsetFill);
      std::swap(offsetLine,other.offsetLine);
      std::swap(offsetPoint,other.offsetPoint);
      std::swap(factor,other.factor);
      std::swap(units,other.units);
      return *this;
    }

    inline void glCullFace(GLenum mode)
    {
      cull = mode;
    }

    inline void glFrontFace(GLenum mode)
    {
      frontFace = mode;
    }

    inline void glPolygonMode(GLenum f, GLenum mode)
    {
      switch (f)
      {
        case GL_FRONT:          frontMode = mode;            break;
        case GL_BACK:           backMode  = mode;            break;
        case GL_FRONT_AND_BACK: frontMode = backMode = mode; break;
        default:                                             break;
      }
    }

    inline void glPolygonOffset(GLfloat f, GLfloat u)
    {
      factor = f;
      units  = u;
    }

    inline Polygon &get(DispatchTable &dt)
    {
      cullEnable = dt.call(&dt.glIsEnabled)(GL_CULL_FACE);
      dt.call(&dt.glGetIntegerv)(GL_CULL_FACE_MODE,reinterpret_cast<GLint *>(&cull));
      dt.call(&dt.glGetIntegerv)(GL_FRONT_FACE,reinterpret_cast<GLint *>(&frontFace));
      dt.call(&dt.glGetIntegerv)(GL_POLYGON_MODE,reinterpret_cast<GLint *>(&frontMode));
      dt.call(&dt.glGetIntegerv)(GL_FRONT_FACE,reinterpret_cast<GLint *>(&frontFace));

      smoothEnable  = dt.call(&dt.glIsEnabled)(GL_POLYGON_SMOOTH);
      stippleEnable = dt.call(&dt.glIsEnabled)(GL_POLYGON_STIPPLE);
      offsetFill    = dt.call(&dt.glIsEnabled)(GL_POLYGON_OFFSET_FILL);
      offsetLine    = dt.call(&dt.glIsEnabled)(GL_POLYGON_OFFSET_LINE);
      offsetPoint   = dt.call(&dt.glIsEnabled)(GL_POLYGON_OFFSET_POINT);
      dt.call(&dt.glGetFloatv)(GL_POLYGON_OFFSET_FACTOR,&factor);
      dt.call(&dt.glGetFloatv)(GL_POLYGON_OFFSET_UNITS,&units);
      return *this;
    }

    inline const Polygon &set(DispatchTable &dt) const
    {
      if (cullEnable)
        dt.call(&dt.glEnable)(GL_CULL_FACE);
      else
        dt.call(&dt.glDisable)(GL_CULL_FACE);

      dt.call(&dt.glCullFace)(cull);

      dt.call(&dt.glFrontFace)(frontFace);

      dt.call(&dt.glPolygonMode)(GL_FRONT,frontMode);
      dt.call(&dt.glPolygonMode)(GL_BACK,backMode);

      if (smoothEnable)
        dt.call(&dt.glEnable)(GL_POLYGON_SMOOTH);
      else
        dt.call(&dt.glDisable)(GL_POLYGON_SMOOTH);

      if (stippleEnable)
        dt.call(&dt.glEnable)(GL_POLYGON_STIPPLE);
      else
        dt.call(&dt.glDisable)(GL_POLYGON_STIPPLE);

      if (offsetFill)
        dt.call(&dt.glEnable)(GL_POLYGON_OFFSET_FILL);
      else
        dt.call(&dt.glDisable)(GL_POLYGON_OFFSET_FILL);

      if (offsetLine)
        dt.call(&dt.glEnable)(GL_POLYGON_OFFSET_LINE);
      else
        dt.call(&dt.glDisable)(GL_POLYGON_OFFSET_LINE);

      if (offsetPoint)
        dt.call(&dt.glEnable)(GL_POLYGON_OFFSET_POINT);
      else
        dt.call(&dt.glDisable)(GL_POLYGON_OFFSET_POINT);

      dt.call(&dt.glPolygonOffset)(factor,units);

      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, cullEnable, "GL_CULL_FACE", delim);
      tmp << print_string("glCullFace(",Token::toString(cull),");",delim);
      tmp << print_string("glFrontFace(",Token::toString(frontFace),");",delim);
      tmp << print_string("glPolygonMode(GL_FRONT,",Token::toString(frontMode),");",delim);
      tmp << print_string("glPolygonMode(GL_BACK,",Token::toString(backMode),");",delim);
      enableToString(tmp, smoothEnable, "GL_POLYGON_SMOOTH", delim);
      enableToString(tmp, stippleEnable, "GL_POLYGON_STIPPLE", delim);
      enableToString(tmp, offsetFill, "GL_POLYGON_OFFSET_FILL", delim);
      enableToString(tmp, offsetLine, "GL_POLYGON_OFFSET_LINE", delim);
      enableToString(tmp, offsetPoint, "GL_POLYGON_OFFSET_POINT", delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_TRANSFORM_BIT)
  //

  struct ClipPlaneEquation
  {
    GLdouble data[4];

    inline ClipPlaneEquation()
    {
      data[0] = data[1] = data[2] = data[3] = 0;
    }

    bool operator!= (const ClipPlaneEquation& other) const
    {
      return (data[0] != other.data[0]) || (data[1] != other.data[1]) || (data[2] != other.data[2]) || (data[3] != other.data[3]);
    }
  };

  struct ClipPlane
  {
    GLboolean enabled;
    ClipPlaneEquation equation;

    inline ClipPlane()
    : enabled(false)
    {
    }

    inline ClipPlane &swap(ClipPlane &other)
    {
      std::swap(enabled,other.enabled);
      std::swap(equation,other.equation);
      return *this;
    }
  };

  struct Transform
  {
    // This state matches glspec43.compatability.20120806.pdf Table 23.10,
    // except possibly extended a bit to allow for extra clip planes.

    ClipPlane   clipPlane[REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES];
    GLenum      matrixMode;
    GLboolean   normalize;
    GLboolean   rescaleNormal;
    GLboolean   depthClamp;

    inline Transform()
    : matrixMode(GL_MODELVIEW), normalize(GL_FALSE), rescaleNormal(GL_FALSE), depthClamp(GL_FALSE)
    {
    }

    inline size_t maxPlanes() const
    {
      return sizeof(clipPlane)/sizeof(ClipPlane);
    }

    inline Transform &swap(Transform &other)
    {
      std::swap_ranges(clipPlane,clipPlane+REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES,other.clipPlane);
      std::swap(matrixMode,other.matrixMode);
      std::swap(normalize,other.normalize);
      std::swap(rescaleNormal,other.rescaleNormal);
      std::swap(depthClamp,other.depthClamp);
      return *this;
    }

    inline void glMatrixMode(GLenum mode)
    {
      matrixMode = mode;
    }

    inline void glClipPlane(GLenum plane, const GLdouble* equation)
    {
      GLuint planeIndex = plane - GL_CLIP_PLANE0;
      RegalAssert(planeIndex < REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES);
      if (planeIndex < REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES)
      {
        clipPlane[planeIndex].equation.data[0] = equation[0];
        clipPlane[planeIndex].equation.data[1] = equation[1];
        clipPlane[planeIndex].equation.data[2] = equation[2];
        clipPlane[planeIndex].equation.data[3] = equation[3];
      }
    }

    inline const Transform &transition(DispatchTable &dt, Transform& current) const
    {
      for (GLint i = 0; i < REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES; i++)
      {
        if (current.clipPlane[i].enabled != clipPlane[i].enabled)
          setEnable(dt, GL_CLIP_PLANE0 + i, clipPlane[i].enabled);

        if (current.clipPlane[i].equation != clipPlane[i].equation)
          dt.call(&dt.glClipPlane)(GL_CLIP_PLANE0 + i, clipPlane[i].equation.data);
      }

      if (current.matrixMode != matrixMode)
        dt.call(&dt.glMatrixMode)(matrixMode);

      if (current.normalize != normalize)
        setEnable(dt, GL_NORMALIZE, normalize);

      if (current.rescaleNormal != rescaleNormal)
        setEnable(dt, GL_RESCALE_NORMAL, rescaleNormal);

      if (current.depthClamp != depthClamp )
        setEnable(dt, GL_DEPTH_CLAMP, depthClamp );

      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      for (GLint i = 0; i < REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES; i++)
      {
        GLenum plane = GL_CLIP_PLANE0 + i;
        tmp << print_string(clipPlane[i].enabled ? "glEnable(" : "glDisable(",Token::toString(plane),")",delim);
        tmp << print_string("glClipPlane(",Token::toString(plane),clipPlane[i].equation.data[0],clipPlane[i].equation.data[1],clipPlane[i].equation.data[2],clipPlane[i].equation.data[3],")",delim);
      }
      tmp << print_string("glMatrixMode(",Token::toString(matrixMode),");",delim);
      enableToString(tmp, normalize, "GL_NORMALIZE", delim);
      enableToString(tmp, rescaleNormal, "GL_RESCALE_NORMAL", delim);
      enableToString(tmp, depthClamp, "GL_DEPTH_CLAMP", delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_HINT_BIT)
  //

  struct Hint
  {
    GLenum perspectiveCorrection;
    GLenum pointSmooth;
    GLenum lineSmooth;
    GLenum polygonSmooth;
    GLenum fog;
    GLenum generateMipmap;
    GLenum textureCompression;
    GLenum fragmentShaderDerivative;

    inline Hint()
    : perspectiveCorrection(GL_DONT_CARE)
    , pointSmooth(GL_DONT_CARE)
    , lineSmooth(GL_DONT_CARE)
    , polygonSmooth(GL_DONT_CARE)
    , fog(GL_DONT_CARE)
    , generateMipmap(GL_DONT_CARE)
    , textureCompression(GL_DONT_CARE)
    , fragmentShaderDerivative(GL_DONT_CARE)
    {
    }

    inline Hint &swap(Hint &other)
    {
      std::swap(perspectiveCorrection, other.perspectiveCorrection);
      std::swap(pointSmooth, other.pointSmooth);
      std::swap(lineSmooth, other.lineSmooth);
      std::swap(polygonSmooth, other.polygonSmooth);
      std::swap(fog, other.fog);
      std::swap(generateMipmap, other.generateMipmap);
      std::swap(textureCompression, other.textureCompression);
      std::swap(fragmentShaderDerivative, other.fragmentShaderDerivative);
      return *this;
    }

    inline void glHint(GLenum target, GLenum mode)
    {
      switch (target)
      {
        case GL_PERSPECTIVE_CORRECTION_HINT: perspectiveCorrection = mode; break;
        case GL_POINT_SMOOTH_HINT: pointSmooth = mode; break;
        case GL_LINE_SMOOTH_HINT: lineSmooth = mode; break;
        case GL_POLYGON_SMOOTH_HINT: polygonSmooth = mode; break;
        case GL_FOG_HINT: fog = mode; break;
        case GL_GENERATE_MIPMAP_HINT: generateMipmap = mode; break;
        case GL_TEXTURE_COMPRESSION_HINT: textureCompression = mode; break;
        case GL_FRAGMENT_SHADER_DERIVATIVE_HINT: fragmentShaderDerivative = mode; break;
        default: break;
      }
    }

    inline Hint &get(DispatchTable &dt)
    {
      dt.call(&dt.glGetIntegerv)(GL_PERSPECTIVE_CORRECTION_HINT,reinterpret_cast<GLint *>(&perspectiveCorrection));
      dt.call(&dt.glGetIntegerv)(GL_POINT_SMOOTH_HINT,reinterpret_cast<GLint *>(&pointSmooth));
      dt.call(&dt.glGetIntegerv)(GL_LINE_SMOOTH_HINT,reinterpret_cast<GLint *>(&lineSmooth));
      dt.call(&dt.glGetIntegerv)(GL_POLYGON_SMOOTH_HINT,reinterpret_cast<GLint *>(&polygonSmooth));
      dt.call(&dt.glGetIntegerv)(GL_FOG_HINT,reinterpret_cast<GLint *>(&fog));
      dt.call(&dt.glGetIntegerv)(GL_GENERATE_MIPMAP_HINT,reinterpret_cast<GLint *>(&generateMipmap));
      dt.call(&dt.glGetIntegerv)(GL_TEXTURE_COMPRESSION_HINT,reinterpret_cast<GLint *>(&textureCompression));
      dt.call(&dt.glGetIntegerv)(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,reinterpret_cast<GLint *>(&fragmentShaderDerivative));
      return *this;
    }

    inline const Hint &set(DispatchTable &dt) const
    {
      dt.call(&dt.glHint)(GL_PERSPECTIVE_CORRECTION_HINT, perspectiveCorrection);
      dt.call(&dt.glHint)(GL_POINT_SMOOTH_HINT, pointSmooth);
      dt.call(&dt.glHint)(GL_LINE_SMOOTH_HINT, lineSmooth);
      dt.call(&dt.glHint)(GL_POLYGON_SMOOTH_HINT, polygonSmooth);
      dt.call(&dt.glHint)(GL_FOG_HINT, fog);
      dt.call(&dt.glHint)(GL_GENERATE_MIPMAP_HINT, generateMipmap);
      dt.call(&dt.glHint)(GL_TEXTURE_COMPRESSION_HINT, textureCompression);
      dt.call(&dt.glHint)(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, fragmentShaderDerivative);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glHint(GL_PERSPECTIVE_CORRECTION_HINT,",Token::toString(perspectiveCorrection),");",delim);
      tmp << print_string("glHint(GL_POINT_SMOOTH_HINT,",Token::toString(pointSmooth),");",delim);
      tmp << print_string("glHint(GL_LINE_SMOOTH_HINT,",Token::toString(lineSmooth),");",delim);
      tmp << print_string("glHint(GL_POLYGON_SMOOTH_HINT,",Token::toString(polygonSmooth),");",delim);
      tmp << print_string("glHint(GL_FOG_HINT,",Token::toString(fog),");",delim);
      tmp << print_string("glHint(GL_GENERATE_MIPMAP_HINT,",Token::toString(generateMipmap),");",delim);
      tmp << print_string("glHint(GL_TEXTURE_COMPRESSION_HINT,",Token::toString(textureCompression),");",delim);
      tmp << print_string("glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,",Token::toString(fragmentShaderDerivative),");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_LIST_BIT)
  //

  struct List
  {
    GLuint base;

    inline List()
    : base(0)
    {
    }

    inline List &swap(List &other)
    {
      std::swap(base,other.base);
      return *this;
    }

    void glListBase( GLuint b )
    {
      base = b;
    }

    inline List &get(DispatchTable &dt)
    {
      dt.call(&dt.glGetIntegerv)(GL_LIST_BASE,reinterpret_cast<GLint *>(&base));
      return *this;
    }

    inline const List &set(DispatchTable &dt) const
    {
      dt.call(&dt.glListBase)(base);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glListBase(",base,");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_ACCUM_BUFFER_BIT)
  //

  struct AccumBuffer
  {
    GLfloat clear[4];

    inline AccumBuffer()
    {
      clear[0] = clear[1] = clear[2] = clear[3] = 0;
    }

    inline AccumBuffer &swap(AccumBuffer &other)
    {
      std::swap_ranges(clear,clear+4,other.clear);
      return *this;
    }

    void glClearAccum(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
      clear[0] = r;
      clear[1] = g;
      clear[2] = b;
      clear[3] = a;
    }

    inline AccumBuffer &get(DispatchTable &dt)
    {
      dt.call(&dt.glGetFloatv)(GL_ACCUM_CLEAR_VALUE,&(clear[0]));
      return *this;
    }

    inline const AccumBuffer &set(DispatchTable &dt) const
    {
      dt.call(&dt.glClearAccum)(clear[0],clear[1],clear[2],clear[3]);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glClearAccum(",clear[0],",",clear[1],",",clear[2],",",clear[3],");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_SCISSOR_BIT)
  //

  struct Scissor
  {
    GLboolean   scissorTest[REGAL_MAX_VIEWPORTS];   // GL_SCISSOR_TEST
    GLint       scissorBox[REGAL_MAX_VIEWPORTS][4]; // GL_SCISSOR_BOX
    bool        valid[REGAL_MAX_VIEWPORTS];

    inline Scissor()
    {
      std::memset(scissorTest,GL_FALSE,sizeof(scissorTest));
      std::memset(scissorBox,0,sizeof(scissorBox));
      std::memset(valid,false,sizeof(valid));
    }

    inline Scissor(const Scissor &other)
    {
      if (this!=&other)
        std::memcpy(this,&other,sizeof(Scissor));
    }

    inline Scissor &swap(Scissor &other)
    {
      std::swap_ranges(scissorTest,scissorTest+REGAL_MAX_VIEWPORTS,other.scissorTest);
      std::swap_ranges(&scissorBox[0][0],&scissorBox[0][0]+(REGAL_MAX_VIEWPORTS*4),&other.scissorBox[0][0]);
      std::swap_ranges(valid,valid+REGAL_MAX_VIEWPORTS,other.valid);
      return *this;
    }

    bool defined() const
    {
      for (GLuint ii = 0; ii < REGAL_MAX_VIEWPORTS; ii++)
      {
        if (!valid[ii])
          return false;
      }
      return true;
    }

    void define(DispatchTable &dt)
    {
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
      {
        if (!valid[ii])
        {
          dt.call(&dt.glGetIntegeri_v)(GL_SCISSOR_BOX, ii, &scissorBox[ii][0]);
          valid[ii] = true;
        }
      }
    }

    void glScissorArrayv( GLuint first, GLsizei count, const GLint *v )
    {
      GLuint last = first + count;
      if (last > REGAL_MAX_VIEWPORTS)
        last = REGAL_MAX_VIEWPORTS;
      for (GLuint ii = first; ii < last; ii++)
      {
        scissorBox[ii][0] = v[0];
        scissorBox[ii][1] = v[1];
        scissorBox[ii][2] = v[2];
        scissorBox[ii][3] = v[3];
        valid[ii] = true;
        v += 4;
      }
    }

    void glScissorIndexed( GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height )
    {
      if (index < REGAL_MAX_VIEWPORTS)
      {
        scissorBox[index][0] = left;
        scissorBox[index][1] = bottom;
        scissorBox[index][2] = width;
        scissorBox[index][3] = height;
        valid[index] = true;
      }
    }

    void glScissorIndexedv( GLuint index, const GLint *v )
    {
      if (index < REGAL_MAX_VIEWPORTS)
      {
        scissorBox[index][0] = v[0];
        scissorBox[index][1] = v[1];
        scissorBox[index][2] = v[2];
        scissorBox[index][3] = v[3];
        valid[index] = true;
      }
    }

    void glScissor( GLint left, GLint bottom, GLsizei width, GLsizei height )
    {
      for (GLuint ii = 0; ii < REGAL_MAX_VIEWPORTS; ii++)
      {
        scissorBox[ii][0] = left;
        scissorBox[ii][1] = bottom;
        scissorBox[ii][2] = width;
        scissorBox[ii][3] = height;
        valid[ii] = true;
      }
    }

    inline Scissor &get(DispatchTable &dt)
    {
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
      {
        scissorTest[ii] = dt.call(&dt.glIsEnabledi)(GL_SCISSOR_TEST,ii);
        dt.call(&dt.glGetIntegeri_v)(GL_SCISSOR_BOX, ii, &scissorBox[ii][0]);
        valid[ii] = true;
      }
      return *this;
    }

    inline const Scissor &set(DispatchTable &dt) const
    {
      RegalAssert(defined());
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        setEnablei(dt,GL_SCISSOR_TEST,ii,scissorTest[ii]);
      dt.call(&dt.glScissorArrayv)(0, REGAL_MAX_VIEWPORTS, &scissorBox[0][0]);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      RegalAssert(defined());
      string_list tmp;
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        enableiToString(tmp, scissorTest[ii], "GL_SCISSOR_TEST", ii, delim);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        tmp << print_string("glScissorIndexed(",ii,",",scissorBox[ii][0],",",scissorBox[ii][1],",",scissorBox[ii][2],",",scissorBox[ii][3],");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_VIEWPORT_BIT)
  //

  struct Viewport
  {
    GLfloat  viewport[REGAL_MAX_VIEWPORTS][4]; // GL_VIEWPORT
    GLclampd depthRange[REGAL_MAX_VIEWPORTS][2]; // GL_DEPTH_RANGE
    bool     valid[REGAL_MAX_VIEWPORTS];

    inline Viewport()
    {
      std::memset(viewport,0,sizeof(viewport));
      for (GLuint ii = 0; ii < REGAL_MAX_VIEWPORTS; ii++)
      {
        depthRange[ii][0] = 0;
        depthRange[ii][1] = 1;
      }
      std::memset(valid,false,sizeof(valid));
    }

    inline Viewport &swap(Viewport &other)
    {
      std::swap_ranges(&viewport[0][0],&viewport[0][0]+(REGAL_MAX_VIEWPORTS*4),&other.viewport[0][0]);
      std::swap_ranges(&depthRange[0][0],&depthRange[0][0]+(REGAL_MAX_VIEWPORTS*2),&other.depthRange[0][0]);
      std::swap_ranges(valid,valid+REGAL_MAX_VIEWPORTS,other.valid);
      return *this;
    }

    bool defined() const
    {
      for (GLuint ii = 0; ii < REGAL_MAX_VIEWPORTS; ii++)
      {
        if (!valid[ii])
          return false;
      }
      return true;
    }

    inline void define(DispatchTable &dt)
    {
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
      {
        dt.call(&dt.glGetFloati_v)(GL_VIEWPORT, ii, &viewport[ii][0]);
        valid[ii] = true;
      }
    }

    void glViewportArrayv( GLuint first, GLsizei count, const GLfloat *v )
    {
      GLuint last = first + count;
      if (last > REGAL_MAX_VIEWPORTS)
        last = REGAL_MAX_VIEWPORTS;
      for (GLuint ii = first; ii < last; ii++)
      {
        viewport[ii][0] = v[0];
        viewport[ii][1] = v[1];
        viewport[ii][2] = v[2];
        viewport[ii][3] = v[3];
        valid[ii] = true;
        v += 4;
      }
    }

    void glViewportIndexedf( GLuint index, GLfloat x, GLfloat y, GLfloat w, float h )
    {
      if (index < REGAL_MAX_VIEWPORTS)
      {
        viewport[index][0] = x;
        viewport[index][1] = y;
        viewport[index][2] = w;
        viewport[index][3] = h;
        valid[index] = true;
      }
    }

    void glViewportIndexedfv( GLuint index, const GLfloat *v )
    {
      if (index < REGAL_MAX_VIEWPORTS)
      {
        viewport[index][0] = v[0];
        viewport[index][1] = v[1];
        viewport[index][2] = v[2];
        viewport[index][3] = v[3];
        valid[index] = true;
      }
    }

    void glViewport( GLint x, GLint y, GLsizei w, GLsizei h )
    {
      Internal("Regal::State::Viewport::glViewport [ ",x,", ",y,", ",w,", ",h,", "," ]");
      for (GLuint ii = 0; ii < REGAL_MAX_VIEWPORTS; ii++)
      {
        viewport[ii][0] = static_cast<GLfloat>(x);
        viewport[ii][1] = static_cast<GLfloat>(y);
        viewport[ii][2] = static_cast<GLfloat>(w);
        viewport[ii][3] = static_cast<GLfloat>(h);
        valid[ii] = true;
      }
    }

    void glDepthRangeArrayv( GLuint first, GLsizei count, const GLdouble *v )
    {
      GLuint last = first + count;
      if (last > REGAL_MAX_VIEWPORTS)
        last = REGAL_MAX_VIEWPORTS;
      for (GLuint ii = first; ii < last; ii++)
      {
        depthRange[ii][0] = v[0];
        depthRange[ii][1] = v[1];
        v += 2;
      }
    }

    void glDepthRangeIndexed( GLuint index, GLdouble n, GLdouble f )
    {
      if (index < REGAL_MAX_VIEWPORTS)
      {
        depthRange[index][0] = n;
        depthRange[index][1] = f;
      }
    }

    void glDepthRange( GLdouble n, GLdouble f )
    {
      for (GLuint ii = 0; ii < REGAL_MAX_VIEWPORTS; ii++)
      {
        depthRange[ii][0] = n;
        depthRange[ii][1] = f;
      }
    }

    void glDepthRangef( GLfloat n, GLfloat f )
    {
      glDepthRange( n, f );
    }

    inline Viewport &get(DispatchTable &dt)
    {
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
      {
        dt.call(&dt.glGetFloati_v)(GL_VIEWPORT, ii, &viewport[ii][0]);
        dt.call(&dt.glGetDoublei_v)(GL_DEPTH_RANGE, ii, &depthRange[ii][0]);
        valid[ii] = true;
      }
      return *this;
    }

    inline const Viewport &set(DispatchTable &dt) const
    {
      RegalAssert(defined());
      dt.call(&dt.glDepthRangeArrayv)(0, REGAL_MAX_VIEWPORTS, &depthRange[0][0] );
      dt.call(&dt.glViewportArrayv)(0, REGAL_MAX_VIEWPORTS, &viewport[0][0] );
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      RegalAssert(defined());
      string_list tmp;
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        tmp << print_string("glViewportIndexedf(",ii,",",viewport[ii][0],",",viewport[ii][1],",",viewport[ii][2],",",viewport[ii][3],");",delim);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        tmp << print_string("glDepthRangeIndexed(",ii,",",depthRange[ii][0],",",depthRange[ii][1],");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_LINE_BIT)
  //

  struct Line
  {
    GLfloat     width;          // GL_LINE_WIDTH
    GLboolean   smooth;         // GL_LINE_SMOOTH
    GLboolean   stipple;        // GL_LINE_STIPPLE
    GLint       stippleRepeat;  // GL_LINE_STIPPLE_REPEAT
    GLushort    stipplePattern; // GL_LINE_STIPPLE_PATTERN

    inline Line()
    : width(1.0)
    , smooth(GL_FALSE)
    , stipple(GL_FALSE)
    , stippleRepeat(1)
    , stipplePattern(static_cast<GLushort>(~0))
    {
    }

    inline Line &swap(Line &other)
    {
      std::swap(width,other.width);
      std::swap(smooth,other.smooth);
      std::swap(stipple,other.stipple);
      std::swap(stippleRepeat,other.stippleRepeat);
      std::swap(stipplePattern,other.stipplePattern);
      return *this;
    }

    void glLineWidth(GLfloat w)
    {
      width = w;
    }

    void glLineStipple( GLint repeat, GLushort pattern )
    {
      stippleRepeat = repeat;
      stipplePattern = pattern;
    }

    inline Line &get(DispatchTable &dt)
    {
      dt.call(&dt.glGetFloatv)(GL_LINE_WIDTH,&width);
      smooth = dt.call(&dt.glIsEnabled)(GL_LINE_SMOOTH);
      stipple = dt.call(&dt.glIsEnabled)(GL_LINE_STIPPLE);
      dt.call(&dt.glGetIntegerv)(GL_LINE_STIPPLE_PATTERN,&stippleRepeat);
      stipplePattern = static_cast<GLushort>(stippleRepeat);
      dt.call(&dt.glGetIntegerv)(GL_LINE_STIPPLE_REPEAT,&stippleRepeat);
      return *this;
    }

    inline const Line &set(DispatchTable &dt) const
    {
      dt.call(&dt.glLineWidth)(width);
      setEnable(dt,GL_LINE_SMOOTH,smooth);
      setEnable(dt,GL_LINE_STIPPLE,stipple);
      dt.call(&dt.glLineStipple)(stippleRepeat, stipplePattern);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glLineWidth(",width,");",delim);
      enableToString(tmp, smooth, "GL_LINE_SMOOTH", delim);
      enableToString(tmp, stipple, "GL_LINE_STIPPLE", delim);
      tmp << print_string("glLineStipple(",stippleRepeat,",0x",hex(stipplePattern),");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_MULTISAMPLE_BIT)
  //

  struct Multisample
  {
    GLboolean   multisample;               // GL_MULTISAMPLE
    GLboolean   sampleAlphaToCoverage;     // GL_SAMPLE_ALPHA_TO_COVERAGE
    GLboolean   sampleAlphaToOne;          // GL_SAMPLE_ALPHA_TO_ONE
    GLboolean   sampleCoverage;            // GL_SAMPLE_COVERAGE
    GLclampf    sampleCoverageValue;       // GL_SAMPLE_COVERAGE_VALUE
    GLboolean   sampleCoverageInvert;      // GL_SAMPLE_COVERAGE_INVERT
    GLboolean   sampleShading;             // GL_SAMPLE_SHADING
    GLfloat     minSampleShadingValue;     // GL_MIN_SAMPLE_SHADING_VALUE

    inline Multisample()
    : multisample(GL_TRUE)
    , sampleAlphaToCoverage(GL_FALSE)
    , sampleAlphaToOne(GL_FALSE)
    , sampleCoverage(GL_FALSE)
    , sampleCoverageValue(1)
    , sampleCoverageInvert(GL_FALSE)
    , sampleShading(GL_FALSE)
    , minSampleShadingValue(0)
    {
    }

    inline Multisample &swap(Multisample &other)
    {
      std::swap(multisample,other.multisample);
      std::swap(sampleAlphaToCoverage,other.sampleAlphaToCoverage);
      std::swap(sampleAlphaToOne,other.sampleAlphaToOne);
      std::swap(sampleCoverage,other.sampleCoverage);
      std::swap(sampleCoverageValue,other.sampleCoverageValue);
      std::swap(sampleCoverageInvert,other.sampleCoverageInvert);
      std::swap(sampleShading,other.sampleShading);
      std::swap(minSampleShadingValue,other.minSampleShadingValue);
      return *this;
    }

    void glSampleCoverage( GLfloat value, GLboolean invert )
    {
      sampleCoverageValue = value;
      sampleCoverageInvert = invert;
    }

    void glMinSampleShading( GLfloat value )
    {
      minSampleShadingValue = value;
    }

    inline Multisample &get(DispatchTable &dt)
    {
      multisample = dt.call(&dt.glIsEnabled)(GL_MULTISAMPLE);
      sampleAlphaToCoverage = dt.call(&dt.glIsEnabled)(GL_SAMPLE_ALPHA_TO_COVERAGE);
      sampleAlphaToOne = dt.call(&dt.glIsEnabled)(GL_SAMPLE_ALPHA_TO_ONE);
      sampleCoverage = dt.call(&dt.glIsEnabled)(GL_SAMPLE_COVERAGE);
      dt.call(&dt.glGetFloatv)(GL_SAMPLE_COVERAGE_VALUE,&sampleCoverageValue);
      sampleCoverageInvert = dt.call(&dt.glIsEnabled)(GL_SAMPLE_COVERAGE_INVERT);
      sampleShading = dt.call(&dt.glIsEnabled)(GL_SAMPLE_SHADING);
      dt.call(&dt.glGetFloatv)(GL_MIN_SAMPLE_SHADING_VALUE,&minSampleShadingValue);
      return *this;
    }

    inline const Multisample &set(DispatchTable &dt) const
    {
      setEnable(dt,GL_MULTISAMPLE,multisample);
      setEnable(dt,GL_SAMPLE_ALPHA_TO_COVERAGE,sampleAlphaToCoverage);
      setEnable(dt,GL_SAMPLE_ALPHA_TO_ONE,sampleAlphaToOne);
      setEnable(dt,GL_SAMPLE_COVERAGE,sampleCoverage);
      dt.call(&dt.glSampleCoverage)(sampleCoverageValue, sampleCoverageInvert);
      setEnable(dt,GL_SAMPLE_SHADING,sampleShading);
      dt.call(&dt.glMinSampleShading)(minSampleShadingValue);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, multisample, "GL_MULTISAMPLE", delim);
      enableToString(tmp, sampleAlphaToCoverage, "GL_SAMPLE_ALPHA_TO_COVERAGE", delim);
      enableToString(tmp, sampleAlphaToOne, "GL_SAMPLE_ALPHA_TO_ONE", delim);
      enableToString(tmp, sampleCoverage, "GL_SAMPLE_COVERAGE", delim);
      tmp << print_string("glSampleCoverage(",sampleCoverageValue,",",sampleCoverageInvert,");",delim);
      enableToString(tmp, sampleShading, "GL_SAMPLE_SHADING", delim);
      tmp << print_string("glMinSampleShading(",minSampleShadingValue,");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_EVAL_BIT)
  //

  struct Eval
  {
    GLboolean   autoNormal;                // GL_AUTO_NORMAL
    GLboolean   map1dEnables[9];           // GL_MAP1_x
    GLboolean   map2dEnables[9];           // GL_MAP2_x
    GLdouble    map1GridDomain[2];         // GL_MAP1_GRID_DOMAIN
    GLdouble    map2GridDomain[4];         // GL_MAP2_GRID_DOMAIN
    GLuint      map1GridSegments;          // GL_MAP1_GRID_SEGMENTS
    GLuint      map2GridSegments[2];       // GL_MAP2_GRID_SEGMENTS

    inline Eval()
    : autoNormal(GL_FALSE)
    , map1GridSegments(1)
    {
      RegalAssert(static_cast<int>(GL_MAP1_VERTEX_4-GL_MAP1_COLOR_4) == 8);
      RegalAssert(static_cast<int>(GL_MAP2_VERTEX_4-GL_MAP2_COLOR_4) == 8);
      std::memset(map1dEnables,GL_FALSE,sizeof(map1dEnables));
      std::memset(map2dEnables,GL_FALSE,sizeof(map2dEnables));
      map1GridSegments = 1;
      map1GridDomain[0] = 0;
      map1GridDomain[1] = 1;
      map2GridSegments[0] = 1;
      map2GridSegments[1] = 1;
      map2GridDomain[0] = 0;
      map2GridDomain[1] = 1;
      map2GridDomain[2] = 0;
      map2GridDomain[3] = 1;
    }

    inline Eval &swap(Eval &other)
    {
      std::swap(autoNormal,other.autoNormal);
      std::swap_ranges(map1dEnables,map1dEnables+9,other.map1dEnables);
      std::swap_ranges(map2dEnables,map2dEnables+9,other.map2dEnables);
      std::swap_ranges(map1GridDomain,map1GridDomain+2,other.map1GridDomain);
      std::swap_ranges(map2GridDomain,map2GridDomain+4,other.map2GridDomain);
      std::swap(map1GridSegments,other.map1GridSegments);
      std::swap_ranges(map2GridSegments,map2GridSegments+2,other.map2GridSegments);
      return *this;
    }

    template <typename T> void glMapGrid1( GLint n, T u1, T u2 )
    {
      map1GridSegments = n;
      map1GridDomain[0] = static_cast<GLdouble>(u1);
      map1GridDomain[1] = static_cast<GLdouble>(u2);
    }

    template <typename T> void glMapGrid2( GLint un, T u1, T u2, GLint vn, T v1, T v2 )
    {
      map2GridSegments[0] = un;
      map2GridDomain[0] = static_cast<GLdouble>(u1);
      map2GridDomain[1] = static_cast<GLdouble>(u2);
      map2GridSegments[1] = vn;
      map2GridDomain[2] = static_cast<GLdouble>(v1);
      map2GridDomain[3] = static_cast<GLdouble>(v2);
    }

    inline Eval &get(DispatchTable &dt)
    {
      autoNormal = dt.call(&dt.glIsEnabled)(GL_AUTO_NORMAL);
      for (GLuint ii=0; ii<9; ii++)
        map1dEnables[ii] = dt.call(&dt.glIsEnabled)(GL_MAP1_COLOR_4+ii);
      for (GLuint ii=0; ii<9; ii++)
        map2dEnables[ii] = dt.call(&dt.glIsEnabled)(GL_MAP2_COLOR_4+ii);
      dt.call(&dt.glGetDoublev)(GL_MAP1_GRID_DOMAIN, map1GridDomain);
      dt.call(&dt.glGetDoublev)(GL_MAP2_GRID_DOMAIN, map2GridDomain);
      dt.call(&dt.glGetIntegerv)(GL_MAP1_GRID_SEGMENTS,reinterpret_cast<GLint*>(&map1GridSegments));
      dt.call(&dt.glGetIntegerv)(GL_MAP2_GRID_SEGMENTS,reinterpret_cast<GLint*>(&map2GridSegments));
      return *this;
    }

    inline const Eval &set(DispatchTable &dt) const
    {
      setEnable(dt,GL_AUTO_NORMAL,autoNormal);
      for (GLuint ii=0; ii<9; ii++)
        setEnable(dt,GL_MAP1_COLOR_4+ii,map1dEnables[ii]);
      for (GLuint ii=0; ii<9; ii++)
        setEnable(dt,GL_MAP2_COLOR_4+ii,map1dEnables[ii]);
      dt.call(&dt.glMapGrid1d)(map1GridSegments, map1GridDomain[0], map1GridDomain[1]);
      dt.call(&dt.glMapGrid2d)(map2GridSegments[0], map2GridDomain[0], map2GridDomain[1],
                               map2GridSegments[1], map2GridDomain[2], map2GridDomain[3]);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, autoNormal, "GL_AUTO_NORMAL",delim);
      for (GLuint ii=0; ii<9; ii++)
        enableToString(tmp,map1dEnables[ii],Token::toString(GL_MAP1_COLOR_4+ii),delim);
      for (GLuint ii=0; ii<9; ii++)
        enableToString(tmp,map2dEnables[ii],Token::toString(GL_MAP2_COLOR_4+ii),delim);
      tmp << print_string("glMapGrid1d(",map1GridSegments,",",map1GridDomain[0],",",map1GridDomain[1],");",delim);
      tmp << print_string("glMapGrid2d(",map2GridSegments[0],",",map2GridDomain[0],",",map2GridDomain[1],
                                         map2GridSegments[1],",",map2GridDomain[2],",",map2GridDomain[3],");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_FOG_BIT)
  //

  struct Fog
  {
    GLfloat   color[4]; // GL_FOG_COLOR
    GLfloat   index;    // GL_FOG_INDEX
    GLfloat   density;  // GL_FOG_DENSITY
    GLfloat   start;    // GL_FOG_START
    GLfloat   end;      // GL_FOG_END
    GLenum    mode;     // GL_FOG_MODE
    GLboolean enable;   // GL_FOG
    GLenum    coordSrc; // GL_FOG_COORD_SRC
    GLboolean colorSum; // GL_COLOR_SUM

    inline Fog()
    : index(0)
    , density(1)
    , start(1)
    , end(1)
    , mode(GL_EXP)
    , enable(GL_FALSE)
    , coordSrc(GL_FRAGMENT_DEPTH)
    , colorSum(GL_FALSE)
    {
      std::memset(color,0,sizeof(color));
    }

    inline Fog &swap(Fog &other)
    {
      std::swap_ranges(color,color+4,other.color);
      std::swap(index,other.index);
      std::swap(density,other.density);
      std::swap(start,other.start);
      std::swap(end,other.end);
      std::swap(mode,other.mode);
      std::swap(enable,other.enable);
      std::swap(coordSrc,other.coordSrc);
      std::swap(colorSum,other.colorSum);
      return *this;
    }

    inline Fog &get(DispatchTable &dt)
    {
      dt.call(&dt.glGetFloatv)(GL_FOG_COLOR,color);
      dt.call(&dt.glGetFloatv)(GL_FOG_INDEX,&index);
      dt.call(&dt.glGetFloatv)(GL_FOG_DENSITY,&density);
      dt.call(&dt.glGetFloatv)(GL_FOG_START,&start);
      dt.call(&dt.glGetFloatv)(GL_FOG_END,&end);
      dt.call(&dt.glGetIntegerv)(GL_FOG_MODE,reinterpret_cast<GLint*>(&mode));
      enable = dt.call(&dt.glIsEnabled)(GL_FOG);
      dt.call(&dt.glGetIntegerv)(GL_FOG_COORD_SRC,reinterpret_cast<GLint*>(&coordSrc));
      colorSum = dt.call(&dt.glIsEnabled)(GL_COLOR_SUM);
      return *this;
    }

    inline const Fog &set(DispatchTable &dt) const
    {
      dt.call(&dt.glFogfv)(GL_FOG_COLOR,color);
      dt.call(&dt.glFogf)(GL_FOG_INDEX,index);
      dt.call(&dt.glFogf)(GL_FOG_DENSITY,density);
      dt.call(&dt.glFogf)(GL_FOG_START,start);
      dt.call(&dt.glFogf)(GL_FOG_END,end);
      dt.call(&dt.glFogi)(GL_FOG_MODE,mode);
      setEnable(dt,GL_FOG,enable);
      dt.call(&dt.glFogi)(GL_FOG_COORD_SRC,coordSrc);
      setEnable(dt,GL_COLOR_SUM,colorSum);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glFogfv(GL_FOG_COLOR,[",color[0],",",color[1],",",color[2],",",color[3],",","] );",delim);
      tmp << print_string("glFogf(GL_FOG_INDEX,",index,");",delim);
      tmp << print_string("glFogf(GL_FOG_DENSITY,",density,");",delim);
      tmp << print_string("glFogf(GL_FOG_START,",start,");",delim);
      tmp << print_string("glFogf(GL_FOG_END,",end,");",delim);
      tmp << print_string("glFogi(GL_FOG_MODE,",Token::toString(mode),");",delim);
      enableToString(tmp, enable, "GL_FOG",delim);
      tmp << print_string("glFogi(GL_FOG_COORD_SRC,",Token::toString(coordSrc),");",delim);
      enableToString(tmp, colorSum, "GL_COLOR_SUM",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_POINT_BIT)
  //

  struct Point
  {
    GLfloat    size;                                      // GL_POINT_SIZE
    GLboolean  smooth;                                    // GL_POINT_SMOOTH
    GLboolean  sprite;                                    // GL_POINT_SPRITE
    GLfloat    sizeMin;                                   // GL_POINT_SIZE_MIN
    GLfloat    sizeMax;                                   // GL_POINT_SIZE_MAX
    GLfloat    fadeThresholdSize;                         // GL_POINT_FADE_THRESHOLD_SIZE
    GLfloat    distanceAttenuation[3];                    // GL_POINT_DISTANCE_ATTENUATION
    GLenum     spriteCoordOrigin;                         // GL_POINT_SPRITE_COORD_ORIGIN
    GLboolean  coordReplace[REGAL_EMU_MAX_TEXTURE_UNITS]; // GL_COORD_REPLACE

    inline Point()
    : size(1)
    , smooth(GL_FALSE)
    , sprite(GL_FALSE)
    , sizeMin(0)
    , sizeMax(1)        // Max of the impl. dependent max. aliased and smooth point sizes.
    , fadeThresholdSize(1)
    , spriteCoordOrigin(GL_UPPER_LEFT)
    {
      distanceAttenuation[0] = 1;
      distanceAttenuation[1] = 0;
      distanceAttenuation[2] = 0;
      std::memset(coordReplace,GL_FALSE,sizeof(coordReplace));
    }

    inline Point &swap(Point &other)
    {
      std::swap(size,other.size);
      std::swap(smooth,other.smooth);
      std::swap(sprite,other.sprite);
      std::swap(sizeMin,other.sizeMin);
      std::swap(sizeMax,other.sizeMax);
      std::swap(fadeThresholdSize,other.fadeThresholdSize);
      std::swap_ranges(distanceAttenuation,distanceAttenuation+3,other.distanceAttenuation);
      std::swap(spriteCoordOrigin,other.spriteCoordOrigin);
      std::swap_ranges(coordReplace,coordReplace+REGAL_EMU_MAX_TEXTURE_UNITS,other.coordReplace);
      return *this;
    }

    void glPointSize(GLfloat s)
    {
      size = s;
    }

    template <typename T> void glPointParameter( GLenum pname, T param )
    {
      switch (pname)
      {
        case GL_POINT_SIZE_MIN:
          sizeMin = static_cast<GLfloat>(param);
          break;
        case GL_POINT_SIZE_MAX:
          sizeMax = static_cast<GLfloat>(param);
          break;
        case GL_POINT_FADE_THRESHOLD_SIZE:
          fadeThresholdSize = static_cast<GLfloat>(param);
          break;
        case GL_POINT_SPRITE_COORD_ORIGIN:
          spriteCoordOrigin = static_cast<GLenum>(param);
          break;
        default:
          break;
      }
    }

    template <typename T> void glPointParameterv( GLenum pname, const T *params )
    {
      switch (pname)
      {
        case GL_POINT_DISTANCE_ATTENUATION:
          distanceAttenuation[0] = static_cast<GLfloat>(params[0]);
          distanceAttenuation[1] = static_cast<GLfloat>(params[1]);
          distanceAttenuation[2] = static_cast<GLfloat>(params[2]);
          break;
        default:
          break;
      }
    }

    template <typename T> void glMultiTexEnv(GLenum texunit, GLenum target, GLenum pname, T param)
    {
      if ((target == GL_POINT_SPRITE) && (pname == GL_COORD_REPLACE))
      {
        GLint unit = texunit = GL_TEXTURE0;
        if ((unit >= 0) && (unit<REGAL_EMU_MAX_TEXTURE_UNITS))
          coordReplace[unit] = static_cast<GLboolean>(param);
      }
    }

    template <typename T> void glMultiTexEnvv(GLenum texunit, GLenum target, GLenum pname, const T *params)
    {
      if ((target == GL_POINT_SPRITE) && (pname == GL_COORD_REPLACE))
      {
        GLint unit = texunit = GL_TEXTURE0;
        if ((unit >= 0) && (unit<REGAL_EMU_MAX_TEXTURE_UNITS))
          coordReplace[unit] = static_cast<GLboolean>(params[0]);
      }
    }

    inline Point &get(DispatchTable &dt)
    {
      dt.call(&dt.glGetFloatv)(GL_POINT_SIZE,&size);
      smooth = dt.call(&dt.glIsEnabled)(GL_POINT_SMOOTH);
      sprite = dt.call(&dt.glIsEnabled)(GL_POINT_SPRITE);
      dt.call(&dt.glGetFloatv)(GL_POINT_SIZE_MIN,&sizeMin);
      dt.call(&dt.glGetFloatv)(GL_POINT_SIZE_MAX,&sizeMin);
      dt.call(&dt.glGetFloatv)(GL_POINT_FADE_THRESHOLD_SIZE,&fadeThresholdSize);
      dt.call(&dt.glGetFloatv)(GL_POINT_DISTANCE_ATTENUATION,distanceAttenuation);
      dt.call(&dt.glGetIntegerv)(GL_POINT_SPRITE_COORD_ORIGIN,reinterpret_cast<GLint*>(&spriteCoordOrigin));
      for (GLuint ii=0; ii<REGAL_EMU_MAX_TEXTURE_UNITS; ii++)
        dt.call(&dt.glGetMultiTexEnvivEXT)(ii,GL_POINT_SPRITE,GL_COORD_REPLACE,reinterpret_cast<GLint*>(&coordReplace[ii]));
      return *this;
    }

    inline const Point &set(DispatchTable &dt) const
    {
      dt.call(&dt.glPointSize)(size);
      setEnable(dt,GL_POINT_SMOOTH,smooth);
      setEnable(dt,GL_POINT_SPRITE,sprite);
      dt.call(&dt.glPointParameterf)(GL_POINT_SIZE_MIN,sizeMin);
      dt.call(&dt.glPointParameterf)(GL_POINT_SIZE_MAX,sizeMax);
      dt.call(&dt.glPointParameterf)(GL_POINT_FADE_THRESHOLD_SIZE,fadeThresholdSize);
      dt.call(&dt.glPointParameterfv)(GL_POINT_DISTANCE_ATTENUATION,distanceAttenuation);
      dt.call(&dt.glPointParameteri)(GL_POINT_SPRITE_COORD_ORIGIN,spriteCoordOrigin);
      for (GLuint ii=0; ii<REGAL_EMU_MAX_TEXTURE_UNITS; ii++)
        dt.call(&dt.glMultiTexEnviEXT)(ii,GL_POINT_SPRITE,GL_COORD_REPLACE,coordReplace[ii]);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glPointSize(",size,");",delim);
      enableToString(tmp, smooth, "GL_POINT_SMOOTH",delim);
      enableToString(tmp, sprite, "GL_POINT_SPRITE",delim);
      tmp << print_string("glPointParameterf(GL_POINT_SIZE_MIN",sizeMin,");",delim);
      tmp << print_string("glPointParameterf(GL_POINT_SIZE_MAX",sizeMax,");",delim);
      tmp << print_string("glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE",fadeThresholdSize,");",delim);
      tmp << print_string("glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, [ ",distanceAttenuation[0],", ",
                                                                                 distanceAttenuation[1],", ",
                                                                                 distanceAttenuation[2]," ]);",delim);
      tmp << print_string("glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN",Token::toString(spriteCoordOrigin),");",delim);
      for (GLuint ii=0; ii<REGAL_EMU_MAX_TEXTURE_UNITS; ii++)
        tmp << print_string("glMultiTexEnviEXT(",ii,",GL_POINT_SPRITE,GL_COORD_REPLACE,",coordReplace[ii],");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_POLYGON_STIPPLE_BIT)
  //

  struct PolygonStipple
  {
    GLubyte pattern[32*4];

    inline PolygonStipple()
    {
      std::memset(pattern,0xff,sizeof(pattern));
    }

    inline PolygonStipple &swap(PolygonStipple &other)
    {
      std::swap_ranges(pattern,pattern+(32*4),other.pattern);
      return *this;
    }

    void glPolygonStipple( const GLubyte *p )
    {
      for (int ii=0; ii<(32*4); ii++)
        pattern[ii] = p[ii];
    }

    inline PolygonStipple &get(DispatchTable &dt)
    {
      dt.call(&dt.glGetPolygonStipple)(pattern);
      return *this;
    }

    inline const PolygonStipple &set(DispatchTable &dt) const
    {
      dt.call(&dt.glPolygonStipple)(pattern);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glPolygonStipple([");
      for (int ii=0; ii<(32*4)-1; ii++)
        tmp << print_string(" 0x",hex(pattern[ii]),",");
      tmp << print_string(" 0x",hex(pattern[(32*4)-1]),"]);",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_COLOR_BUFFER_BIT)
  //

  struct ColorBuffer
  {
    GLenum      clampFragmentColor;                         // GL_CLAMP_FRAGMENT_COLOR
    GLenum      clampReadColor;                             // GL_CLAMP_READ_COLOR
    GLboolean   alphaTest;                                  // GL_ALPHA_TEST
    GLenum      alphaTestFunc;                              // GL_ALPHA_TEST_FUNC
    GLclampf    alphaTestRef;                               // GL_ALPHA_TEST_REF
    GLboolean   blend[REGAL_MAX_DRAW_BUFFERS];              // GL_BLEND
    GLenum      blendSrcRgb[REGAL_MAX_DRAW_BUFFERS];        // GL_BLEND_SRC_RGB
    GLenum      blendSrcAlpha[REGAL_MAX_DRAW_BUFFERS];      // GL_BLEND_SRC_ALPHA
    GLenum      blendDstRgb[REGAL_MAX_DRAW_BUFFERS];        // GL_BLEND_DST_RGB
    GLenum      blendDstAlpha[REGAL_MAX_DRAW_BUFFERS];      // GL_BLEND_DST_ALPHA
    GLenum      blendEquationRgb[REGAL_MAX_DRAW_BUFFERS];   // GL_BLEND_EQUATION_RGB
    GLenum      blendEquationAlpha[REGAL_MAX_DRAW_BUFFERS]; // GL_BLEND_EQUATION_ALPHA
    GLclampf    blendColor[4];                              // GL_BLEND_COLOR
    GLboolean   framebufferSRGB;                            // GL_FRAMEBUFFER_SRGB
    GLboolean   dither;                                     // GL_DITHER
    GLboolean   indexLogicOp;                               // GL_INDEX_LOGIC_OP
    GLboolean   colorLogicOp;                               // GL_COLOR_LOGIC_OP
    GLenum      logicOpMode;                                // GL_LOGIC_OP_MODE
    GLuint      indexWritemask;                             // GL_INDEX_WRITEMASK
    GLboolean   colorWritemask[REGAL_MAX_DRAW_BUFFERS][4];  // GL_COLOR_WRITEMASK
    GLclampf    colorClearValue[4];                         // GL_COLOR_CLEAR_VALUE
    GLfloat     indexClearValue;                            // GL_INDEX_CLEAR_VALUE
    GLenum      drawBuffers[REGAL_MAX_DRAW_BUFFERS];        // GL_DRAW_BUFERi
    bool        valid;

    inline ColorBuffer()
    : clampFragmentColor(GL_FIXED_ONLY)
    , clampReadColor(GL_FIXED_ONLY)
    , alphaTest(GL_FALSE)
    , alphaTestFunc(GL_ALWAYS)
    , alphaTestRef(0)
    , framebufferSRGB(GL_FALSE)
    , dither(GL_TRUE)
    , indexLogicOp(GL_FALSE)
    , colorLogicOp(GL_FALSE)
    , logicOpMode(GL_COPY)
    , indexWritemask(static_cast<GLuint>(~0))
    , indexClearValue(0)
    , valid(false)
    {
      std::memset(blend,GL_FALSE,sizeof(blend));
      std::memset(blendSrcRgb,GL_ONE,sizeof(blendSrcRgb));
      std::memset(blendSrcAlpha,GL_ONE,sizeof(blendSrcAlpha));
      std::memset(blendDstRgb,GL_ZERO,sizeof(blendDstRgb));
      std::memset(blendDstAlpha,GL_ZERO,sizeof(blendDstAlpha));
      std::memset(blendEquationRgb,GL_FUNC_ADD,sizeof(blendEquationRgb));
      std::memset(blendEquationAlpha,GL_FUNC_ADD,sizeof(blendEquationAlpha));
      std::memset(blendColor,0,sizeof(blendColor));
      std::memset(colorWritemask,GL_TRUE,sizeof(colorWritemask));
      std::memset(colorClearValue,0,sizeof(colorClearValue));
      std::memset(drawBuffers,GL_NONE,sizeof(drawBuffers));
    }

    bool defined() const
    {
      return valid;
    }

    void define(DispatchTable &dt)
    {
      if (!valid)
      {
        for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
          dt.call(&dt.glGetIntegerv)(GL_DRAW_BUFFER0+ii, reinterpret_cast<GLint*>(&drawBuffers[ii]));
        valid = true;
      }
    }

    inline ColorBuffer &swap(ColorBuffer &other)
    {
      std::swap(clampFragmentColor,other.clampFragmentColor);
      std::swap(clampReadColor,other.clampReadColor);
      std::swap(alphaTest,other.alphaTest);
      std::swap(alphaTestFunc,other.alphaTestFunc);
      std::swap(alphaTestRef,other.alphaTestRef);
      std::swap_ranges(blend,blend+REGAL_MAX_DRAW_BUFFERS,other.blend);
      std::swap_ranges(blendSrcRgb,blendSrcRgb+REGAL_MAX_DRAW_BUFFERS,other.blendSrcRgb);
      std::swap_ranges(blendSrcAlpha,blendSrcAlpha+REGAL_MAX_DRAW_BUFFERS,other.blendSrcAlpha);
      std::swap_ranges(blendDstRgb,blendDstRgb+REGAL_MAX_DRAW_BUFFERS,other.blendDstRgb);
      std::swap_ranges(blendDstAlpha,blendDstAlpha+REGAL_MAX_DRAW_BUFFERS,other.blendDstAlpha);
      std::swap_ranges(blendEquationRgb,blendEquationRgb+REGAL_MAX_DRAW_BUFFERS,other.blendEquationRgb);
      std::swap_ranges(blendEquationAlpha,blendEquationAlpha+REGAL_MAX_DRAW_BUFFERS,other.blendEquationAlpha);
      std::swap_ranges(blendColor,blendColor+4,other.blendColor);
      std::swap(framebufferSRGB,other.framebufferSRGB);
      std::swap(dither,other.dither);
      std::swap(indexLogicOp,other.indexLogicOp);
      std::swap(colorLogicOp,other.colorLogicOp);
      std::swap(logicOpMode,other.logicOpMode);
      std::swap(indexWritemask,other.indexWritemask);
      std::swap_ranges(&colorWritemask[0][0],&colorWritemask[0][0]+(REGAL_MAX_DRAW_BUFFERS*4),&other.colorWritemask[0][0]);
      std::swap_ranges(colorClearValue,colorClearValue+4,other.colorClearValue);
      std::swap(indexClearValue,other.indexClearValue);
      std::swap_ranges(drawBuffers,drawBuffers+REGAL_MAX_DRAW_BUFFERS,other.drawBuffers);
      return *this;
    }

    inline ColorBuffer &get(DispatchTable &dt)
    {
      dt.call(&dt.glGetIntegerv)(GL_CLAMP_FRAGMENT_COLOR,reinterpret_cast<GLint*>(&clampFragmentColor));
      dt.call(&dt.glGetIntegerv)(GL_CLAMP_READ_COLOR,reinterpret_cast<GLint*>(&clampReadColor));
      alphaTest = dt.call(&dt.glIsEnabled)(GL_ALPHA_TEST);
      dt.call(&dt.glGetIntegerv)(GL_ALPHA_TEST_FUNC,reinterpret_cast<GLint*>(&alphaTestFunc));
      dt.call(&dt.glGetFloatv)(GL_ALPHA_TEST_REF,&alphaTestRef);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        blend[ii] = dt.call(&dt.glIsEnabledi)(GL_BLEND, ii);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
          dt.call(&dt.glGetIntegeri_v)(GL_BLEND_SRC_RGB, ii, reinterpret_cast<GLint*>(&blendSrcRgb[ii]));
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
          dt.call(&dt.glGetIntegeri_v)(GL_BLEND_SRC_ALPHA, ii, reinterpret_cast<GLint*>(&blendSrcAlpha[ii]));
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
          dt.call(&dt.glGetIntegeri_v)(GL_BLEND_DST_RGB, ii, reinterpret_cast<GLint*>(&blendDstRgb[ii]));
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
          dt.call(&dt.glGetIntegeri_v)(GL_BLEND_DST_ALPHA, ii, reinterpret_cast<GLint*>(&blendDstAlpha[ii]));
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
          dt.call(&dt.glGetIntegeri_v)(GL_BLEND_EQUATION_RGB, ii, reinterpret_cast<GLint*>(&blendEquationRgb[ii]));
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
          dt.call(&dt.glGetIntegeri_v)(GL_BLEND_EQUATION_ALPHA, ii, reinterpret_cast<GLint*>(&blendEquationAlpha[ii]));
      dt.call(&dt.glGetFloatv)(GL_BLEND_COLOR,blendColor);
      framebufferSRGB = dt.call(&dt.glIsEnabled)(GL_FRAMEBUFFER_SRGB);
      dither = dt.call(&dt.glIsEnabled)(GL_DITHER);
      indexLogicOp = dt.call(&dt.glIsEnabled)(GL_INDEX_LOGIC_OP);
      colorLogicOp  = dt.call(&dt.glIsEnabled)(GL_COLOR_LOGIC_OP);
      dt.call(&dt.glGetIntegerv)(GL_LOGIC_OP_MODE, reinterpret_cast<GLint*>(&logicOpMode));
      dt.call(&dt.glGetIntegerv)(GL_INDEX_WRITEMASK, reinterpret_cast<GLint*>(&indexWritemask));
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        dt.call(&dt.glGetBooleani_v)(GL_COLOR_WRITEMASK, ii, &colorWritemask[ii][0]);
      dt.call(&dt.glGetFloatv)(GL_COLOR_CLEAR_VALUE,colorClearValue);
      dt.call(&dt.glGetFloatv)(GL_INDEX_CLEAR_VALUE,&indexClearValue);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        dt.call(&dt.glGetIntegerv)(GL_DRAW_BUFFER0+ii, reinterpret_cast<GLint*>(&drawBuffers[ii]));
      return *this;
    }

    inline const ColorBuffer &set(DispatchTable &dt) const
    {
      dt.call(&dt.glClampColor)(GL_CLAMP_FRAGMENT_COLOR,clampFragmentColor);
      dt.call(&dt.glClampColor)(GL_CLAMP_READ_COLOR,clampReadColor);
      setEnable(dt,GL_ALPHA_TEST,alphaTest);
      dt.call(&dt.glAlphaFunc)(alphaTestFunc,alphaTestRef);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        setEnablei(dt,GL_BLEND,ii,blend[ii]);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        dt.call(&dt.glBlendFuncSeparatei)(ii,blendSrcRgb[ii],blendSrcAlpha[ii],blendDstRgb[ii],blendDstAlpha[ii]);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        dt.call(&dt.glBlendEquationSeparatei)(ii,blendEquationRgb[ii],blendEquationAlpha[ii]);
      dt.call(&dt.glBlendColor)(blendColor[0],blendColor[1],blendColor[2],blendColor[3]);
      setEnable(dt,GL_FRAMEBUFFER_SRGB,framebufferSRGB);
      setEnable(dt,GL_DITHER,dither);
      setEnable(dt,GL_INDEX_LOGIC_OP,indexLogicOp);
      setEnable(dt,GL_COLOR_LOGIC_OP,colorLogicOp);
      dt.call(&dt.glLogicOp)(logicOpMode);
      dt.call(&dt.glIndexMask)(indexWritemask);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        dt.call(&dt.glColorMaski)(ii, colorWritemask[ii][0], colorWritemask[ii][1], colorWritemask[ii][2], colorWritemask[ii][3]);
      dt.call(&dt.glClearColor)(colorClearValue[0],colorClearValue[1],colorClearValue[2],colorClearValue[3]);
      dt.call(&dt.glClearIndex)(indexClearValue);
      dt.call(&dt.glDrawBuffers)(REGAL_MAX_DRAW_BUFFERS, drawBuffers);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glClampColor(GL_CLAMP_FRAGMENT_COLOR",Token::toString(clampFragmentColor),");",delim);
      tmp << print_string("glClampColor(GL_CLAMP_READ_COLOR",Token::toString(clampReadColor),");",delim);
      enableToString(tmp, alphaTest, "GL_ALPHA_TEST",delim);
      tmp << print_string("glAlphaFunc(Token::toString(alphaTestFunc),",alphaTestRef,");",delim);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        enableiToString(tmp, blend[ii], "GL_BLEND", ii,delim);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        tmp << print_string("glBlendFuncSeparatei(",ii,",",Token::toString(blendSrcRgb[ii]),",",Token::toString(blendSrcAlpha[ii]),",",Token::toString(blendDstRgb[ii]),",",Token::toString(blendDstAlpha[ii]),delim);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        tmp << print_string("glBlendEquationSeparatei(",ii,",",Token::toString(blendEquationRgb[ii]),",",Token::toString(blendEquationAlpha[ii]),delim);
      tmp << print_string("glBlendColor(",blendColor[0],",",blendColor[1],",",blendColor[2],",",blendColor[3],");",delim);
      enableToString(tmp, framebufferSRGB, "GL_FRAMEBUFFER_SRGB",delim);
      enableToString(tmp, dither, "GL_DITHER",delim);
      enableToString(tmp, indexLogicOp, "GL_INDEX_LOGIC_OP",delim);
      enableToString(tmp, colorLogicOp, "GL_COLOR_LOGIC_OP",delim);
      tmp << print_string("glLogicOp(",Token::toString(logicOpMode),");",delim);
      tmp << print_string("glIndexMask(0x",hex(indexWritemask),");",delim);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        tmp << print_string("glColorMaski(",ii,",", colorWritemask[ii][0],",",colorWritemask[ii][1],",",colorWritemask[ii][2],",",colorWritemask[ii][3],");",delim);
      tmp << print_string("glClearColor(",colorClearValue[0],",",colorClearValue[1],",",colorClearValue[2],",",colorClearValue[3],");",delim);
      tmp << print_string("glClearIndex(",indexClearValue,");",delim);
      tmp << print_string("glDrawBuffers(",REGAL_MAX_DRAW_BUFFERS,", [");
      for (int ii=0; ii<REGAL_MAX_DRAW_BUFFERS-1; ii++)
        tmp << print_string(" ",Token::toString(drawBuffers[ii]),",");
      tmp << print_string(" ",Token::toString(drawBuffers[REGAL_MAX_DRAW_BUFFERS-1]),"]);",delim);
      return tmp;
    }
  };
}

REGAL_NAMESPACE_END

#endif
