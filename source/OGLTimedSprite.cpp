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
// Function: SetCharacterMapping
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
// Function: SetCharacterMapping
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
// Function: SetCharacterMapping
// Notes: None
// --------------------------------------------------------------------------------------
OGLTimedSprite::~OGLTimedSprite()
{

}