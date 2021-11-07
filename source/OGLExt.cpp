#include "OGLExt.h"
#include <string>

// ---------------------------------------------------------------
// OpenGL API Extensions
// ---------------------------------------------------------------
PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB;
PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNGLCREATEPROGRAMPROC            glCreateProgram;
PFNGLDELETEPROGRAMPROC            glDeleteProgram;
PFNGLUSEPROGRAMPROC               glUseProgram;
PFNGLATTACHSHADERPROC             glAttachShader;
PFNGLLINKPROGRAMPROC              glLinkProgram;
PFNGLGETPROGRAMIVPROC             glGetProgramiv;
PFNGLDELETESHADERPROC             glDeleteShader;
PFNGLACTIVETEXTUREPROC            glActiveTexture;
PFNGLCREATESHADERPROC             glCreateShader;
PFNGLSHADERSOURCEPROC             glShaderSource;
PFNGLCOMPILESHADERPROC            glCompileShader;
PFNGLGETSHADERIVPROC              glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
PFNGLBINDBUFFERPROC               glBindBuffer;
PFNGLBUFFERDATAPROC               glBufferData;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
PFNGLGENBUFFERSPROC               glGenBuffers;
PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays;
PFNGLDELETEBUFFERSPROC            glDeleteBuffers;

// ---------------------------------------------------------------
// Static Definitions
// ---------------------------------------------------------------
OGLExt::GLStatusTypes OGLExt::Status = OGLExt::OGL_SUCCESS;

HINSTANCE OGLExt::ThisInstance = nullptr;
HWND OGLExt::DummyWndHandle    = 0;
HWND OGLExt::WindowHandle      = 0;
HDC OGLExt::WindowContext      = 0;
HGLRC OGLExt::OGLContext       = 0;
RECT OGLExt::WindowRect        = {0, 0, 0, 0};
bool OGLExt::Refresh           = false;
std::clock_t OGLExt::FrameTime = 0;

int32_t OGLExt::PixelFormatChoice         = 0;
PIXELFORMATDESCRIPTOR OGLExt::PixelFormat =
{
	sizeof(PIXELFORMATDESCRIPTOR),                              // Size
	1,                                                          // Ver.
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
	PFD_TYPE_RGBA,        // Pixel Type
	32,                   // Color Bits
	0, 0, 0, 0, 0, 0,
	0,
	0,
	0,
	0, 0, 0, 0,
	24,                   // Depth Bits
	8,                    // Stencil Bits
	0,                    // Aux Buffs
	PFD_MAIN_PLANE,
	0,
	0, 0, 0
};

int32_t OGLExt::PixelAttributes[] =
{
  WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
  WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
  WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
  WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
  WGL_COLOR_BITS_ARB,     32,
  WGL_DEPTH_BITS_ARB,     24,
  WGL_STENCIL_BITS_ARB,   8,
  0 // End
};

uint32_t OGLExt::RefreshFrequency = 1000 / 60; // 60-FPS default


// ---------------------------------------------------------------
// User-Defined Callbacks
// ---------------------------------------------------------------
OGLDrawFunc       OGLExt::DrawCallback       = nullptr;
OGLIdleFunc       OGLExt::IdleCallback       = nullptr;
OGLViewChangeFunc OGLExt::ViewChangeCallback = nullptr;
OGLKeyUpFunc      OGLExt::KeyUpCallback      = nullptr;
OGLKeyDownFunc    OGLExt::KeyDownCallback    = nullptr;
OGLLUpFunc        OGLExt::MLeftUpCallback    = nullptr;
OGLLDownFunc      OGLExt::MLeftDownCallback  = nullptr;
OGLRUpFunc        OGLExt::MRightUpCallback   = nullptr;
OGLRDownFunc      OGLExt::MRightDownCallback = nullptr;
OGLMouseMoveFunc  OGLExt::MMoveCallback      = nullptr;
OGLJoystickFunc   OGLExt::JoystickCallback   = nullptr;

// --------------------------------------------------------------------------------------
// Function: CreateContext
// Notes: None
// --------------------------------------------------------------------------------------
OGLExt::GLStatusTypes OGLExt::CreateContext(uint32_t displayFlags)
{
#if !(_DEBUG)
  FreeConsole();
#endif

  // Update Pixel attributes
  if (displayFlags & OGL_SGL_BUFFER)
  {
    // Defaults to double buffer, so we need to update this.
    PixelAttributes[5] = GL_FALSE;
  }

  // WinMain normally provides this member, but we'll retrieve it via WinAPI.
  ThisInstance = GetModuleHandle(nullptr);

  // Register our window class
  WNDCLASSA oglWndClass;
  memset(&oglWndClass, 0, sizeof(WNDCLASS));

  oglWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  oglWndClass.lpfnWndProc = OGLWndProc;
  oglWndClass.hInstance = ThisInstance;
  oglWndClass.lpszClassName = "OGLWindowClass";
  if (false == RegisterClassA(&oglWndClass))
  {
    Status = OGL_REG_CLASS_ERROR;
    return Status;
  }

  // Create the dummy window. The WndProc will catch this creation 
  // and continue processing the context creation.
  CreateWindowA("OGLWindowClass",
                "OGLDummyWindow",
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                100,
                100,
                0,
                0,
                ThisInstance,
                nullptr);

  return Status;
}

// --------------------------------------------------------------------------------------
// Function: InitWindow
// Notes: If using GLEW or other context libs, invoke this after initializing that.
// --------------------------------------------------------------------------------------
void OGLExt::InitWindow(const char* windowTitle,
                        int32_t x, 
                        int32_t y,
                        uint32_t width,
                        uint32_t height,
                        bool canResize,
                        bool canMaximize)
{
  // Create window with specified flags
  uint32_t windowFlags = WS_OVERLAPPEDWINDOW;
  windowFlags ^= (true == canResize ? 0 : WS_THICKFRAME);
  windowFlags ^= (true == canMaximize ? 0 : WS_MAXIMIZEBOX);

  WindowHandle = CreateWindowA("OGLWindowClass",
                               windowTitle,
                               windowFlags,
                               x,
                               y,
                               width,
                               height,
                               0,
                               0,
                               ThisInstance,
                               nullptr);
  if (0 == WindowHandle)
  {
    Status = OGL_WND_CREATE_ERROR;
    return;
  }

  // Bring window to front
  SetForegroundWindow(WindowHandle);

  // Update Window Rect
  GetWindowRect(WindowHandle, &WindowRect);

  // Get device context
  WindowContext = GetDC(WindowHandle);

  // Choose pixel format
  int32_t pixelFormat  = 0;
  uint32_t formatCount = 0;
  if (FALSE == wglChoosePixelFormatARB(WindowContext, 
                                       PixelAttributes, 
                                       nullptr, 
                                       1, 
                                       &pixelFormat, 
                                       &formatCount))
  {
    Status = OGL_PX_FMT_NOT_FOUND;
    return;
  }

  // Set Pixel format
  if (FALSE == SetPixelFormat(WindowContext, PixelFormatChoice, &PixelFormat))
  {
    Status = OGL_SET_PX_FMT_ERROR;
    return;
  }

  // Create new context
  int32_t glContextAttr[] =
  {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 2,
    WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0
  };

  OGLContext = wglCreateContextAttribsARB(WindowContext, 0, glContextAttr);
  if (ERROR_SUCCESS != GetLastError())
  {
    MessageBoxA(0, "OGLExt Failed Creating Context", "ERROR", MB_ICONERROR);
  }

  wglMakeCurrent(WindowContext, OGLContext);
}

// --------------------------------------------------------------------------------------
// Function: GetThisWindow
// Notes: None
// --------------------------------------------------------------------------------------
HWND OGLExt::GetThisWindow()
{
  return WindowHandle;
}

// --------------------------------------------------------------------------------------
// Function: GetLastStatus
// Notes: None
// --------------------------------------------------------------------------------------
OGLExt::GLStatusTypes OGLExt::GetLastStatus()
{
  return Status;
}

// --------------------------------------------------------------------------------------
// Function: SetRefreshRate
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::SetRefreshRate(uint32_t fps)
{
  RefreshFrequency = 1000 / fps;
}

// --------------------------------------------------------------------------------------
// Function: RefreshDisplay
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RefreshDisplay()
{
  Refresh = true;
}

// --------------------------------------------------------------------------------------
// Function: GLSwapBuffers
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::GLSwapBuffers()
{
  SwapBuffers(WindowContext);
}

// --------------------------------------------------------------------------------------
// Function: MainLoop
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::MainLoop()
{
  bool firstQuit = true;

  ShowWindow(WindowHandle, SW_SHOW);

  MSG message;
  while (true)
  {
    // Process all window messages here.
    if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
      if (WM_QUIT == message.message)
      {
        // Avoid destroying our game window on init
        if (false == firstQuit)
        {
          break;
        }
        firstQuit = false;
      }

      TranslateMessage(&message);
      DispatchMessage(&message);
    }

    // Send a PAINT message to our WndProc if we reach our frame limit.
    std::clock_t currTime = std::clock();
    uint32_t targetTime = (((currTime - FrameTime) * 1000) / CLOCKS_PER_SEC);

    if ((true == Refresh) ||
        (RefreshFrequency <= targetTime))
    {
      // Send PAINT message to WndProc
      message.message = WM_PAINT;
      TranslateMessage(&message);
      DispatchMessage(&message);

      // Update FrameTime and Refresh flag
      FrameTime = currTime;
      Refresh = false;
      continue;
    }
    else 
    {
      InvokeIdleCallback();
    }
  }
}

// --------------------------------------------------------------------------------------
// Function: RegisterDraw
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterDraw(OGLDrawFunc func)
{
  DrawCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: RegisterIdle
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterIdle(OGLIdleFunc func)
{
  IdleCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: RegisterViewChange
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterViewChange(OGLViewChangeFunc func)
{
  ViewChangeCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: RegisterKeyUp
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterKeyUp(OGLKeyUpFunc func)
{
  KeyUpCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: RegisterKeyDown
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterKeyDown(OGLKeyDownFunc func)
{
  KeyDownCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: RegisterMouseLeftUp
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterMouseLeftUp(OGLLUpFunc func)
{
  MLeftUpCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: RegisterMouseLeftDown
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterMouseLeftDown(OGLLDownFunc func)
{
  MLeftDownCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: RegisterMouseRightUp
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterMouseRightUp(OGLRUpFunc func)
{
  MRightUpCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: RegisterMouseRightDown
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterMouseRightDown(OGLRDownFunc func)
{
  MRightDownCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: RegisterMouseMove
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterMouseMove(OGLMouseMoveFunc func)
{
  MMoveCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: RegisterJoystick
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::RegisterJoystick(OGLJoystickFunc func)
{
  JoystickCallback = func;
}

// --------------------------------------------------------------------------------------
// Function: InvokeDrawCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeDrawCallback()
{
  if (nullptr != DrawCallback)
  {
    DrawCallback();
  }
}

// --------------------------------------------------------------------------------------
// Function: InvokeIdleCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeIdleCallback()
{
  if (nullptr != IdleCallback)
  {
    IdleCallback();
  }
}

// --------------------------------------------------------------------------------------
// Function: InvokeViewChangeCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeViewChangeCallback()
{
  GetWindowRect(WindowHandle, &WindowRect);

  if (nullptr != ViewChangeCallback)
  {
    ViewChangeCallback();
  }
}

// --------------------------------------------------------------------------------------
// Function: InvokeKeyUpCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeKeyUpCallback(WPARAM wParam)
{
  if (nullptr != KeyUpCallback)
  {
    KeyUpCallback(wParam);
  }
}

// --------------------------------------------------------------------------------------
// Function: InvokeKeyDownCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeKeyDownCallback(WPARAM wParam)
{
  if (nullptr != KeyDownCallback)
  {
    KeyDownCallback(wParam);
  }
}

// --------------------------------------------------------------------------------------
// Function: InvokeMLeftUpCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeMLeftUpCallback(int32_t x, int32_t y)
{
  if (nullptr != MLeftUpCallback)
  {
    MLeftUpCallback(x - WindowRect.left, y - WindowRect.top);
  }
}

// --------------------------------------------------------------------------------------
// Function: InvokeMLeftDownCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeMLeftDownCallback(int32_t x, int32_t y)
{
  if (nullptr != MLeftDownCallback)
  {
    MLeftDownCallback(x - WindowRect.left, y - WindowRect.top);
  }
}

// --------------------------------------------------------------------------------------
// Function: InvokeMRightUpCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeMRightUpCallback(int32_t x, int32_t y)
{
  if (nullptr != MRightUpCallback)
  {
    MRightUpCallback(x - WindowRect.left, y - WindowRect.top);
  }
}

// --------------------------------------------------------------------------------------
// Function: InvokeMRightDownCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeMRightDownCallback(int32_t x, int32_t y)
{
  if (nullptr != MRightDownCallback)
  {
    MRightDownCallback(x - WindowRect.left, y - WindowRect.top);
  }
}

// --------------------------------------------------------------------------------------
// Function: InvokeMouseMoveCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeMouseMoveCallback(int32_t x, int32_t y)
{
  if (nullptr != MMoveCallback)
  {
    MMoveCallback(x - WindowRect.left, y - WindowRect.top);
  }
}

// --------------------------------------------------------------------------------------
// Function: InvokeJoystickCallback
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::InvokeJoystickCallback(uint32_t msg, WPARAM wParam, LPARAM lParam)
{
  if (nullptr != JoystickCallback)
  {
    JoystickCallback(msg, wParam, lParam);
  }
}

// --------------------------------------------------------------------------------------
// Function: FinalizeOpenGLInitialization
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::FinalizeOpenGLInitialization()
{
  // DummyWndHandle should have been set by now.
  if (0 == DummyWndHandle)
  {
    Status = OGL_WND_CREATE_ERROR;
    return;
  }

  // Retrieve window device context
  WindowContext = GetDC(DummyWndHandle);

  // Retrieve similar supported format
  PixelFormatChoice = ChoosePixelFormat(WindowContext, &PixelFormat);
  SetPixelFormat(WindowContext, PixelFormatChoice, &PixelFormat);

  // Set rendering context
  OGLContext = wglCreateContext(WindowContext);
  wglMakeCurrent(WindowContext, OGLContext);

  // Now that we have our dummy context, initialize OpenGL API
  LoadOpenGLExtensions();
}

// --------------------------------------------------------------------------------------
// Function: LoadOpenGLExtensions
// Notes: None
// --------------------------------------------------------------------------------------
void OGLExt::LoadOpenGLExtensions()
{
  HMODULE glModule = LoadLibraryA("opengl32.dll");
  if (nullptr == glModule)
  {
    Status = OGL_MODULE_NOT_FOUND;
    return;
  }

  wglGetExtensionsStringARB  = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>
                               (GetGLProc(glModule, "wglGetExtensionsStringARB"));
  wglChoosePixelFormatARB    = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>
                               (GetGLProc(glModule, "wglChoosePixelFormatARB"));
  wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>
                               (GetGLProc(glModule, "wglCreateContextAttribsARB"));
//  glGetString                = reinterpret_cast<PFNGLGETSTRINGPROC>
//                               (GetGLProc(glModule, "glGetString"));
//  glClearColor               = reinterpret_cast<PFNGLCLEARCOLORPROC>
//                               (GetGLProc(glModule, "glClearColor"));
  glCreateProgram            = reinterpret_cast<PFNGLCREATEPROGRAMPROC>
                               (GetGLProc(glModule, "glCreateProgram"));
  glDeleteProgram            = reinterpret_cast<PFNGLDELETEPROGRAMPROC>
                               (GetGLProc(glModule, "glDeleteProgram"));
  glUseProgram               = reinterpret_cast<PFNGLUSEPROGRAMPROC>
                               (GetGLProc(glModule, "glUseProgram"));
  glAttachShader             = reinterpret_cast<PFNGLATTACHSHADERPROC>
                               (GetGLProc(glModule, "glAttachShader"));
  glLinkProgram              = reinterpret_cast<PFNGLLINKPROGRAMPROC>
                               (GetGLProc(glModule, "glLinkProgram"));
  glGetProgramiv             = reinterpret_cast<PFNGLGETPROGRAMIVPROC>
                               (GetGLProc(glModule, "glGetProgramiv"));
  glDeleteShader             = reinterpret_cast<PFNGLDELETESHADERPROC>
                               (GetGLProc(glModule, "glDeleteShader"));
  glActiveTexture            = reinterpret_cast<PFNGLACTIVETEXTUREPROC>
                               (GetGLProc(glModule, "glActiveTexture"));
  glCreateShader             = reinterpret_cast<PFNGLCREATESHADERPROC>
                               (GetGLProc(glModule, "glCreateShader"));
  glShaderSource             = reinterpret_cast<PFNGLSHADERSOURCEPROC>
                               (GetGLProc(glModule, "glShaderSource"));
  glCompileShader            = reinterpret_cast<PFNGLCOMPILESHADERPROC>
                               (GetGLProc(glModule, "glCompileShader"));
  glGetShaderiv              = reinterpret_cast<PFNGLGETSHADERIVPROC>
                               (GetGLProc(glModule, "glGetShaderiv"));
  glGetShaderInfoLog         = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>
                               (GetGLProc(glModule, "glGetShaderInfoLog"));
  glBindVertexArray          = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>
                               (GetGLProc(glModule, "glBindVertexArray"));
  glBindBuffer               = reinterpret_cast<PFNGLBINDBUFFERPROC>
                               (GetGLProc(glModule, "glBindBuffer"));
  glBufferData               = reinterpret_cast<PFNGLBUFFERDATAPROC>
                               (GetGLProc(glModule, "glBufferData"));
  glVertexAttribPointer      = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>
                               (GetGLProc(glModule, "glVertexAttribPointer"));
  glEnableVertexAttribArray  = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>
                               (GetGLProc(glModule, "glEnableVertexAttribArray"));
  glUniformMatrix4fv         = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>
                               (GetGLProc(glModule, "glUniformMatrix4fv"));
  glGetUniformLocation       = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>
                               (GetGLProc(glModule, "glGetUniformLocation"));
  glGenVertexArrays          = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>
                               (GetGLProc(glModule, "glGenVertexArrays"));
  glGenBuffers               = reinterpret_cast<PFNGLGENBUFFERSPROC>
                               (GetGLProc(glModule, "glGenBuffers"));
  glDeleteVertexArrays       = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>
                               (GetGLProc(glModule, "glDeleteVertexArrays"));
  glDeleteBuffers            = reinterpret_cast<PFNGLDELETEBUFFERSPROC>
                               (GetGLProc(glModule, "glDeleteBuffers"));

  if ((nullptr == wglGetExtensionsStringARB) ||
      (nullptr == wglChoosePixelFormatARB) ||
      (nullptr == wglCreateContextAttribsARB) ||
      (nullptr == glCreateProgram) ||
      (nullptr == glDeleteProgram) ||
      (nullptr == glAttachShader) ||
      (nullptr == glLinkProgram) ||
      (nullptr == glGetProgramiv) ||
      (nullptr == glDeleteShader) ||
      (nullptr == glActiveTexture) ||
      (nullptr == glCreateShader) ||
      (nullptr == glShaderSource) ||
      (nullptr == glCompileShader) ||
      (nullptr == glGetShaderiv) ||
      (nullptr == glGetShaderInfoLog) ||
      (nullptr == glBindVertexArray) ||
      (nullptr == glBindBuffer) ||
      (nullptr == glBufferData) ||
      (nullptr == glVertexAttribPointer) ||
      (nullptr == glEnableVertexAttribArray) ||
      (nullptr == glUniformMatrix4fv) ||
      (nullptr == glGetUniformLocation) ||
      (nullptr == glGenVertexArrays) ||
      (nullptr == glGenBuffers) ||
      (nullptr == glDeleteVertexArrays) ||
      (nullptr == glDeleteBuffers))
  {
    Status = OGL_API_LOAD_ERROR;
  }

  FreeLibrary(glModule);
}

// --------------------------------------------------------------------------------------
// Function: GetGLProc
// Notes: None
// --------------------------------------------------------------------------------------
void* OGLExt::GetGLProc(HMODULE hModule, const char* func)
{
  void* pFunc = reinterpret_cast<void*>(wglGetProcAddress(func));
  if (nullptr == pFunc)
  {
    pFunc = GetProcAddress(hModule, func);
  }

  return pFunc;
}

// --------------------------------------------------------------------------------------
// Function: OGLWndProc
// Notes: None
// --------------------------------------------------------------------------------------
LRESULT __stdcall OGLExt::OGLWndProc(HWND hWnd,
                                     uint32_t msg,
                                     WPARAM wParam,
                                     LPARAM lParam)
{
  static bool shouldQuit = false;
  switch (msg)
  {
    // ------------------------------------------
    // Window Creation/Destruction
    // ------------------------------------------
    case WM_CREATE:
    {
      // If the creation is the dummy window, complete initialization.
      if (0 == DummyWndHandle)
      {
        // Update Dummy Handle
        DummyWndHandle = hWnd; 

        // Complete OpenGL initialization
        FinalizeOpenGLInitialization();

        // Release dummy window and context
        wglMakeCurrent(0, 0);
        wglDeleteContext(OGLContext);
        ReleaseDC(DummyWndHandle, WindowContext);
        PostQuitMessage(0);
        return 0;
      }
    }
    break;

    case WM_DESTROY:
    {
      PostQuitMessage(0);
      return 0;
    }
    break;

    // ------------------------------------------
    // Draw Event
    // ------------------------------------------
    case WM_PAINT:
    {
      InvokeDrawCallback();
    }
    break;

    // ------------------------------------------
    // Move/Resize
    // ------------------------------------------
    case WM_MOVE:
    case WM_SIZE:
    {
      InvokeViewChangeCallback();
    }
    break;

    // ------------------------------------------
    // Input Handling
    // ------------------------------------------
    case WM_KEYUP:
    {
      InvokeKeyUpCallback(wParam);
    }
    break;

    case WM_KEYDOWN:
    {
      InvokeKeyDownCallback(wParam);
    }
    break;

    case WM_LBUTTONDOWN:
    {
      POINTS point = *reinterpret_cast<POINTS*>(&lParam);
      InvokeMLeftDownCallback(point.x, point.y);
    }
    break;

    case WM_LBUTTONUP:
    {
      POINTS point = *reinterpret_cast<POINTS*>(&lParam);
      InvokeMLeftUpCallback(point.x, point.y);
    }

    case WM_RBUTTONDOWN:
    {
      POINTS point = *reinterpret_cast<POINTS*>(&lParam);
      InvokeMRightDownCallback(point.x, point.y);
    }
    break;

    case WM_RBUTTONUP:
    {
      POINTS point = *reinterpret_cast<POINTS*>(&lParam);
      InvokeMRightUpCallback(point.x, point.y);
    }
    break;

    case WM_MOUSEMOVE:
    {
      POINTS point = *reinterpret_cast<POINTS*>(&lParam);
      InvokeMouseMoveCallback(point.x, point.y);
    }
    break;

    case MM_JOY1MOVE:
    case MM_JOY1BUTTONDOWN:
    case MM_JOY2BUTTONDOWN:
    case MM_JOY2MOVE:
    {
      InvokeJoystickCallback(msg, wParam, lParam);
    }
    break;
  }

  return DefWindowProcA(hWnd, msg, wParam, lParam);
}