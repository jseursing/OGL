#pragma once
#include <future>
#include <stdint.h>
#include <Windows.h>


class OGLApp
{
public:

  // OpenGL Origin
  enum OriginTypeEnum
  {
    MIDPOINT,
    TOP_LEFT
  };

  void MainLoop();
  void ResetDisplayTick();
  void SetBackgroundColor(float r, float g, float b, float alpha);
  void SetDimensions(int l, int r, int t, int b);
  void GetDimensions(RECT& rect);
  void SetWindowTitle(const char* title);
  void SetRefreshRate(uint32_t refreshRate);
  OGLApp(int argc, char** argv);
  ~OGLApp();


protected:

  virtual void Initialize(OriginTypeEnum origin);
  virtual void DrawGraphics(uint32_t tick) = 0;
  virtual void OnKeyPressed(int32_t key) = 0;

  static OGLApp* MyInstance;

private:
  
  static void Display();
  static void RefreshDisplay();
  static void KeyHandler(int32_t key);


  std::future<void> ThreadHandle;

  RECT Dimensions;
  uint32_t RefreshRate;
  uint32_t DisplayTick;
  char WindowTitle[128];
};
