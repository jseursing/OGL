#include "OGLText.h"


// --------------------------------------------------------------------------------------
// Function: SetCharacterMapping
// Notes: None
// --------------------------------------------------------------------------------------
void OGLText::SetCharacterMapping(char c, uint32_t frame)
{
  CharMap[c] = frame;
}

// --------------------------------------------------------------------------------------
// Function: SetCurrentText
// Notes: None
// --------------------------------------------------------------------------------------
void OGLText::SetCurrentText(const char* text)
{
  CurrentText = text;
}

// --------------------------------------------------------------------------------------
// Function: Render
// Notes: None
// --------------------------------------------------------------------------------------
void OGLText::Render(uint32_t texId)
{
  // The way we are rendering text is to rend the bitmap once for each
  // character in CurrentText.
  for (size_t i = 0; i < CurrentText.length(); ++i)
  {
    // Update position
    OGLSprite::SetX(TextX + (Spacing * i));

    // Usually a space isn't found in a font bitmap
    if (' ' != CurrentText[i])
    {
      // Do not continue if we have an unrecognized character.
      if (CharMap.end() == CharMap.find(CurrentText[i]))
      {
        break;
      }

      // Update activa frame to match the current character
      SetActiveFrame(CharMap[CurrentText[i]]);

      // Render the character
      OGLSprite::Render(texId);
    }
  }
}

// --------------------------------------------------------------------------------------
// Function: SetX
// Notes: None
// --------------------------------------------------------------------------------------
void OGLText::SetX(float x)
{
  TextX = x;
  OGLSprite::SetX(TextX);
}

// --------------------------------------------------------------------------------------
// Function: SetY
// Notes: None
// --------------------------------------------------------------------------------------
void OGLText::SetY(float y)
{
  TextY = y;
  OGLSprite::SetY(TextY);
}

// --------------------------------------------------------------------------------------
// Function: SetSpacing
// Notes: None
// --------------------------------------------------------------------------------------
void OGLText::SetSpacing(float px)
{
  Spacing = px;
}

// --------------------------------------------------------------------------------------
// Function: RegisterCharSet
// Notes: None
// --------------------------------------------------------------------------------------
void OGLText::RegisterCharSet(char firstChar,
                              char lastChar,
                              uint32_t offset)
{
  for (char c = firstChar; c <= lastChar; ++c) 
  {
    SetCharacterMapping(c, offset + c - firstChar);
  }
}

// --------------------------------------------------------------------------------------
// Function: OGLText
// Notes: None
// --------------------------------------------------------------------------------------
OGLText::OGLText(uint32_t programId,
                 const char* imagePath,
                 float windowW,
                 float windowH,
                 uint32_t hFrames,
                 uint32_t vFrames) :
  OGLSprite(programId, imagePath, windowW, windowH, hFrames, vFrames),
  TextX(0),
  TextY(0),
  Spacing(0),
  CurrentText("")
{
}

// --------------------------------------------------------------------------------------
// Function: ~OGLText
// Notes: None
// --------------------------------------------------------------------------------------
OGLText::~OGLText()
{

}