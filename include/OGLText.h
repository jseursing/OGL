#pragma once
#include "OGLSprite.h"
#include <map>
#include <string>

class OGLText : public OGLSprite
{
public:

  void SetCharacterMapping(char c, uint32_t frame);
  void SetCurrentText(const char* text);
  virtual void Render(uint32_t texId);
  virtual void SetX(float x);
  virtual void SetY(float y);
  void SetSpacing(float px);
  void RegisterCharSet(char firstChar, 
                       char lastChar, 
                       uint32_t offset);
  OGLText(uint32_t programId,
          const char* imagePath,
          float windowW,
          float windowH,
          uint32_t hFrames,
          uint32_t vFrames);
  virtual ~OGLText();

private:

  float TextX;
  float TextY;
  float Spacing;
  std::map<char, uint32_t> CharMap;
  std::string CurrentText;
  
};