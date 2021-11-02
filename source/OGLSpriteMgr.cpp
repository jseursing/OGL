#include "OGLExt.h"
#include "OGLSpriteMgr.h"
#include "OGLSprite.h"
#include <Windows.h>

// --------------------------------------------------------------------------------------
// Function: Instance
// Notes: None
// --------------------------------------------------------------------------------------
OGLSpriteMgr* OGLSpriteMgr::Instance()
{
  static OGLSpriteMgr instance;
  return &instance;
}

// --------------------------------------------------------------------------------------
// Function: Initialize
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSpriteMgr::Initialize(float windowW, float windowH)
{
  WindowWidth = windowW;
  WindowHeight = windowH;
}

// --------------------------------------------------------------------------------------
// Function: BuildShaderGL
// Notes: None
// --------------------------------------------------------------------------------------
uint32_t OGLSpriteMgr::BuildShaderGL(char* vertexCode, char* fragmentCode)
{
  uint32_t vertexHandle = CompileShaderGL(vertexCode, GL_VERTEX_SHADER);
  if (0 == vertexHandle)
  {
    MessageBoxA(0, "Failed compiling Vertex Shader", "Error", MB_OK);
    return 0;
  }

  uint32_t fragmentHandle = CompileShaderGL(fragmentCode, GL_FRAGMENT_SHADER);
  if (0 == vertexHandle)
  {
    MessageBoxA(0, "Failed compiling Fragment Shader", "Error", MB_OK);
    return 0;
  }

  uint32_t programId = glCreateProgram();
  glAttachShader(programId, vertexHandle);
  glAttachShader(programId, fragmentHandle);
  glLinkProgram(programId);

  int32_t success;
  char infoLog[512] = { 0 };
  glGetProgramiv(programId, GL_LINK_STATUS, &success);
  if (0 == success)
  {
    MessageBoxA(0, "Failed linking GL Shaders", "Error", MB_OK);
    glDeleteProgram(programId);
  }

  glDeleteShader(vertexHandle);
  glDeleteShader(fragmentHandle);

  return programId;
}

// --------------------------------------------------------------------------------------
// Function: RegisterSprite
// Notes: None
// --------------------------------------------------------------------------------------
OGLSprite* OGLSpriteMgr::RegisterSprite(uint32_t programId,
                                        const char* textureName,
                                        const char* imagePath,
                                        uint32_t hFrames,
                                        uint32_t vFrames)
{
  // Texture Name is not used for now. But it's left here in case custom code
  // is to be written to GLSL which can then be retrieved by the textureName.
  Sprites.emplace_back(new OGLSprite(programId, 
                                     imagePath,
                                     WindowWidth, 
                                     WindowHeight, 
                                     hFrames, 
                                     vFrames));
  return Sprites.back();
}

// --------------------------------------------------------------------------------------
// Function: RegisterSprite
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSpriteMgr::RegisterSprite(OGLSprite* sprite, const char* textureName)
{
  Sprites.emplace_back(sprite);
}

// --------------------------------------------------------------------------------------
// Function: RegisterSprite
// Notes: None
// --------------------------------------------------------------------------------------
OGLSprite* OGLSpriteMgr::GetSprite(uint32_t index)
{
  if (index >= Sprites.size())
  {
    return nullptr;
  }

  return Sprites[index];
}

// --------------------------------------------------------------------------------------
// Function: Render
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSpriteMgr::Render()
{
  // Build our vertices..
  for (size_t i = 0; i < Sprites.size(); ++i)
  {
    glActiveTexture(GL_TEXTURE0);
    Sprites[i]->Render(i); 
  }
}

// --------------------------------------------------------------------------------------
// Function: OGLSpriteMgr
// Notes: None
// --------------------------------------------------------------------------------------
OGLSpriteMgr::OGLSpriteMgr()
{

}

// --------------------------------------------------------------------------------------
// Function: ~OGLSpriteMgr
// Notes: None
// --------------------------------------------------------------------------------------
OGLSpriteMgr::~OGLSpriteMgr()
{
}

// --------------------------------------------------------------------------------------
// Function: CompileShaderGL
// Notes: None
// --------------------------------------------------------------------------------------
uint32_t OGLSpriteMgr::CompileShaderGL(char* source, uint32_t type)
{
  uint32_t shaderHandle = glCreateShader(type);
  glShaderSource(shaderHandle, 1, &source, 0);
  glCompileShader(shaderHandle);

  int32_t success;
  char infoLog[512] = { 0 };
  glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
  if (0 == success)
  {
    int32_t logLen = 0;
    glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLen);
    if (0 < logLen)
    {
      std::vector<char> logData(logLen + 1);
      glGetShaderInfoLog(shaderHandle, logLen, nullptr, &logData[0]);
      printf(logData.data());
    }
    return 0;
  }

  return shaderHandle;
}