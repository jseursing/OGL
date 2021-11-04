#pragma once
#include <stdint.h>
#include <vector>

// Forward declarations
class OGLSprite;

class OGLSpriteMgr
{
public:

  static OGLSpriteMgr* Instance();
  void Initialize(float windowW, float windowH);
  uint32_t BuildShaderGL(char* vertexCode, char* fragmentCode);
  OGLSprite* RegisterSprite(uint32_t programId,
                            const char* textureName,
                            const char* imagePath, 
                            uint32_t hFrames, 
                            uint32_t vFrames);
  OGLSprite* RegisterSprite(uint32_t programId,
                            const char* textureName,
                            uint32_t pixelWidth,
                            uint32_t pixelHeight, 
                            uint32_t hFrames, 
                            uint32_t vFrames);
  void RegisterSprite(OGLSprite* sprite, const char* textureName);
  OGLSprite* GetSprite(uint32_t index);
  void Render();

private:
  
  OGLSpriteMgr();
  ~OGLSpriteMgr();
  uint32_t CompileShaderGL(char* source, uint32_t type);


  float WindowWidth;
  float WindowHeight;
  std::vector<OGLSprite*> Sprites;
};