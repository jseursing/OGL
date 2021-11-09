#pragma once
#include <ctime>
#include <stdint.h>
#include <Windows.h>
#include <gl/glcorearb.h>
#include <gl/wglext.h>
#include <gl/GL.h>
#include <gl/GLU.h>


// ---------------------------------------------------------------
// OpenGL API Extensions
// ---------------------------------------------------------------
extern PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLDELETESHADERPROC             glDeleteShader;
extern PFNGLACTIVETEXTUREPROC            glActiveTexture;
extern PFNGLCREATESHADERPROC             glCreateShader;
extern PFNGLSHADERSOURCEPROC             glShaderSource;
extern PFNGLCOMPILESHADERPROC            glCompileShader;
extern PFNGLGETSHADERIVPROC              glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
extern PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
extern PFNGLBINDBUFFERPROC               glBindBuffer;
extern PFNGLBUFFERDATAPROC               glBufferData;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
extern PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
extern PFNGLGENBUFFERSPROC               glGenBuffers;
extern PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays;
extern PFNGLDELETEBUFFERSPROC            glDeleteBuffers;

// ----------------------------------------
// Event Callback definitions
// ----------------------------------------
typedef void (*OGLDrawFunc)(void);                  // Invoked when time to draw
typedef void (*OGLIdleFunc)(void);                  // Invoked when not time to draw
typedef void (*OGLViewChangeFunc)(void);            // Invoked when resized/moved
typedef void (*OGLKeyUpFunc)(int32_t);              // On KeyUp
typedef void (*OGLKeyDownFunc)(int32_t);            // On KeyDown
typedef void (*OGLLUpFunc)(int32_t, int32_t);       // On Mouse-Left up
typedef void (*OGLLDownFunc)(int32_t, int32_t);     // On Mouse-Left down
typedef void (*OGLRUpFunc)(int32_t, int32_t);       // On Mouse-Right up
typedef void (*OGLRDownFunc)(int32_t, int32_t);     // On Mouse-Right down
typedef void (*OGLMouseMoveFunc)(int32_t, int32_t); // On Mouse-Move
typedef void (*OGLJoystickFunc)(uint32_t, WPARAM, LPARAM); // On Joystick

// Class definition
class OGLExt
{
public:

  enum GLStatusTypes
  {
    OGL_SUCCESS,
    OGL_REG_CLASS_ERROR,
    OGL_WND_CREATE_ERROR,
    OGL_MODULE_NOT_FOUND,
    OGL_API_LOAD_ERROR,
    OGL_PX_FMT_NOT_FOUND,
    OGL_SET_PX_FMT_ERROR
  };

  enum GLDisplayTypes
  {
    OGL_SGL_BUFFER = 0x00000001,
    OGL_DBL_BUFFER = 0x00000002,
  };

  static GLStatusTypes CreateContext(uint32_t displayFlags);
  static void InitWindow(const char* windowTitle,
                         int32_t x, 
                         int32_t y,
                         uint32_t width,
                         uint32_t height,
                         bool canResize = false,
                         bool canMaximize = false);

  static HWND GetThisWindow();
  static GLStatusTypes GetLastStatus();
  static void SetRefreshRate(uint32_t fps);
  static void RefreshDisplay();
  static void GLSwapBuffers();
  static void MainLoop();

  static void RegisterDraw(OGLDrawFunc func);
  static void RegisterIdle(OGLIdleFunc func);
  static void RegisterViewChange(OGLViewChangeFunc func);
  static void RegisterKeyUp(OGLKeyUpFunc func);
  static void RegisterKeyDown(OGLKeyDownFunc func);
  static void RegisterMouseLeftUp(OGLLUpFunc func);
  static void RegisterMouseLeftDown(OGLLDownFunc func);
  static void RegisterMouseRightUp(OGLRUpFunc func);
  static void RegisterMouseRightDown(OGLRDownFunc func);
  static void RegisterMouseMove(OGLMouseMoveFunc func);

private:

  static void InvokeDrawCallback();
  static void InvokeIdleCallback();
  static void InvokeViewChangeCallback();
  static void InvokeKeyUpCallback(WPARAM wParam);
  static void InvokeKeyDownCallback(WPARAM wParam);
  static void InvokeMLeftUpCallback(int32_t x, int32_t y);
  static void InvokeMLeftDownCallback(int32_t x, int32_t y);
  static void InvokeMRightUpCallback(int32_t x, int32_t y);
  static void InvokeMRightDownCallback(int32_t x, int32_t y);
  static void InvokeMouseMoveCallback(int32_t x, int32_t y);
  static void InvokeJoystickCallback(uint32_t msg, WPARAM wParam, LPARAM lParam);

  static void FinalizeOpenGLInitialization();
  static void LoadOpenGLExtensions();
  static void* GetGLProc(HMODULE hModule, const char* func);

  static LRESULT __stdcall OGLWndProc(HWND hWnd, 
                                      uint32_t msg, 
                                      WPARAM wParam, 
                                      LPARAM lParam);

  // Members
  static GLStatusTypes Status;
  static HINSTANCE ThisInstance;
  static HWND DummyWndHandle;
  static HWND WindowHandle;
  static HDC WindowContext;
  static HGLRC OGLContext;
  static RECT WindowRect;
  static bool Refresh;
  static std::clock_t FrameTime;

  // Modifiable attributes
  static int32_t PixelFormatChoice;
  static PIXELFORMATDESCRIPTOR PixelFormat;
  static int32_t PixelAttributes[];
  static uint32_t RefreshFrequency;

  // Callbacks
  static OGLDrawFunc       DrawCallback;
  static OGLIdleFunc       IdleCallback;
  static OGLViewChangeFunc ViewChangeCallback;
  static OGLKeyUpFunc      KeyUpCallback;
  static OGLKeyDownFunc    KeyDownCallback;
  static OGLLUpFunc        MLeftUpCallback;
  static OGLLDownFunc      MLeftDownCallback;
  static OGLRUpFunc        MRightUpCallback;
  static OGLRDownFunc      MRightDownCallback;
  static OGLMouseMoveFunc  MMoveCallback;
};