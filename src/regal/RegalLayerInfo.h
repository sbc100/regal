/*
  Copyright (c) 2011-2013 NVIDIA Corporation
  Copyright (c) 2011-2013 Cass Everitt
  Copyright (c) 2013 Scott Nations
  Copyright (c) 2013 Mathias Schott
  Copyright (c) 2013 Nigel Stewart
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

 Regal Emulation layer enable logic
 Cass Everitt

 */

#ifndef __REGAL_LAYER_INFO_H__
#define __REGAL_LAYER_INFO_H__

#include "RegalUtil.h"

#if REGAL_EMULATION

REGAL_GLOBAL_BEGIN

#include "RegalContext.h"
#include "RegalEmuInfo.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

namespace Emu
{

  struct LayerInfo
  {
    LayerInfo() : emulationNeeded( false ), emulationSupported( false ) {}
    bool emulationNeeded;
    bool emulationSupported;
  };

  inline void GetBaseVertexLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    bool from_core = ctx.info->gl_version_3_2;
    bool from_ext = ctx.info->gl_arb_draw_elements_base_vertex;
    layer.emulationNeeded = from_core == false && from_ext == false;
    layer.emulationSupported = true;
  }

  inline void SetBaseVertexEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    if( enable || layer.emulationNeeded == false ) {
      emuInfo->gl_arb_vertex_array_object = GL_TRUE;
    }
  }

  // FIXME: set correctly
  inline void GetBinLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    UNUSED_PARAMETER( ctx );
    layer = LayerInfo();
    // for now this layer can only be forced on
    layer.emulationNeeded = false;
    layer.emulationSupported = true;
  }

  inline void SetBinEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  // FIXME: set correctly
  inline void GetDsaLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    layer.emulationNeeded = ctx.info->gl_ext_direct_state_access == false;
    layer.emulationSupported = true;
  }

  inline void SetDsaEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    if( enable || layer.emulationNeeded == false ) {
      emuInfo->gl_ext_direct_state_access = GL_TRUE;
    }
  }

  inline void GetFilterLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    UNUSED_PARAMETER( ctx );
    layer = LayerInfo();
    layer.emulationNeeded = true;
    layer.emulationSupported = true;
  }

  inline void SetFilterEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  // FIXME: set correctly
  inline void GetHintLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    layer.emulationNeeded = ctx.info->es2;
    layer.emulationSupported = true;
  }

  inline void SetHintEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  inline void GetIffLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    layer.emulationNeeded = ctx.info->es2 || ctx.info->core;
    bool has_glsl = (   ctx.info->gl_version_major >= 2
                    || ctx.info->gles_version_major >= 2 );
    bool has_vao = ctx.emuInfo->gl_arb_vertex_array_object;
    layer.emulationSupported = has_glsl && has_vao;
  }

  inline void SetIffEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  inline void GetObjLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    UNUSED_PARAMETER( ctx );
    layer = LayerInfo();
    // For some future resolution:
    // the "true" in emulationNeeded is because this layer isolates the app object
    // namespace from the regal internal one...
    // if we had a ctx.info->emu_obj_layer_required, lower layers could set it if they needed it
    layer.emulationNeeded = true || ctx.info->es2 || ctx.info->core;
    layer.emulationSupported = true;
  }

  inline void SetObjEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  /*
  // FIXME: mjk to set "emulationSupported" correctly
  inline void GetPathLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    layer.emulationNeeded = ctx.info->gl_nv_path_rendering == false;
    // FIXME: mjk to set "emulationSupported" correctly
    layer.emulationSupported = ctx.info->gl_ext_texture_buffer_object;
  }
  */

  // FIXME: set correctly
  inline void GetPpaLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    UNUSED_PARAMETER( ctx );
    layer = LayerInfo();
    // Need ppca layer with core to track fixed function state
    // layer.emulationNeeded = ctx.info->es1 || ctx.info->es2 || ctx.info->core;
    layer.emulationNeeded = true; // because we need to honor glGet for all known state
    layer.emulationSupported = true;
  }

  inline void SetPpaEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  // FIXME: set correctly
  inline void GetPpcaLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    // Need ppca layer with core to track fixed function state
    layer.emulationNeeded = ctx.info->es1 || ctx.info->es2 || ctx.info->core;
    layer.emulationSupported = true;
  }

  inline void SetPpcaEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  // FIXME: set correctly
  inline void GetQuadsLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    // Need ppca layer with core to track fixed function state
    layer.emulationNeeded = ctx.info->es1 || ctx.info->es2 || ctx.info->core;
    layer.emulationSupported = true;
  }

  inline void SetQuadsEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  // FIXME: set correctly
  inline void GetRectLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    layer.emulationNeeded = ctx.info->es1 || ctx.info->es2 || ctx.info->core;
    layer.emulationSupported = true;
  }

  inline void SetRectEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  inline void GetSoLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    bool from_core = ctx.info->gl_version_3_3 == GL_TRUE || ctx.info->gles_version_major >= 3;
    bool from_ext = ctx.info->gl_arb_sampler_objects == GL_TRUE;
    layer.emulationNeeded = ! ( from_core || from_ext );
    layer.emulationSupported = true;
  }

  inline void SetSoEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    if( enable || layer.emulationNeeded == false ) {
      emuInfo->gl_arb_sampler_objects = GL_TRUE;
    }
  }

  inline void GetTexCLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    layer.emulationNeeded = ctx.info->es2;
    layer.emulationSupported = true;
  }

  inline void SetTexCEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  inline void GetTexStoLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    bool from_core = ctx.info->gl_version_4_2;
    bool from_ext = ctx.info->gl_arb_texture_storage || ctx.info->gl_ext_texture_storage;
    layer.emulationNeeded = from_core == false && from_ext == false;
    layer.emulationSupported = true;
  }

  inline void SetTexStoEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  inline void GetVaoLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    layer.emulationSupported = true;
    bool from_core = (   ctx.info->gl_version_major >= 3
                      || ctx.info->gles_version_major >= 3 );
    bool from_ext = ctx.info->gl_arb_vertex_array_object;
    // need to handle the default vao case, which core profile doesn't handle...
    layer.emulationNeeded = ctx.info->core || ( from_core == false && from_ext == false );
  }

  inline void SetVaoEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    if( enable || layer.emulationNeeded == false ) {
      emuInfo->gl_arb_vertex_array_object = GL_TRUE;
    }
  }

  inline void GetXferLayerInfo( RegalContext &ctx, LayerInfo &layer ) {
    layer = LayerInfo();
    layer.emulationNeeded = ctx.info->es2;
    layer.emulationSupported = true;
  }

  inline void SetXferEmuInfo( bool enable, EmuInfo *emuInfo, LayerInfo &layer ) {
    UNUSED_PARAMETER( emuInfo );
    if( enable || layer.emulationNeeded == false ) {
      // set some sort of enable
    }
  }

  extern bool validTextureEnum(GLenum texture);
  extern bool validTextureUnit(GLuint unit);
}

REGAL_NAMESPACE_END

#endif // REGAL_EMULATION

#endif // ! __REGAL_LAYER_INFO_H__

