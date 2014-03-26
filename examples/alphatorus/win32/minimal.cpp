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

*/

// An example of the minimal Win32 OpenGL (via Regal) program.
// It only works in 16 bit color modes or higher (since it doesn't
// create a palette).

#include <windows.h>

#include <GL/Regal.h>

#include "render.h"

#include <cstdio>
#include <cstdlib>
using namespace std;

LRESULT WINAPI
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static PAINTSTRUCT ps;

  switch(uMsg)
  {
    case WM_PAINT:
      alphaTorusDisplay(true);
      SwapBuffers(GetDC(hWnd));
      BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
      return 0;

    case WM_SIZE:
      alphaTorusReshape(LOWORD(lParam), HIWORD(lParam));
      PostMessage(hWnd, WM_PAINT, 0, 0);
      return 0;

    case WM_CHAR:
      switch (wParam) {
        case 27:      /* ESC key */
            PostQuitMessage(0);
            break;
      }
      return 0;

    case WM_CLOSE:
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND
CreateOpenGLWindow(LPCTSTR title, int x, int y, int width, int height, BYTE type, DWORD flags)
{
  int         pf;
  HDC         hDC;
  HWND        hWnd;
  WNDCLASS    wc;
  PIXELFORMATDESCRIPTOR pfd;
  static HINSTANCE hInstance = 0;

  /* only register the window class once - use hInstance as a flag. */
  if (!hInstance)
  {
    hInstance        = GetModuleHandle(NULL);
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = (WNDPROC)WindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = TEXT("OpenGL");

    if (!RegisterClass(&wc))
    {
      MessageBox(NULL, TEXT("RegisterClass() failed: Cannot register window class."),
        TEXT("Error"), MB_OK);
      return NULL;
    }
  }

  hWnd = CreateWindow(TEXT("OpenGL"), title,
    WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    x, y, width, height, NULL, NULL, hInstance, NULL);

  if (!hWnd)
  {
    MessageBox(NULL, TEXT("CreateWindow() failed:  Cannot create a window."),
      TEXT("Error"), MB_OK);
    return NULL;
  }

  hDC = GetDC(hWnd);

  /* there is no guarantee that the contents of the stack that become
     the pfd are zeroed, therefore _make sure_ to clear these bits. */
  memset(&pfd, 0, sizeof(pfd));
  pfd.nSize        = sizeof(pfd);
  pfd.nVersion     = 1;
  pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;
  pfd.iPixelType   = type;
  pfd.cColorBits   = 32;

  pf = ChoosePixelFormat(hDC, &pfd);
  if (!pf)
  {
    MessageBox(NULL, TEXT("ChoosePixelFormat() failed:  Cannot find a suitable pixel format."),
      TEXT("Error"), MB_OK);
    return 0;
  }

  if (!SetPixelFormat(hDC, pf, &pfd)) {
    MessageBox(NULL, TEXT("SetPixelFormat() failed:  Cannot set format specified."),
      TEXT("Error"), MB_OK);
    return 0;
  }

  DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

  ReleaseDC(hWnd, hDC);

  return hWnd;
}

int APIENTRY
WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow)
{
  HDC hDC;        /* device context */
  HGLRC hRC;      /* opengl context */
  HWND  hWnd;     /* window         */
  MSG   msg;      /* message        */

  hWnd = CreateOpenGLWindow(TEXT("alphatorus_win32"), 0, 0, 256, 256, PFD_TYPE_RGBA, PFD_DOUBLEBUFFER);
  if (!hWnd)
    exit(1);

  hDC = GetDC(hWnd);
  hRC = wglCreateContext(hDC);
  wglMakeCurrent(hDC,hRC);
  RegalMakeCurrent(hRC);

  ShowWindow(hWnd, nCmdShow);

  while(GetMessage(&msg, hWnd, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  RegalMakeCurrent(NULL);
  ReleaseDC(hWnd, hDC);
  wglDeleteContext(hRC);
  DestroyWindow(hWnd);

  return (int) msg.wParam;
}
