#pragma once
#include "OGLSprite.h"


class OGLTimedSprite : public OGLSprite
{
public:
  void StartRender(uint32_t ticks);
  virtual void Render(uint32_t texId);
  OGLTimedSprite(uint32_t programId,
                 const char* imagePath,
                 float windowW,
                 float windowH,
                 uint32_t hFrames,
                 uint32_t vFrames);
  OGLTimedSprite(uint32_t programId,
                 float windowW,
                 float windowH,
                 uint32_t pixelWidth,
                 uint32_t pixelHeight,
                 uint32_t hFrames,
                 uint32_t vFrames);
  virtual ~OGLTimedSprite();

protected:

  struct Texture
  {
    float    left;
    float    top;
    float    right;
    float    bottom;
  };

  std::vector<Texture> Textures;

private:

  uint32_t RemainingTime;
};