/*

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

- Created by Cass Everitt on 3/15/11.

*/

#import "RegalView.h"
#import "RGLOpenGLContext.h"
#include "render.h"
#include <stdio.h>

@implementation GlesView

bool resized = false;

NSTimer *timer = nil;

- (id)initWithCoder:(NSCoder *)aDecoder {

    [super initWithCoder: aDecoder];
  NSOpenGLPixelFormatAttribute attr[] = {
#if 1
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
#else
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
#endif
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    NSOpenGLPFAAlphaSize, 8,
    NSOpenGLPFAColorSize, 32,
    NSOpenGLPFADepthSize, 24,
    NSOpenGLPFAStencilSize, 0,
    0
  };
  NSOpenGLPixelFormat *fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:attr];
    [self setPixelFormat: fmt];
    RGLOpenGLContext *rglCtx = [[RGLOpenGLContext alloc] initWithFormat:fmt shareContext:nil];
    [self setOpenGLContext: rglCtx];
    [rglCtx setView:self];
  timer = [NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector (render) userInfo:nil repeats:YES];
    resized = false;
    return self;
}


- (void)drawRect:(NSRect)dirtyRect {
  if( resized == false ) {
    [self windowResized:NULL];
    resized = true;
  }
  alphaTorusDisplay( true );
  CGLFlushDrawable( CGLGetCurrentContext() );
  //[[self openGLContext] flushBuffer];
}

- (void) render {
  [self setNeedsDisplay:YES];
}

- (void)viewDidMoveToWindow
{
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(windowResized:) name:NSWindowDidResizeNotification
                                               object:[self window]];
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}

- (void)windowResized:(NSNotification *)notification;
{
    NSRect rect = [[[self window] contentView] frame];
    NSRect crect = NSRectFromCGRect( CGRectMake( 0, 0, rect.size.width, rect.size.height ) );
    [self setFrame: crect];
    [self lockFocus];
    alphaTorusReshape( rect.size.width, rect.size.height );
    resized = true;
    [self unlockFocus];
}

@end
