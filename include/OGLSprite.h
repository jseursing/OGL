#pragma once
#include <stdint.h>
#include <vector>


class OGLSprite
{
public:
  
  void EnableRender();
  void DisableRender();
  virtual void Render(uint32_t texId);
  void PostRenderProcess();
  uint32_t GetTextureId() const;
  void SetActiveFrame(uint32_t frame);
  virtual void SetX(float x);
  float GetAbsX() const;
  virtual void SetY(float y);
  float GetAbsY() const;
  void SetSize(float w, float h);
  void SetDegree(uint32_t degree);
  float GetPixelWidth() const;
  float GetPixelHeight() const;
  void BackupImage();
  void RestoreImage();
  void UpdateImage(int32_t x, 
                   int32_t y, 
                   int32_t w, 
                   int32_t h, 
                   uint8_t r, 
                   uint8_t g, 
                   uint8_t b);
  void InvertImage();
  
  OGLSprite(uint32_t programId,
            const char* imagePath, 
            float windowW,
            float windowH,
            uint32_t hFrames, 
            uint32_t vFrames);
  virtual ~OGLSprite();

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

  // OpenGL SL specific members
  uint32_t ProgramId;
  uint32_t TextureId;
  uint32_t TransformId;
  uint32_t TranslateId;
  uint32_t VertexArrayObj;
  uint32_t VertexBufferObj;
  uint32_t IndexBufferObj;
  uint8_t* ImageBuffer;
  uint8_t* BackupBuffer;
  uint32_t ActiveFrame;
  bool UploadImage;
  
  enum VerticeEnum
  {
    TOP_RIGHT,
    BOTTOM_RIGHT,
    BOTTOM_LEFT,
    TOP_LEFT,
    MAX_POINTS
  };

  struct VerticeEntry
  {
    float x;
    float y;
    float z;
//    float r;
//    float g;
//    float b;
    float tX;
    float tY;
  };
  VerticeEntry Vertices[MAX_POINTS]; 
  static const uint32_t VSize = sizeof(VerticeEntry) / sizeof(float);

  float X;
  float Y;
  float AbsX;
  float AbsY;
  float Degree;
  float PixelWidth;
  float PixelHeight;
  bool RenderEnabled;

  float FrameWidth;
  float FrameHeight;
  float WindowWidth;
  float WindowHeight;
  int32_t SheetWidth;
  int32_t SheetHeight;
  float ScaleWidth;
  float ScaleHeight;
  uint32_t HFrameCount;
  uint32_t VFrameCount;
  uint32_t TotalFrames;
};