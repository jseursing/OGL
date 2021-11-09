#include "OGLApp.h"
#include "OGLExt.h"
#include <ctime>
#include <thread>


// Static definitions
OGLApp* OGLApp::MyInstance = nullptr;

// --------------------------------------------------------------------------------------
// Function: MainLoop
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::MainLoop()
{
  // Enter main loop
  OGLExt::MainLoop();
}

// --------------------------------------------------------------------------------------
// Function: ResetDisplayTick
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::ResetDisplayTick()
{
  DisplayTick = 0;
}

// --------------------------------------------------------------------------------------
// Function: SetBackgroundColor
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::SetBackgroundColor(float r, float g, float b, float alpha)
{
  glClearColor(r, g, b, alpha);
}

// --------------------------------------------------------------------------------------
// Function: SetDimensions
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::SetDimensions(int l, int r, int t, int b)
{
  SetRect(&Dimensions, l, t, r, b);
}

// --------------------------------------------------------------------------------------
// Function: GetDimensions
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::GetDimensions(RECT& rect)
{
  memcpy(&rect, &Dimensions, sizeof(RECT));
}

// --------------------------------------------------------------------------------------
// Function: SetWindowTitle
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::SetWindowTitle(const char* title)
{
  strcpy_s(WindowTitle, title);
}

// --------------------------------------------------------------------------------------
// Function: SetRefreshRate
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::SetRefreshRate(uint32_t refreshRate)
{
  RefreshRate = refreshRate;
}

// --------------------------------------------------------------------------------------
// Function: OGLApp
// Notes: None
// --------------------------------------------------------------------------------------
OGLApp::OGLApp(int argc, char** argv) :
  DisplayTick(0)
{
  // Set MyInstance
  MyInstance = this;

  // Initialize GL library
  if (OGLExt::OGL_SUCCESS != OGLExt::CreateContext(OGLExt::OGL_DBL_BUFFER))
  {
    MessageBoxA(0, "OGLInit failed", "ERROR", MB_ICONERROR);
  }
}

// --------------------------------------------------------------------------------------
// Function:~OGLApp
// Notes: None
// --------------------------------------------------------------------------------------
OGLApp::~OGLApp()
{

}

// --------------------------------------------------------------------------------------
// Function: Initialize
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::Initialize(OriginTypeEnum origin)
{
  // Initialize window
  OGLExt::InitWindow("GhostSchool",
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      Dimensions.right - Dimensions.left, 
                      Dimensions.bottom - Dimensions.top);
  OGLExt::RegisterKeyDown(reinterpret_cast<OGLKeyDownFunc>(KeyHandler));
  OGLExt::RegisterIdle(IdleHandler);
  OGLExt::RegisterDraw(Display);

  if (OGLExt::OGL_SUCCESS != OGLExt::GetLastStatus())
  {
    MessageBoxA(0, "OGLInit failed", "ERROR", MB_ICONERROR);
    return;
  }

  // Assign background color as black
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // Enable alpha blending
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set Picture default variables
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Set Draw origin
  int width = Dimensions.right - Dimensions.left;
  int height = Dimensions.bottom - Dimensions.top;

  switch (origin)
  {
    case MIDPOINT:
    {
      int mWidth = (Dimensions.right - Dimensions.left) / 2;
      int mHeight = (Dimensions.bottom - Dimensions.top) / 2;

      gluOrtho2D(-mWidth, mWidth, -mHeight, mHeight);
    }
    break;
    case TOP_LEFT:
    {
      gluOrtho2D(0, width, height, 0);
    }
    break;
  }
}

// --------------------------------------------------------------------------------------
// Function: Display
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::Display()
{
  MyInstance->DrawGraphics(MyInstance->DisplayTick++);
}

// --------------------------------------------------------------------------------------
// Function: RefreshDisplay
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::IdleHandler()
{
  MyInstance->OnIdle();
}

// --------------------------------------------------------------------------------------
// Function: KeyHandler
// Notes: None
// --------------------------------------------------------------------------------------
void OGLApp::KeyHandler(int32_t key)
{
  MyInstance->OnKeyPressed(key);
}
