#include "OGLTimedSprite.h"

// --------------------------------------------------------------------------------------
// Function: StartRender
// Notes: None
// --------------------------------------------------------------------------------------
void OGLTimedSprite::StartRender(uint32_t ticks)
{
  RemainingTime = ticks;
  EnableRender();
}

// --------------------------------------------------------------------------------------
// Function: Render
// Notes: None
// --------------------------------------------------------------------------------------
void OGLTimedSprite::Render(uint32_t texId)
{
  if (0 == RemainingTime)
  {
    DisableRender();
  }
  else
  {
    --RemainingTime;
    OGLSprite::Render(texId);
  }
}

// --------------------------------------------------------------------------------------
// Function: GetRemainingTicks
// Notes: None
// --------------------------------------------------------------------------------------
uint32_t OGLTimedSprite::GetRemainingTicks() const
{
  return RemainingTime;
}

// --------------------------------------------------------------------------------------
// Function: OGLTimedSprite
// Notes: None
// --------------------------------------------------------------------------------------
OGLTimedSprite::OGLTimedSprite(uint32_t programId,
                               const char* imagePath,
                               float windowW,
                               float windowH,
                               uint32_t hFrames,
                               uint32_t vFrames) :
  OGLSprite(programId, imagePath, windowW, windowH, hFrames, vFrames)
{
  DisableRender();
}

// --------------------------------------------------------------------------------------
// Function: OGLTimedSprite
// Notes: None
// --------------------------------------------------------------------------------------
OGLTimedSprite::OGLTimedSprite(uint32_t programId,
                               float windowW,
                               float windowH,
                               uint32_t pixelWidth,
                               uint32_t pixelHeight,
                               uint32_t hFrames,
                               uint32_t vFrames) :
  OGLSprite(programId, windowW, windowH, pixelWidth, pixelHeight, hFrames, vFrames)
{
  DisableRender();
}
// --------------------------------------------------------------------------------------
// Function: ~OGLTimedSprite
// Notes: None
// --------------------------------------------------------------------------------------
OGLTimedSprite::~OGLTimedSprite()
{

}