/*
  Copyright (c) 2011-2013 NVIDIA Corporation
  Copyright (c) 2011-2012 Cass Everitt
  Copyright (c) 2012-2013 Nigel Stewart
  Copyright (c) 2012 Scott Nations
  Copyright (c) 2012 Mathias Schott
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

 Regal Vertex Array Object implementation
 Cass Everitt

 */

#ifndef __REGAL_VAO_H__
#define __REGAL_VAO_H__

#include "RegalUtil.h"

REGAL_GLOBAL_BEGIN

#include <string>

#include "RegalEmu.h"
#include "RegalEmuInfo.h"
#include "RegalContext.h"
#include "RegalContextInfo.h"
#include "RegalSharedMap.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

namespace Emu
{

struct Vao
{

  struct DrawElementsIndirectCommand
  {
    GLuint count;
    GLuint primCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
  };

  struct DrawArraysIndirectCommand
  {
    GLuint count;
    GLuint primCount;
    GLuint first;
    GLuint baseInstance;
  };

  struct PointerCount
  {
      GLuint _count;
      GLuint64 _ptr;

      bool operator() (const PointerCount& i, const PointerCount& j) { return i._ptr < j._ptr; };
  };

  struct Array
  {

    Array()
      : enabled( GL_FALSE )
      , buffer( 0 )
      , shadow_buffer( 0 )
      , size( 4 )
      , type( GL_FLOAT )
      , normalized( GL_FALSE )
      , integer( GL_FALSE )
      , stride( 0 )
      , pointer( NULL )
      , shadow_pointer( NULL )
    {}

    GLboolean enabled;
    GLuint buffer;
    GLuint shadow_buffer;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLboolean integer;
    GLsizei stride;
    const GLvoid *pointer;
    const GLvoid *shadow_pointer;
  };

  struct Object
  {
    Object() : vertexBuffer( 0 ), indexBuffer( 0 ) {}
    GLuint vertexBuffer;
    GLuint indexBuffer;
    Array a[ REGAL_EMU_MAX_VERTEX_ATTRIBS ];
  };

  shared_map<GLuint, Object> objects;

  GLenum clientActiveTexture;

  GLuint current;
  GLuint enables;
  Object *currObject;

  GLuint coreVao;
  GLuint maxName;

  // to alias vertex arrays to generic attribs
  GLuint ffAttrMap[ REGAL_EMU_MAX_VERTEX_ATTRIBS ];
  GLuint ffAttrInvMap[ REGAL_EMU_MAX_VERTEX_ATTRIBS ];
  GLuint ffAttrTexBegin;
  GLuint ffAttrTexEnd;
  GLuint ffAttrNumTex;
  GLuint max_vertex_attribs;

  void Init( RegalContext &ctx )
  {
    maxName = 0;
    clientActiveTexture = GL_TEXTURE0;

    max_vertex_attribs = ctx.emuInfo->gl_max_vertex_attribs;
    RegalAssert( max_vertex_attribs <= REGAL_EMU_MAX_VERTEX_ATTRIBS );
    if (max_vertex_attribs > REGAL_EMU_MAX_VERTEX_ATTRIBS)
      max_vertex_attribs = REGAL_EMU_MAX_VERTEX_ATTRIBS;

    RegalContext *sharingWith = ctx.shareGroup->front();
    if (sharingWith)
      objects = sharingWith->vao->objects;

    // we have RFF2A maps for sets of 8 and 16 attributes. if
    // REGAL_EMU_MAX_VERTEX_ATTRIBS > 16 a new map needs to be added

    RegalAssert( REGAL_EMU_MAX_VERTEX_ATTRIBS <= 16 );

    if ( max_vertex_attribs >= 16 )
    {
      RegalAssert( REGAL_EMU_MAX_VERTEX_ATTRIBS == 16);
      //RegalOutput( "Setting up for %d Vertex Attribs\n", max_vertex_attribs );
      for( int i = 0; i < 16; i++ )
      {
        ffAttrMap[i] = RFF2AMap16[i];
        ffAttrInvMap[i] = RFF2AInvMap16[i];
      }
      ffAttrTexBegin = RFF2ATexBegin16;
      ffAttrTexEnd = RFF2ATexEnd16;
      if (max_vertex_attribs > 16)
        max_vertex_attribs = 16;
    }
    else
    {
      RegalAssert( max_vertex_attribs >= 8 );
      //RegalOutput( "Setting up for 8 Vertex Attribs" );
      for( int i = 0; i < 8; i++ )
      {
        ffAttrMap[i] = RFF2AMap8[i];
        ffAttrInvMap[i] = RFF2AInvMap8[i];
      }
      for( int i = 8; i < REGAL_EMU_MAX_VERTEX_ATTRIBS; i++ )
      {
        ffAttrMap[i] = GLuint(-1);
        ffAttrInvMap[i] = GLuint(-1);
      }
      ffAttrTexBegin = RFF2ATexBegin8;
      ffAttrTexEnd = RFF2ATexEnd8;
      if (max_vertex_attribs > 8)
        max_vertex_attribs = 8;
    }
    ffAttrNumTex = ffAttrTexEnd - ffAttrTexBegin;

    if( ctx.info->core )
    {
      maxName = 1;
      ctx.dispatcher.driver.glGenVertexArrays( 1, & coreVao );
      RegalAssert( coreVao != 0 );
      ctx.dispatcher.driver.glBindVertexArray( coreVao );
    }
    else
      coreVao = 0;

    current = 9999999; // this is only to force the bind...
    currObject = NULL;
    BindVertexArray( ctx, 0 );

    if( ctx.info->gl_arb_vertex_array_object == false ) {
      ctx.info->gl_arb_vertex_array_object = true;
    }

    if( ctx.emuInfo->extensionsSet.count( "GL_ARB_vertex_array_object" ) == 0 ) {
      ctx.emuInfo->extensionsSet.insert( "GL_ARB_vertex_array_object" );
    }

  }

  void Cleanup( RegalContext &ctx )
  {
    UNUSED_PARAMETER(ctx);
  }

  void ShadowBufferBinding( GLenum target, GLuint bufferBinding )
  {
    RegalAssert( currObject != NULL );
    if( target == GL_ARRAY_BUFFER )
    {
      currObject->vertexBuffer = bufferBinding;
    }
    else if( target == GL_ELEMENT_ARRAY_BUFFER )
    {
      currObject->indexBuffer = bufferBinding;
    }
  }

  void BindVertexArray( RegalContext &ctx, GLuint name )
  {
    if( name == current )
    {
      return;
    }

    current = name;

    Object &vao = objects[current];  // force VAO construction
    currObject = & vao;
    if( maxName < current )
    {
      maxName = current;
    }
    DispatchTableGL &tbl = ctx.dispatcher.emulation;
    tbl.glBindBuffer( GL_ARRAY_BUFFER, vao.vertexBuffer );
    tbl.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vao.indexBuffer );
    GLuint lastBuffer = vao.vertexBuffer;
    RegalAssert( max_vertex_attribs <= REGAL_EMU_MAX_VERTEX_ATTRIBS );
    for( GLuint i = 0; i < max_vertex_attribs; i++ )
    {
      Array &a = vao.a[ i ];
      if( a.buffer != lastBuffer )
      {
        tbl.glBindBuffer( GL_ARRAY_BUFFER, a.buffer );
        lastBuffer = a.buffer;
      }

      EnableDisableVertexAttribArray( ctx, a.enabled, i );
      tbl.glVertexAttribPointer( i, a.size, a.type, a.normalized, a.stride, a.pointer );
    }
    if( lastBuffer != vao.vertexBuffer )
    {
      tbl.glBindBuffer( GL_ARRAY_BUFFER, vao.vertexBuffer );
    }
  }

  void GenVertexArrays( GLsizei n, GLuint *arrays )
  {
    if( maxName < 0x80000000 )   // fast gen for sequential allocation
    {
      for( GLsizei i = 0; i < n; i++ )
      {
        arrays[ i ] = ++maxName;
        objects[ maxName ]; // gen allocates
      }
    }
    else     // otherwise plod through
    {
      GLsizei i = 0;
      GLuint name = 1;
      while( i < n )
      {
        if( objects.count( name ) == 0 )
        {
          arrays[ i++ ] = name;
          objects[ name ]; // gen allocates
        }
        name++;
      }
    }
  }

  void DeleteVertexArrays( RegalContext &ctx, GLsizei n, const GLuint *arrays )
  {
    for( GLsizei i = 0; i < n; i++ )
    {
      GLuint name = arrays[ i ];
      if( name != coreVao && objects.count( name ) > 0 )
      {
        GLuint bos[ REGAL_EMU_MAX_VERTEX_ATTRIBS ];
        GLuint count = 0;
        for ( GLsizei j = 0; j < REGAL_EMU_MAX_VERTEX_ATTRIBS; j++ )
        {
          GLuint b = objects[name].a[j].shadow_buffer;
          if (b)
            bos[count++] = b;
        }
        if (count)
          ctx.dispatcher.driver.glDeleteBuffers( count, bos );
        objects.erase( name );
      }
    }
  }

  GLboolean IsVertexArray( GLuint name )
  {
    return objects.count( name ) > 0 ? GL_TRUE : GL_FALSE;
  }

  void EnableDisableVertexAttribArray( RegalContext &ctx, GLboolean enable, GLuint index )
  {
    RegalAssert( index < REGAL_EMU_MAX_VERTEX_ATTRIBS );
    RegalAssert( index < max_vertex_attribs );
    if (index >= max_vertex_attribs || index >= REGAL_EMU_MAX_VERTEX_ATTRIBS)
      return;

    DispatchTableGL &tbl = ctx.dispatcher.emulation;
    Array &a = objects[current].a[index];
    a.enabled = enable;
    if( a.enabled == GL_TRUE )
    {
      tbl.glEnableVertexAttribArray( index );
      enables |= 1 << index;
    }
    else
    {
      tbl.glDisableVertexAttribArray( index );
      enables &= ~( 1 << index );
    }
  }

  void EnableVertexAttribArray( RegalContext &ctx, GLuint index )
  {
    EnableDisableVertexAttribArray( ctx, GL_TRUE, index );
  }

  void DisableVertexAttribArray( RegalContext &ctx, GLuint index )
  {
    EnableDisableVertexAttribArray( ctx, GL_FALSE, index );
  }

  void AttribPointer( RegalContext &ctx, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer )
  {
    // do nothing for these various error conditions

    if ( ctx.depthBeginEnd )
      return;

    RegalAssert( index < REGAL_EMU_MAX_VERTEX_ATTRIBS );
    RegalAssert( index < max_vertex_attribs );
    if (index >= max_vertex_attribs || index >= REGAL_EMU_MAX_VERTEX_ATTRIBS )
      return;

    switch (size)
    {
      case 1:
      case 2:
      case 3:
      case 4:
        break;
      case GL_BGRA:
        switch (type)
        {
          case GL_UNSIGNED_BYTE:
          case GL_INT_2_10_10_10_REV:
          case GL_UNSIGNED_INT_2_10_10_10_REV:
            break;
          default:
            return;
        }
        break;
      default:
        return;
    }

    switch (type)
    {
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_HALF_FLOAT:
      case GL_FLOAT:
      case GL_DOUBLE:
        break;
      case GL_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
        if (size != 4 && size != GL_BGRA)
          return;
        break;
      default:
        RegalAssert( "Unknown <type> in for attrib." );
        return;
    }

    if (size == GL_BGRA && normalized == GL_FALSE)
    {
      return;
    }

    if (stride < 0)
    {
      return;
    }

    //<> if ( ( currentClientState.vertexArrayState.vertexArrayObject != 0 ) &&
    //<> ( currentClientState.vertexArrayState.vertexBuffer == 0 ) &&
    //<> ( pointer != NULL ) ) {
    //<> return;
    //<> }

    RegalAssert( index < max_vertex_attribs );
    RegalAssert( index < REGAL_EMU_MAX_VERTEX_ATTRIBS );
    Array &a = objects[current].a[index];

    RegalAssert( currObject != NULL );
    RegalAssert( currObject->vertexBuffer == 0 || GLuint64( pointer ) < ( 1 << 22 ) );

    if ( currObject->vertexBuffer == 0 && !pointer )
      return;
      
    if(currObject->vertexBuffer == 0)
    {
      if (a.shadow_buffer == 0)
          ctx.dispatcher.driver.glGenBuffers( 1, &a.shadow_buffer );
      ctx.dispatcher.driver.glBindBuffer( GL_ARRAY_BUFFER, a.shadow_buffer );
    }

    a.size = size;
    a.type = type;
    a.normalized = normalized;
    a.stride = stride;

    if(currObject->vertexBuffer == 0)
    {
      a.buffer = a.shadow_buffer;
      a.pointer = 0;
      a.shadow_pointer = pointer;
    }
    else
    {
      a.buffer = currObject->vertexBuffer;
      a.pointer = pointer;
      a.shadow_pointer = 0;
    }
      
    ctx.dispatcher.emulation.glVertexAttribPointer( index, size, type, normalized, stride, a.pointer );

    if (currObject->vertexBuffer == 0)
      ctx.dispatcher.driver.glBindBuffer( GL_ARRAY_BUFFER, 0 );
  }

  void Validate( RegalContext &ctx )
  {
    UNUSED_PARAMETER(ctx);
    RegalAssert( currObject != NULL );
    RegalAssert( max_vertex_attribs <= REGAL_EMU_MAX_VERTEX_ATTRIBS );
    for( GLuint i = 0; i < max_vertex_attribs; i++ )
    {
#if !REGAL_NO_ASSERT
      const Array &a = currObject->a[ i ];
      RegalAssert( !( a.enabled && a.buffer == 0 && GLuint64( a.pointer ) < 1024 ) );
#endif
    }
  }

  template <typename T> void GetAttrib( GLint index, GLenum pname, T *params )
  {
    if (index >= REGAL_EMU_MAX_VERTEX_ATTRIBS)
      return;

    Array &a = objects[current].a[index];
    switch( pname )
    {
      case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
        *params = static_cast<T>(a.enabled);
        break;
      case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
        *params = static_cast<T>(a.buffer);
        break;
      case GL_VERTEX_ATTRIB_ARRAY_SIZE:
        *params = static_cast<T>(a.size);
        break;
      case GL_VERTEX_ATTRIB_ARRAY_TYPE:
        *params = static_cast<T>(a.type);
        break;
      case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
        *params = static_cast<T>(a.normalized);
        break;
      case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
        *params = static_cast<T>(a.stride);
        break;
      case GL_VERTEX_ATTRIB_ARRAY_DIVISOR:
        *params = static_cast<T>(0);
      default:
        break;
    }
  }

  template <typename T> void GetAttrib( GLint index, GLenum pname, T **params )
  {
    if (index >= REGAL_EMU_MAX_VERTEX_ATTRIBS)
      return;

    Array &a = objects[current].a[index];
    switch( pname )
    {
      case GL_VERTEX_ATTRIB_ARRAY_POINTER:
        *params = const_cast<T *>(a.pointer);
        break;
      default:
        break;
    }
  }

  template <typename T> bool Get( GLenum pname, T *params )
  {

    switch (pname)
    {
      case GL_VERTEX_ARRAY_BINDING:
        *params = static_cast<T>( current );
        break;

      case GL_ARRAY_BUFFER_BINDING:
        *params = static_cast<T>(currObject->vertexBuffer);
        break;

      case GL_ELEMENT_ARRAY_BUFFER_BINDING:
        *params = static_cast<T>(currObject->indexBuffer);
        break;

      default:
        return false;
    }
    return true;
  }

  bool GetVertexAttribPointerv( GLuint index, GLenum pname, GLvoid **pointer)
  {
    if (index >= REGAL_EMU_MAX_VERTEX_ATTRIBS)
      return false;

    if ( pname != GL_VERTEX_ATTRIB_ARRAY_POINTER )
      return false;

    Array &a = objects[current].a[index];
    switch( pname )
    {
      case GL_VERTEX_ATTRIB_ARRAY_POINTER:
        *pointer = const_cast<GLvoid *>(a.pointer);
        break;
      default:
        return false;
    }
    return true;
  }

  GLuint ClientStateToAttribIndex( GLenum array )
  {
    switch ( array )
    {
      case GL_VERTEX_ARRAY:
        return ffAttrMap[ RFF2A_Vertex ];
      case GL_NORMAL_ARRAY:
        return ffAttrMap[ RFF2A_Normal ];
      case GL_COLOR_ARRAY:
        return ffAttrMap[ RFF2A_Color ];
      case GL_SECONDARY_COLOR_ARRAY:
        return ffAttrMap[ RFF2A_SecondaryColor ];
      case GL_FOG_COORD_ARRAY:
        return ffAttrMap[ RFF2A_FogCoord ];
      case GL_TEXTURE_COORD_ARRAY:
      {
        GLuint index = clientActiveTexture - GL_TEXTURE0;
        RegalAssert(index < REGAL_EMU_MAX_VERTEX_ATTRIBS);
        if ( index >= ffAttrNumTex )
        {
          Warning("Texture unit out of range: ", index, " >= ", ffAttrNumTex, ". Clamping to supported maximum.");
          index = ffAttrNumTex - 1;
        }
        return ffAttrTexBegin + index;
      }
      break;
      default:
        break;
    }
    return GLuint(~0);
  }

  void ShadowVertexArrayPointer( RegalContext &ctx, GLenum array, GLint size,
                                 GLenum type, GLsizei stride, const GLvoid *pointer)
  {
    //<> if ( ( currentClientState.vertexArrayState.vertexArrayObject != 0 ) &&
    //<> ( currentClientState.vertexArrayState.vertexBuffer == 0 ) &&
    //<> ( pointer != NULL ) ) {
    //<> return;
    //<> }

    GLuint index = ClientStateToAttribIndex( array );
    AttribPointer( ctx, index, size, type, GL_TRUE, stride, pointer );
  }

  void ColorPointer(RegalContext &ctx, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
  {
    if ( ctx.depthBeginEnd )
      return;

    switch (size)
    {
      case 3:
      case 4:
        break;
      default:
        return;
    }

    switch (type)
    {
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FLOAT:
      case GL_DOUBLE:
        break;
      default:
        return;
    }

    if (stride < 0)
      return;

    ShadowVertexArrayPointer(ctx, GL_COLOR_ARRAY, size, type, stride, pointer);
  }

  void SecondaryColorPointer(RegalContext &ctx, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
  {
    if ( ctx.depthBeginEnd )
      return;

    if (size != 3)
      return;

    switch (type)
    {
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FLOAT:
      case GL_DOUBLE:
        break;
      default:
        return;
    }

    if (stride < 0)
      return;

    ShadowVertexArrayPointer(ctx, GL_SECONDARY_COLOR_ARRAY, size, type, stride, pointer);
  }

  void TexCoordPointer(RegalContext &ctx, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
  {
    if ( ctx.depthBeginEnd )
      return;

    switch (size)
    {
      case 1:
      case 2:
      case 3:
      case 4:
        break;
      default:
        return;
    }

    switch (type)
    {
      case GL_SHORT:
      case GL_INT:
      case GL_FLOAT:
      case GL_DOUBLE:
        break;
      default:
        return;
    }

    if (stride < 0)
      return;

    ShadowVertexArrayPointer(ctx, GL_TEXTURE_COORD_ARRAY, size, type, stride, pointer);
  }

  void VertexPointer(RegalContext &ctx, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
  {
    if ( ctx.depthBeginEnd )
      return;

    switch (size)
    {
      case 2:
      case 3:
      case 4:
        break;
      default:
        return;
    }

    switch (type)
    {
      case GL_SHORT:
      case GL_INT:
      case GL_FLOAT:
      case GL_DOUBLE:
        break;
      default:
        return;
    }

    if (stride < 0)
      return;

    ShadowVertexArrayPointer(ctx, GL_VERTEX_ARRAY, size, type, stride, pointer);
  }

  void NormalPointer(RegalContext &ctx, GLenum type, GLsizei stride, const GLvoid *pointer)
  {
    if ( ctx.depthBeginEnd )
      return;

    switch (type)
    {
      case GL_BYTE:
      case GL_SHORT:
      case GL_INT:
      case GL_FLOAT:
      case GL_DOUBLE:
        break;
      default:
        return;
    }

    if (stride < 0)
      return;

    ShadowVertexArrayPointer(ctx, GL_NORMAL_ARRAY, 3, type, stride, pointer);
  }

  void FogCoordPointer(RegalContext &ctx, GLenum type, GLsizei stride, const GLvoid *pointer)
  {
    if ( ctx.depthBeginEnd )
      return;

    switch (type)
    {
      case GL_FLOAT:
      case GL_DOUBLE:
        break;
      default:
        return;
    }

    if (stride < 0)
      return;

    ShadowVertexArrayPointer(ctx, GL_FOG_COORD_ARRAY, 0, type, stride, pointer);
  }

  void ClientActiveTexture( RegalContext &ctx, GLenum _texture)
  {
    if ( ctx.depthBeginEnd )
      return;

    GLint index = _texture - GL_TEXTURE0;

    if (index >= 0 && index < REGAL_EMU_MAX_VERTEX_ATTRIBS)
      clientActiveTexture = _texture;
  }

  void ShadowEnableDisableClientState( RegalContext &ctx, GLenum array, GLboolean enable )
  {
    GLuint index = ClientStateToAttribIndex( array );
    EnableDisableVertexAttribArray( ctx, enable, index );
  }

  void EnableClientState( RegalContext &ctx, GLenum array )
  {
    if ( ctx.depthBeginEnd )
      return;

    ShadowEnableDisableClientState( ctx, array, GL_TRUE );
  }

  void DisableClientState( RegalContext &ctx, GLenum array )
  {
    if ( ctx.depthBeginEnd )
      return;

    ShadowEnableDisableClientState( ctx, array, GL_FALSE );
  }

  void InterleavedArrays( RegalContext &ctx, GLenum format,
                          GLsizei stride, const GLvoid *pointer)
  {
    if ( ctx.depthBeginEnd )
      return;

    //<> how can stride be invalid?
    //<> if ( stride is invalid )
    //<> return;

    switch ( format )
    {
      case GL_V2F:
      case GL_V3F:
      case GL_C4UB_V2F:
      case GL_C4UB_V3F:
      case GL_C3F_V3F:
      case GL_N3F_V3F:
      case GL_C4F_N3F_V3F:
      case GL_T2F_V3F:
      case GL_T4F_V4F:
      case GL_T2F_C4UB_V3F:
      case GL_T2F_C3F_V3F:
      case GL_T2F_N3F_V3F:
      case GL_T2F_C4F_N3F_V3F:
      case GL_T4F_C4F_N3F_V4F:
        break;
      default:
        RegalAssert( !"unhandled format value" );
        return;
    }

    GLsizei f = sizeof(GL_FLOAT);
    GLsizei c = 4 * sizeof(GL_UNSIGNED_BYTE);
    //<> need to round up c to the nearest multiple of f

    GLsizei pc = 0;
    GLsizei pn = 0;
    GLsizei pv = 0;
    GLsizei s  = 0;

    switch ( format )
    {
      case GL_V2F:
        pc = 0;
        pn = 0;
        pv = 0;
        s  = 2 * f;
        break;
      case GL_V3F:
        pc = 0;
        pn = 0;
        pv = 0;
        s  = 3 * f;
        break;
      case GL_C4UB_V2F:
        pc = 0;
        pn = 0;
        pv = c + 0;
        s  = c + 2 * f;
        break;
      case GL_C4UB_V3F:
        pc = 0;
        pn = 0;
        pv = c + 0;
        s  = c + 3 * f;
        break;
      case GL_C3F_V3F:
        pc = 0;
        pn = 0;
        pv = 3 * f;
        s  = 6 * f;
        break;
      case GL_N3F_V3F:
        pc = 0;
        pn = 0;
        pv = 3 * f;
        s  = 6 * f;
        break;
      case GL_C4F_N3F_V3F:
        pc = 0;
        pn = 4 * f;
        pv = 7 * f;
        s  = 10;
        break;
      case GL_T2F_V3F:
        pc = 0;
        pn = 0;
        pv = 2 * f;
        s  = 5 * f;
        break;
      case GL_T4F_V4F:
        pc = 0;
        pn = 0;
        pv = 4 * f;
        s  = 8 * f;
        break;
      case GL_T2F_C4UB_V3F:
        pc = 2 * f;
        pn = 0;
        pv = c + 2 * f;
        s  = c + 5 * f;
        break;
      case GL_T2F_C3F_V3F:
        pc = 2 * f;
        pn = 0;
        pv = 5 * f;
        s  = 8 * f;
        break;
      case GL_T2F_N3F_V3F:
        pc = 0;
        pn = 2 * f;
        pv = 5 * f;
        s  = 8 * f;
        break;
      case GL_T2F_C4F_N3F_V3F:
        pc = 2 * f;
        pn = 6 * f;
        pv = 9 * f;
        s  = 12 * f;
        break;
      case GL_T4F_C4F_N3F_V4F:
        pc = 4 * f;
        pn = 8 * f;
        pv = 11 * f;
        s  = 15 * f;
        break;
      default:
        RegalAssert( !"unhandled format value" );
        break;
    }

    GLubyte *pointerc = static_cast<GLubyte *>( const_cast<GLvoid *>(pointer) ) + pc;
    GLubyte *pointern = static_cast<GLubyte *>( const_cast<GLvoid *>(pointer) ) + pn;
    GLubyte *pointerv = static_cast<GLubyte *>( const_cast<GLvoid *>(pointer) ) + pv;

    if ( stride == 0 )
    {
      stride = s;
    }

    //<> ShadowEnableDisableClientState( ctx, GL_EDGE_FLAG_ARRAY, GL_FALSE );
    //<> ShadowEnableDisableClientState( ctx, GL_INDEX_ARRAY, GL_FALSE );
    ShadowEnableDisableClientState( ctx, GL_SECONDARY_COLOR_ARRAY, GL_FALSE );
    ShadowEnableDisableClientState( ctx, GL_FOG_COORD_ARRAY, GL_FALSE );

    GLint size = 0;
    GLenum type = GL_FLOAT;

    switch ( format )
    {
      case GL_T2F_V3F:
      case GL_T4F_V4F:
      case GL_T2F_C4UB_V3F:
      case GL_T2F_C3F_V3F:
      case GL_T2F_N3F_V3F:
      case GL_T2F_C4F_N3F_V3F:
      case GL_T4F_C4F_N3F_V4F:
        size = ((format == GL_T4F_V4F) || (format == GL_T4F_C4F_N3F_V4F)) ? 4 : 2;
        ShadowEnableDisableClientState( ctx, GL_TEXTURE_COORD_ARRAY, GL_TRUE );
        TexCoordPointer(ctx, size, GL_FLOAT, stride, pointer);
        break;
      default:
        ShadowEnableDisableClientState( ctx, GL_TEXTURE_COORD_ARRAY, GL_FALSE );
        break;
    }

    switch ( format )
    {
      case GL_C4UB_V2F:
      case GL_C4UB_V3F:
      case GL_C3F_V3F:
      case GL_C4F_N3F_V3F:
      case GL_T2F_C4UB_V3F:
      case GL_T2F_C3F_V3F:
      case GL_T2F_C4F_N3F_V3F:
      case GL_T4F_C4F_N3F_V4F:
        size = ((format == GL_C3F_V3F) || (format == GL_T2F_C3F_V3F)) ? 3 : 4;
        ShadowEnableDisableClientState( ctx, GL_COLOR_ARRAY, GL_TRUE );
        ColorPointer(ctx, size, type, stride, pointerc);
        break;
      default:
        ShadowEnableDisableClientState( ctx, GL_COLOR_ARRAY, GL_FALSE );
        break;
    }

    switch ( format )
    {
      case GL_N3F_V3F:
      case GL_C4F_N3F_V3F:
      case GL_T2F_N3F_V3F:
      case GL_T2F_C4F_N3F_V3F:
      case GL_T4F_C4F_N3F_V4F:
        ShadowEnableDisableClientState( ctx, GL_NORMAL_ARRAY, GL_TRUE );
        NormalPointer(ctx, GL_FLOAT, stride, pointern);
        break;
      default:
        ShadowEnableDisableClientState( ctx, GL_NORMAL_ARRAY, GL_FALSE );
        break;
    }

    switch ( format )
    {
      case GL_V2F:
      case GL_C4UB_V2F:
        size = 2;
        break;
      case GL_V3F:
      case GL_C4UB_V3F:
      case GL_C3F_V3F:
      case GL_N3F_V3F:
      case GL_C4F_N3F_V3F:
      case GL_T2F_V3F:
      case GL_T2F_C4UB_V3F:
      case GL_T2F_C3F_V3F:
      case GL_T2F_N3F_V3F:
      case GL_T2F_C4F_N3F_V3F:
        size = 3;
        break;
      case GL_T4F_V4F:
      case GL_T4F_C4F_N3F_V4F:
        size = 4;
        break;
      default:
        RegalAssert( !"unhandled format value" );
        break;
    }

    ShadowEnableDisableClientState( ctx, GL_VERTEX_ARRAY, GL_TRUE );
    VertexPointer(ctx, size, GL_FLOAT, stride, pointerv);
  }

  GLsizei TypeSize(GLenum type)
  {
    switch(type)
    {
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
        return sizeof(GLbyte);
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_HALF_FLOAT:
        return sizeof(GLshort);
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
        return sizeof(GLint);
      case GL_FLOAT:
        return sizeof(GLfloat);
      case GL_DOUBLE:
        return sizeof(GLdouble);
      default:
        RegalAssert( "Unknown <type> in for TypeSize." );
        return 0;
    }
  }

  template<typename T> GLsizei MaxVertexCount(const GLvoid * ptr, GLuint base, GLuint count, GLuint restartIdx, GLboolean restart)
  {
    GLsizei vtxcount = 0;
    const T* indices = static_cast<const T*>(ptr);
    GLuint endo = base + count;
    if (restart)
    {
      for(GLuint i = base; i < endo; i++)
      {
        if (indices[i] == restartIdx)
          continue;
        if ((GLsizei) indices[i] > vtxcount)
          vtxcount = (GLsizei) indices[i];
      }
    } else {
      for(GLuint i = base; i < endo; i++)
      {
        if ((GLsizei) indices[i] > vtxcount)
          vtxcount = (GLsizei) indices[i];
      }
    }
    ++vtxcount;
    return vtxcount;
  }

  GLsizei MaxVertexCount(RegalContext &ctx, GLenum type, const GLvoid * ptr, GLuint base, GLuint count)
  {
    GLboolean isRestartVar = ctx.dispatcher.driver.glIsEnabled(GL_PRIMITIVE_RESTART);
    GLboolean isRestartFixed = ctx.dispatcher.driver.glIsEnabled(GL_PRIMITIVE_RESTART_FIXED_INDEX);

    GLboolean isRestart = isRestartVar || isRestartFixed;

    GLuint restartIdx = 0;
    if (isRestartFixed)
    {
      switch(type)
      {
        case GL_UNSIGNED_BYTE:
          restartIdx = 0xFF; break;
        case GL_UNSIGNED_SHORT:
          restartIdx = 0xFFFF; break;
        case GL_UNSIGNED_INT:
          restartIdx = 0xFFFFFFFF; break;
        default: break;
      }
    }
    else
    {
      ctx.dispatcher.driver.glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX, (GLint*)(&restartIdx));
    }

    switch(type)
    {
      case GL_UNSIGNED_BYTE:
        return MaxVertexCount<GLubyte>(ptr, base, count, restartIdx, isRestart);
      case GL_UNSIGNED_SHORT:
        return MaxVertexCount<GLushort>(ptr, base, count, restartIdx, isRestart);
      case GL_UNSIGNED_INT:
        return MaxVertexCount<GLuint>(ptr, base, count, restartIdx, isRestart);
      default:
        RegalAssert( "Unsupported <type> in for MaxVertexCount." );
        return 0;
    }
  }

  GLuint MaxArraysIndirectCount(const DrawArraysIndirectCommand* indirect, GLint drawcount)
  {
    GLuint maxCount = 0;
    for (GLint i = 0; i < drawcount; i++)
    {
      if (indirect[i].count > 0x7FFFFFFF)
        continue;
      GLuint c = indirect[i].count + indirect[i].first;
      if (c > maxCount)
        maxCount = c;
    }
    return maxCount;
  }

  void DrawArrays(RegalContext &ctx, GLint first, GLsizei count)
  {
    DrawArraysPrefix(ctx, &first, &count, 1);
  }

  GLboolean GetIndirectDrawCount(RegalContext &ctx, GLint* pdrawcount, GLint* clientdrawcount, GLboolean bypass)
  {
    GLint count_buffer = 0;
    ctx.dispatcher.driver.glGetIntegerv(GL_PARAMETER_BUFFER_BINDING_ARB, &count_buffer);
    if (bypass || count_buffer == 0)
    {
      if(!pdrawcount)
        return false;
      memcpy(clientdrawcount, (GLvoid*)pdrawcount, sizeof(GLuint));
    }
    else
    {
      ctx.dispatcher.driver.glGetBufferSubData(GL_PARAMETER_BUFFER_ARB, (GLintptr) pdrawcount, sizeof(GLuint), clientdrawcount);
    }

    return true;
  }
    
  template<typename T>
  GLboolean GetIndirectCommand(RegalContext &ctx, const void* indirect, T* clientCmd, GLint drawcount, GLuint stride, GLboolean bypass)
  {
    GLint di_buffer = 0;

    if (!bypass)
      ctx.dispatcher.driver.glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, &di_buffer);

    if (bypass || di_buffer == 0)
    {
      if (!indirect)
        return false;

      if (stride > sizeof(T))
      {
        for (GLint i = 0; i < drawcount; i++)
        {
          const GLubyte* ptr = (const GLubyte*)(indirect) + stride * i;
          memcpy(clientCmd + i, ptr, sizeof(T));
        }
      }
      else
      {
        memcpy(clientCmd, indirect, sizeof(T) * drawcount);
      }
    }
    else
    {
      GLvoid* ptrr = ctx.dispatcher.driver.glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, (GLuint64)(indirect) * sizeof(GLubyte), (stride > sizeof(T) ? stride : sizeof(T)) * drawcount, GL_MAP_READ_BIT);

      if (!ptrr)
          return false;

      if (stride > sizeof(T))
      {
        for(GLint i = 0; i < drawcount; i++)
        {
          const GLubyte* ptr = (GLubyte*)(ptrr) + stride * i;
          memcpy(clientCmd + i, ptr, sizeof(T));
        }
      }
      else
      {
        memcpy(clientCmd, ptrr, sizeof(T) * drawcount);
      }

      ctx.dispatcher.driver.glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
    }
    return true;
  }

  GLboolean DrawArraysPrefixByCommand(RegalContext &ctx, const void* indirect, GLint* pdrawcount, GLint* clientdrawcount, GLsizei maxdrawcount, GLuint stride, GLboolean bypass)
  {
    GLint drawcount = 1;

    std::vector<DrawArraysIndirectCommand> clientCmd;

    GLboolean ccinit = false;
    for (int index= 0; index < REGAL_EMU_MAX_VERTEX_ATTRIBS; index++)
    {
      Array &a = objects[current].a[index];
      if (!a.enabled || a.buffer != a.shadow_buffer || a.shadow_buffer == 0 || !a.shadow_pointer)
          continue;

      if (!ccinit)
      {
        if (!GetIndirectDrawCount(ctx, pdrawcount, &drawcount, bypass))
          return false;

        drawcount = std::min(drawcount, maxdrawcount);

        if (drawcount <= 0)
          return false;

        clientCmd.resize(drawcount);

        if (!GetIndirectCommand<DrawArraysIndirectCommand>(ctx, indirect, &clientCmd[0], drawcount, stride, bypass))
          return false;

        for (GLint i = 0; i < drawcount; i++)
          if (clientCmd[i].primCount == 0 || clientCmd[i].primCount > 0x7FFFFFFF)
            return false;

        ccinit = true;
      }

      GLsizei buffersize = TypeSize(a.type) * (a.size == GL_BGRA ? 4 : a.size) * MaxArraysIndirectCount(&clientCmd[0], drawcount);
      if (!buffersize)
        continue;

      ctx.dispatcher.driver.glBindBuffer(GL_ARRAY_BUFFER, a.shadow_buffer);
      ctx.dispatcher.driver.glBufferData(GL_ARRAY_BUFFER, buffersize, a.shadow_pointer, GL_DYNAMIC_DRAW);
      ctx.dispatcher.driver.glBindBuffer(GL_ARRAY_BUFFER, currObject ? currObject->vertexBuffer : 0);
    }

    if (clientdrawcount)
      *clientdrawcount = drawcount;

    return true;
  }

  GLboolean DrawArraysPrefix(RegalContext &ctx, const GLint* first, const GLsizei* count, GLint drawcount)
  {
    if (drawcount <= 0)
      return false;

    std::vector<DrawArraysIndirectCommand> cmd(drawcount);

    for(GLint i = 0; i < drawcount; i++)
    {
      cmd[i].count = count[i];
      cmd[i].primCount = 1;
      cmd[i].first = first[i];
      cmd[i].baseInstance = 0;
    }

    return DrawArraysPrefixByCommand(ctx, &cmd[0], &drawcount, NULL, drawcount, 0, true);
  }

  GLuint64 MultiCoverage(const DrawElementsIndirectCommand * indirect, GLuint elementSize, const GLvoid * const * pointer, GLint drawcount, GLuint64* offsets, GLvoid** copystart, GLuint64* copylength)
  {
    static PointerCount sort_func;

    std::vector<PointerCount> tmp_idr(drawcount);

    for (GLint i = 0; i < drawcount; i++)
    {
      tmp_idr[i]._count = indirect[i].count * elementSize;
      tmp_idr[i]._ptr = (GLuint64)pointer[i];
    }

    std::sort(tmp_idr.begin(), tmp_idr.end(), sort_func);

    GLuint64 dp = 0;
    GLuint64 base = tmp_idr[0]._ptr;
    GLuint64 sp = 0;
    GLuint64 ep = tmp_idr[0]._count;
    offsets[0] = 0;
    copystart[0] = (GLvoid*)base;
    copylength[0] = ep;

    for(GLint i = 1; i < drawcount; i++)
    {
      GLuint64 si = tmp_idr[i]._ptr - base;
      if (si - dp >= ep)
      {
        dp = si - ep;
        sp = ep;
        ep = sp + tmp_idr[i]._count;
        copystart[i] = (GLvoid*)(si + base);
        copylength[i] = tmp_idr[i]._count;
      }
      else
      {
        GLuint64 ei = si - dp + tmp_idr[i]._count;
        copystart[i] = (GLvoid*)(ep + dp + base);
        if (ei > ep)
        {
          copylength[i] = ei - ep;
          ep = ei;
        }
        else
        {
          copylength[i] = 0;
        }
        sp = si - dp;
      }
      offsets[i] = sp;
    }

    return ep;
  }

  GLboolean DrawElementsPrefixByCommand(RegalContext &ctx, GLenum type, const void* indirect, const GLvoid * const * pointer, GLvoid ** offsets_out, GLuint* pidxbuf, GLint* pdrawcount, GLint* clientdrawcount, GLsizei maxdrawcount, GLuint stride, GLboolean bypass)
  {
    if (!currObject)
      return false;

    GLsizei vtxcount = 0;
    GLint drawcount = 1;
    std::vector<DrawElementsIndirectCommand> clientCmd;
    GLboolean ccinit = false;

    for (int index= 0; index < REGAL_EMU_MAX_VERTEX_ATTRIBS; index++)
    {
      Array &a = objects[current].a[index];
      if(!a.enabled || a.buffer != a.shadow_buffer || a.shadow_buffer == 0 || !a.shadow_pointer)
        continue;

      if(!ccinit)
      {
        if (!GetIndirectDrawCount(ctx, pdrawcount, &drawcount, bypass))
          return false;

        drawcount = std::min(drawcount, maxdrawcount);

        if (drawcount <= 0)
          return false;

        clientCmd.resize(drawcount);

        if (!GetIndirectCommand<DrawElementsIndirectCommand>(ctx, indirect, &clientCmd[0], drawcount, stride, bypass))
          return false;

        for (GLint i = 0; i < drawcount; i++)
          if (clientCmd[i].primCount == 0 || clientCmd[i].primCount > 0x7FFFFFFF)
            return false;

        ccinit = true;

        if (currObject->indexBuffer == 0)
        {
          if (!pointer)
            return false;

          for(GLint i = 0; i < drawcount; i++)
          {
            if (!pointer[i] || clientCmd[i].count > 0x7FFFFFFF)
              continue;

            GLsizei vc = MaxVertexCount(ctx, type, pointer[i], clientCmd[i].firstIndex, clientCmd[i].count) + clientCmd[i].baseVertex;
            if (vc > vtxcount)
              vtxcount = vc;
          }
        }
        else
        {
          GLuint minIdx = 0x7FFFFFFF;
          GLuint maxIdx = 0;

          for(GLint i = 0; i < drawcount; i++)
          {
            if (clientCmd[i].count == 0 || clientCmd[i].count > 0x7FFFFFFF)
              continue;
            if (clientCmd[i].firstIndex < minIdx)
              minIdx = clientCmd[i].firstIndex;
            if (clientCmd[i].firstIndex + clientCmd[i].count > maxIdx)
              maxIdx = clientCmd[i].firstIndex + clientCmd[i].count;
          }

          GLvoid* ptr = ctx.dispatcher.driver.glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, minIdx * TypeSize(type), (maxIdx - minIdx) * TypeSize(type), GL_MAP_READ_BIT);
          if (!ptr)
            return false;

          for(GLint i = 0; i < drawcount; i++)
          {
            GLsizei vc = MaxVertexCount(ctx, type, ptr, clientCmd[i].firstIndex, clientCmd[i].count) + clientCmd[i].baseVertex;
            if (vc > vtxcount)
              vtxcount = vc;
          }
          ctx.dispatcher.driver.glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }
        if(vtxcount < 1)
          return false;
      }

      GLsizei buffersize = TypeSize(a.type) * (a.size == GL_BGRA ? 4 : a.size) * vtxcount;
      if (!buffersize)
        continue;

      ctx.dispatcher.driver.glBindBuffer(GL_ARRAY_BUFFER, a.shadow_buffer);
      ctx.dispatcher.driver.glBufferData(GL_ARRAY_BUFFER, buffersize, a.shadow_pointer, GL_DYNAMIC_DRAW);
      ctx.dispatcher.driver.glBindBuffer(GL_ARRAY_BUFFER, currObject ? currObject->vertexBuffer : 0);
    }

    if (currObject->indexBuffer == 0 && pointer && offsets_out)
    {
      for (GLint i = 0; i < drawcount; i++)
        if (!pointer[i])
          return false;

      if (!ccinit)
      {
        if(!GetIndirectDrawCount(ctx, pdrawcount, &drawcount, bypass))
          return false;

        drawcount = std::min(drawcount, maxdrawcount);

        if (drawcount <= 0)
          return false;

        clientCmd.resize(drawcount);

        if (!GetIndirectCommand<DrawElementsIndirectCommand>(ctx, indirect, &clientCmd[0], drawcount, stride, bypass))
          return false;

        for (GLint i = 0; i < drawcount; i++)
          if (clientCmd[i].primCount == 0 || clientCmd[i].primCount > 0x7FFFFFFF)
            return false;
      }

      std::vector<GLuint64> offsets(drawcount);
      std::vector<GLuint64> copylength(drawcount);
      std::vector<GLvoid*> copystart(drawcount);

      GLuint64 buffersize = MultiCoverage(&clientCmd[0], TypeSize(type), pointer, drawcount, &offsets[0], &copystart[0], &copylength[0]);

      if (buffersize == 0)
        return false;

      GLubyte* buffer = (GLubyte*)malloc(buffersize);

      GLuint64 accuLength = 0;
      for (GLint i = 0; i < drawcount; accuLength += copylength[i++])
      {
        memcpy(buffer + accuLength, copystart[i], copylength[i]);
        offsets_out[i] = (GLvoid *) offsets[i];
      }

      ctx.dispatcher.driver.glGenBuffers(1, pidxbuf);
      ctx.dispatcher.driver.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *pidxbuf);
      ctx.dispatcher.driver.glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffersize, buffer, GL_DYNAMIC_DRAW);
      free(buffer);
    }

    if (clientdrawcount)
      *clientdrawcount = drawcount;

    return true;
  }

  GLboolean DrawElementsPrefix(RegalContext &ctx, GLint drawcount, const GLsizei* count, GLenum type, const GLvoid * const * indices, GLvoid** offsets_out, GLuint* pidxbuf, const GLint* bv)
  {
    if (drawcount <= 0)
      return false;

    std::vector<DrawElementsIndirectCommand> cmd(drawcount);

    for (GLint i = 0; i < drawcount; i++)
    {
      cmd[i].count = count[i];
      cmd[i].primCount = 1;
      cmd[i].firstIndex = 0;
      cmd[i].baseVertex = bv[i];
      cmd[i].baseInstance = 0;
    }

    return DrawElementsPrefixByCommand(ctx, type, &cmd[0], indices, offsets_out, pidxbuf, &drawcount, NULL, drawcount, 0, true);
  }

  void DrawElementsSuffix(RegalContext &ctx, GLuint* pidxbuf)
  {
    if (!pidxbuf || !(*pidxbuf))
      return;
    ctx.dispatcher.driver.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currObject->indexBuffer);
    ctx.dispatcher.driver.glDeleteBuffers(1, pidxbuf);
  }

  GLboolean DrawElements(RegalContext &ctx, GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
  {
    GLuint idxbuf = 0;
    GLint bv = 0;
    GLvoid* offset = 0;

    if (!DrawElementsPrefix(ctx, 1, &count, type, &indices, &offset, &idxbuf, &bv))
      return false;

    if (currObject->indexBuffer == 0)
      ctx.dispatcher.emulation.glDrawElements(mode, count, type, 0);
    else
      ctx.dispatcher.emulation.glDrawElements(mode, count, type, indices);

    DrawElementsSuffix(ctx, &idxbuf);
    return true;
  }

  GLboolean DrawElementsInstanced(RegalContext &ctx, GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
  {
    GLuint idxbuf = 0;
    GLint bv = 0;
    GLvoid* offset = 0;

    if (primcount <= 0)
      return false;

    if (!DrawElementsPrefix(ctx, 1, &count, type, &indices, &offset, &idxbuf, &bv))
      return false;

    if (currObject->indexBuffer == 0)
      ctx.dispatcher.emulation.glDrawElementsInstanced(mode, count, type, 0, primcount);
    else
      ctx.dispatcher.emulation.glDrawElementsInstanced(mode, count, type, indices, primcount);

    DrawElementsSuffix(ctx, &idxbuf);
    return true;
  }

  GLboolean DrawElementsInstancedBaseInstance(RegalContext &ctx, GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount, GLuint baseinstance)
  {
    GLuint idxbuf = 0;
    GLint bv = 0;
    GLvoid* offset = 0;

    if (primcount <= 0)
      return false;

    if (!DrawElementsPrefix(ctx, 1, &count, type, &indices, &offset, &idxbuf, &bv))
      return false;

    if (currObject->indexBuffer == 0)
      ctx.dispatcher.emulation.glDrawElementsInstancedBaseInstance(mode, count, type, 0, primcount, baseinstance);
    else
      ctx.dispatcher.emulation.glDrawElementsInstancedBaseInstance(mode, count, type, indices, primcount, baseinstance);

    DrawElementsSuffix(ctx, &idxbuf);
    return true;
  }

  GLboolean DrawElementsBaseVertex(RegalContext &ctx, GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex)
  {
    GLuint idxbuf = 0;
    GLvoid* offset = 0;

    if (!DrawElementsPrefix(ctx, 1, &count, type, &indices, &offset, &idxbuf, &basevertex))
      return false;

    if (currObject->indexBuffer == 0)
      ctx.dispatcher.emulation.glDrawElementsBaseVertex(mode, count, type, 0, basevertex);
    else
      ctx.dispatcher.emulation.glDrawElementsBaseVertex(mode, count, type, indices, basevertex);

    DrawElementsSuffix(ctx, &idxbuf);
    return true;
  }

  GLboolean DrawElementsInstancedBaseVertex(RegalContext &ctx, GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount, GLint basevertex)
  {
    GLuint idxbuf = 0;
    GLvoid* offset = 0;

    if (primcount <= 0)
      return false;

    if (!DrawElementsPrefix(ctx, 1, &count, type, &indices, &offset, &idxbuf, &basevertex))
      return false;

    if (currObject->indexBuffer == 0)
      ctx.dispatcher.emulation.glDrawElementsInstancedBaseVertex(mode, count, type, 0, primcount, basevertex);
    else
      ctx.dispatcher.emulation.glDrawElementsInstancedBaseVertex(mode, count, type, indices, primcount, basevertex);

    DrawElementsSuffix(ctx, &idxbuf);
    return true;
  }

  GLboolean DrawElementsInstancedBaseVertexBaseInstance(RegalContext &ctx, GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount, GLint basevertex, GLuint baseinstance)
  {
    GLuint idxbuf = 0;
    GLvoid* offset = 0;

    if (primcount <= 0)
      return false;

    if (!DrawElementsPrefix(ctx, 1, &count, type, &indices, &offset, &idxbuf, &basevertex))
      return false;

    if (currObject->indexBuffer == 0)
      ctx.dispatcher.emulation.glDrawElementsInstancedBaseVertexBaseInstance(mode, count, type, 0, primcount, basevertex, baseinstance);
    else
      ctx.dispatcher.emulation.glDrawElementsInstancedBaseVertexBaseInstance(mode, count, type, indices, primcount, basevertex, baseinstance);

    DrawElementsSuffix(ctx, &idxbuf);
    return true;
  }

  GLboolean DrawRangeElements(RegalContext &ctx, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
  {
    GLuint idxbuf = 0;
    GLint bv = 0;
    GLvoid* offset = 0;

    if (!DrawElementsPrefix(ctx, 1, &count, type, &indices, &offset, &idxbuf, &bv))
      return false;

    if (currObject->indexBuffer == 0)
      ctx.dispatcher.emulation.glDrawRangeElements(mode, start, end, count, type, 0);
    else
      ctx.dispatcher.emulation.glDrawRangeElements(mode, start, end, count, type, indices);

    DrawElementsSuffix(ctx, &idxbuf);
    return true;
  }

  GLboolean DrawRangeElementsBaseVertex(RegalContext &ctx, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex)
  {
    GLuint idxbuf = 0;
    GLvoid* offset = 0;

    if (!DrawElementsPrefix(ctx, 1, &count, type, &indices, &offset, &idxbuf, &basevertex))
      return false;

    if (currObject->indexBuffer == 0)
      ctx.dispatcher.emulation.glDrawRangeElementsBaseVertex(mode, start, end, count, type, 0, basevertex);
    else
      ctx.dispatcher.emulation.glDrawRangeElementsBaseVertex(mode, start, end, count, type, indices, basevertex);

    DrawElementsSuffix(ctx, &idxbuf);
    return true;
  }
    
  GLboolean DrawArraysIndirectPrefix(RegalContext &ctx, const void *indirect, GLint* pdrawcount, GLsizei maxdrawcount, GLuint stride, GLuint* pidrbuf)
  {
    GLint drawcount = 1;
    if (!DrawArraysPrefixByCommand(ctx, indirect, pdrawcount, &drawcount, maxdrawcount, stride, false))
      return false;

    GLint di_buffer = 0;
    ctx.dispatcher.driver.glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, &di_buffer);

    if (di_buffer == 0)
    {
      if (!indirect)
        return false;
      ctx.dispatcher.driver.glGenBuffers(1, pidrbuf);
      ctx.dispatcher.driver.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, *pidrbuf);
      ctx.dispatcher.driver.glBufferData(GL_DRAW_INDIRECT_BUFFER,
                                         (stride > sizeof(DrawArraysIndirectCommand) ? stride : sizeof(DrawArraysIndirectCommand)) * drawcount,
                                         indirect, GL_DYNAMIC_DRAW);
    }

    return true;
  }

  void DrawIndirectSuffix(RegalContext &ctx, GLuint* pidrbuf)
  {
    if (!pidrbuf || !(*pidrbuf))
      return;
    ctx.dispatcher.driver.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    ctx.dispatcher.driver.glDeleteBuffers(1, pidrbuf);
  }

  GLboolean DrawElementsIndirectPrefix(RegalContext &ctx, GLint* pdrawcount, GLsizei maxdrawcount, GLuint stride, GLenum type, const void *indirect, GLuint* pidrbuf, GLuint* pidxbuf)
  {
    GLint drawcount = 1;
    if (!DrawElementsPrefixByCommand(ctx, type, indirect, NULL, NULL, pidxbuf, pdrawcount, &drawcount, maxdrawcount, stride, false))
      return false;

    GLint di_buffer = 0;
    ctx.dispatcher.driver.glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, &di_buffer);

    if (di_buffer == 0)
    {
      if (!indirect)
        return false;

      ctx.dispatcher.driver.glGenBuffers(1, pidrbuf);
      ctx.dispatcher.driver.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, *pidrbuf);
      ctx.dispatcher.driver.glBufferData(GL_DRAW_INDIRECT_BUFFER,
                                         (stride > sizeof(DrawElementsIndirectCommand) ? stride : sizeof(DrawElementsIndirectCommand)) * drawcount,
                                         indirect, GL_DYNAMIC_DRAW);
    }

    return true;
  }

  void DrawElementsIndirectSuffix(RegalContext &ctx, GLuint* pidxbuf, GLuint* pidrbuf)
  {
    DrawElementsSuffix(ctx, pidxbuf);
    DrawIndirectSuffix(ctx, pidrbuf);
  }

  GLboolean DrawArraysIndirect(RegalContext &ctx, GLenum mode, const void *indirect)
  {
    GLuint idrbuf = 0;
    GLint drawcount = 1;

    if (!DrawArraysIndirectPrefix(ctx, indirect, &drawcount, (GLsizei) drawcount, 0, &idrbuf))
      return false;

    if (!idrbuf)
      ctx.dispatcher.emulation.glDrawArraysIndirect(mode, indirect);
    else
      ctx.dispatcher.emulation.glDrawArraysIndirect(mode, 0);

    DrawIndirectSuffix(ctx, &idrbuf);

    return true;
  }

  GLboolean DrawElementsIndirect(RegalContext &ctx, GLenum mode, GLenum type, const void *indirect)
  {
    GLuint idxbuf = 0, idrbuf = 0;
    GLint drawcount = 1;

    if (!DrawElementsIndirectPrefix(ctx, &drawcount, (GLsizei) drawcount, 0, type, indirect, &idrbuf, &idxbuf))
      return false;

    if (!idrbuf)
      ctx.dispatcher.emulation.glDrawElementsIndirect(mode, type, indirect);
    else
      ctx.dispatcher.emulation.glDrawElementsIndirect(mode, type, 0);

    DrawElementsIndirectSuffix(ctx, &idxbuf, &idrbuf);

    return true;
  }

  void MultiDrawArrays(RegalContext &ctx, const GLint * first, const GLsizei * count, GLsizei drawcount)
  {
      DrawArraysPrefix(ctx, first, count, drawcount);
  }

  GLboolean MultiDrawArraysIndirect(RegalContext &ctx, GLenum mode, const void *indirect, GLsizei drawcount, GLsizei stride)
  {
    GLuint idrbuf;

    if (!DrawArraysIndirectPrefix(ctx, indirect, (GLint*) &drawcount, (GLsizei) drawcount, stride, &idrbuf))
      return false;

    if (!idrbuf)
      ctx.dispatcher.emulation.glMultiDrawArraysIndirect(mode, indirect, drawcount, stride);
    else
      ctx.dispatcher.emulation.glMultiDrawArraysIndirect(mode, 0, drawcount, stride);

    DrawIndirectSuffix(ctx, &idrbuf);

    return true;
  }

  GLboolean MultiDrawElements(RegalContext &ctx, GLenum mode, const GLsizei *count, GLenum type, const GLvoid * const *indices, GLsizei drawcount)
  {
    GLuint idxbuf = 0;
    std::vector<GLvoid *> offsets(drawcount);
    std::vector<GLint> bvs(drawcount);
    memset(&bvs[0], 0, drawcount * sizeof(GLint));

    if (!DrawElementsPrefix(ctx, drawcount, count, type, indices, &offsets[0], &idxbuf, &bvs[0]))
      return false;

    if (currObject->indexBuffer == 0)
      ctx.dispatcher.emulation.glMultiDrawElements(mode, count, type, (const GLvoid * const *)(&offsets[0]), drawcount);
    else
      ctx.dispatcher.emulation.glMultiDrawElements(mode, count, type, indices, drawcount);

    DrawElementsSuffix(ctx, &idxbuf);
    return true;
  }

  GLboolean MultiDrawElementsBaseVertex(RegalContext &ctx, GLenum mode, const GLsizei *count, GLenum type, const GLvoid * const *indices, GLsizei drawcount, const GLint *basevertex)
  {
    GLuint idxbuf = 0;
    std::vector<GLvoid *> offsets(drawcount);

    if (!DrawElementsPrefix(ctx, drawcount, count, type, indices, &offsets[0], &idxbuf, basevertex))
      return false;

    if (currObject->indexBuffer == 0)
      ctx.dispatcher.emulation.glMultiDrawElementsBaseVertex(mode, count, type, (const GLvoid * const *)(&offsets[0]), drawcount, basevertex);
    else
      ctx.dispatcher.emulation.glMultiDrawElementsBaseVertex(mode, count, type, indices, drawcount, basevertex);

    DrawElementsSuffix(ctx, &idxbuf);
    return true;
  }

  GLboolean MultiDrawElementsIndirect(RegalContext &ctx, GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride)
  {
    GLuint idxbuf = 0, idrbuf = 0;

    if (!DrawElementsIndirectPrefix(ctx, (GLint*) &drawcount, drawcount, stride, type, indirect, &idrbuf, &idxbuf))
      return false;

    if (!idrbuf)
      ctx.dispatcher.emulation.glMultiDrawElementsIndirect(mode, type, indirect, drawcount, stride);
    else
      ctx.dispatcher.emulation.glMultiDrawElementsIndirect(mode, type, 0, drawcount, stride);

    DrawElementsIndirectSuffix(ctx, &idxbuf, &idrbuf);

    return true;
  }

  GLboolean MultiDrawArraysIndirectCount(RegalContext &ctx, GLenum mode, const GLvoid *indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride)
  {
    GLint count_buffer = 0;

    ctx.dispatcher.driver.glGetIntegerv(GL_PARAMETER_BUFFER_BINDING_ARB, &count_buffer);

    if (count_buffer == 0)
    {
      if (!drawcount)
        return false;
      return MultiDrawArraysIndirect(ctx, mode, indirect, std::min(*((GLint*)drawcount), (GLint) maxdrawcount), stride);
    }

    GLuint idrbuf;
    if (!DrawArraysIndirectPrefix(ctx, indirect, (GLint*)drawcount, maxdrawcount, stride, &idrbuf))
      return false;

    if (!idrbuf)
      ctx.dispatcher.emulation.glMultiDrawArraysIndirectCountARB(mode, indirect, drawcount, maxdrawcount, stride);
    else
      ctx.dispatcher.emulation.glMultiDrawArraysIndirectCountARB(mode, 0, drawcount, maxdrawcount, stride);

    DrawIndirectSuffix(ctx, &idrbuf);

    return true;
  }

  GLboolean MultiDrawElementsIndirectCount(RegalContext &ctx, GLenum mode, GLenum type, const GLvoid *indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride)
  {
    GLint count_buffer = 0;

    ctx.dispatcher.driver.glGetIntegerv(GL_PARAMETER_BUFFER_BINDING_ARB, &count_buffer);

    if (count_buffer == 0)
    {
      if (!drawcount)
        return false;
      return MultiDrawElementsIndirect(ctx, mode, type, indirect, std::min(*((GLint*)drawcount), (GLint) maxdrawcount), stride);
    }

    GLuint idxbuf = 0, idrbuf = 0;

    if (!DrawElementsIndirectPrefix(ctx, (GLint*)drawcount, maxdrawcount, stride, type, indirect, &idrbuf, &idxbuf))
      return false;

    if (!idrbuf)
      ctx.dispatcher.emulation.glMultiDrawElementsIndirectCountARB(mode, type, indirect, drawcount, maxdrawcount, stride);
    else
      ctx.dispatcher.emulation.glMultiDrawElementsIndirectCountARB(mode, type, 0, drawcount, maxdrawcount, stride);

    DrawElementsIndirectSuffix(ctx, &idxbuf, &idrbuf);

    return true;
  }
};

}

REGAL_NAMESPACE_END

#endif // ! __REGAL_VAO_H__
