#include "IMGProcessor.h"
#include "OGLExt.h"
#include "OGLSprite.h"
#include <SOIL.h>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



// --------------------------------------------------------------------------------------
// Function: EnableRender
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::OGLSprite::EnableRender()
{
  RenderEnabled = true;
}

// --------------------------------------------------------------------------------------
// Function: DisableRender
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::OGLSprite::DisableRender()
{
  RenderEnabled = false;
}

// --------------------------------------------------------------------------------------
// Function: Render
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::Render(uint32_t texId)
{
  if (true == RenderEnabled)
  {
    // Use our program
    glUseProgram(ProgramId);

    // Bind our texture
    glBindTexture(GL_TEXTURE_2D, TextureId);

    // Bind Vertex Array Object
    glBindVertexArray(VertexArrayObj);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    // 
    if (true == UploadImage)
    {
      UploadImage = false;

      // Load Image Buffer into GPU
      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   SheetWidth,
                   SheetHeight,
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   ImageBuffer);
    }

    unsigned int indices[] = 
    { 
      0, 1, 3, // First triangle 
      1, 2, 3  // Second triangle
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VSize * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Color attribute
    // Disable for now, to update:
    // 1. Uncomment VerticeEntry struct
    // 2. Update TextureCoord attribute 
    // 3. Update Vertex Shader
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VSize * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);

    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VSize * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Perform transformations 
    // The transformations must occur in the following order:
    // 1. Scale
    // 2. Translate
    // 3. Rotate
    glm::mat4 transf = glm::mat4(1.0f);
    transf = glm::scale(transf, glm::vec3(ScaleWidth, ScaleHeight, 0.0f));
    transf = glm::translate(transf, glm::vec3(X, Y, 0.0f));
    transf = glm::rotate(transf, glm::radians(Degree), glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(TransformId, 1, GL_FALSE, glm::value_ptr(transf));
  }

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


// --------------------------------------------------------------------------------------
// Function: SetActiveFrame
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::SetActiveFrame(uint32_t frame)
{
  if (ActiveFrame < TotalFrames)
  {
    ActiveFrame = frame;

    // ----------------------------------------
    // Object Positions
    // Projection relative (this is confusing)
    // X (-1.0 .. 1.0)  // L -> R
    // Y (1.0  .. -1.0) // T -> B
    // ----------------------------------------
    Vertices[TOP_LEFT].x     = -1.0f;
    Vertices[TOP_LEFT].y     = -1.0f;
    Vertices[TOP_LEFT].z     = 0.0f;
    Vertices[TOP_RIGHT].x    = 1.0f;
    Vertices[TOP_RIGHT].y    = -1.0f;
    Vertices[TOP_RIGHT].z    = 0.0f;
    Vertices[BOTTOM_LEFT].x  = -1.0f;
    Vertices[BOTTOM_LEFT].y  = 1.0f;
    Vertices[BOTTOM_LEFT].z  = 0.0f;
    Vertices[BOTTOM_RIGHT].x = 1.0f;
    Vertices[BOTTOM_RIGHT].y = 1.0f;
    Vertices[BOTTOM_RIGHT].z = 0.0f;

    // ----------------------------------------
    // Texture Positions
    // ----------------------------------------
    Vertices[TOP_RIGHT].tX    = Textures[ActiveFrame].right  / SheetWidth;
    Vertices[TOP_RIGHT].tY    = Textures[ActiveFrame].bottom / SheetHeight;
    Vertices[BOTTOM_RIGHT].tX = Textures[ActiveFrame].right  / SheetWidth;
    Vertices[BOTTOM_RIGHT].tY = Textures[ActiveFrame].top    / SheetHeight;
    Vertices[BOTTOM_LEFT].tX  = Textures[ActiveFrame].left   / SheetWidth;
    Vertices[BOTTOM_LEFT].tY  = Textures[ActiveFrame].top    / SheetHeight;
    Vertices[TOP_LEFT].tX     = Textures[ActiveFrame].left   / SheetWidth;
    Vertices[TOP_LEFT].tY     = Textures[ActiveFrame].bottom / SheetHeight;

    // ----------------------------------------
    // Texture Colors
    // ----------------------------------------
/*
    Vertices[TOP_RIGHT].r    = 1.0f;
    Vertices[TOP_RIGHT].g    = 1.0f;
    Vertices[TOP_RIGHT].b    = 1.0f;
    Vertices[BOTTOM_RIGHT].r = 1.0f;
    Vertices[BOTTOM_RIGHT].g = 1.0f;
    Vertices[BOTTOM_RIGHT].b = 1.0f;
    Vertices[BOTTOM_LEFT].r  = 1.0f;
    Vertices[BOTTOM_LEFT].g  = 1.0f;
    Vertices[BOTTOM_LEFT].b  = 1.0f;
    Vertices[TOP_LEFT].r     = 1.0f;
    Vertices[TOP_LEFT].g     = 1.0f;
    Vertices[TOP_LEFT].b     = 1.0f;
*/
  }
}

// --------------------------------------------------------------------------------------
// Function: SetX
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::SetX(float x)
{
  AbsX = x;

  // The point of reference is the center of the sprite.
  // We need the center of the sprite to sit at the position we want.
  // We need to translate the absolute X to projection X
  X = (-1.0f + (2 * ((x + (PixelWidth / 2)) / WindowWidth))) / ScaleWidth;
}

// --------------------------------------------------------------------------------------
// Function: GetAbsX
// Notes: None
// --------------------------------------------------------------------------------------
float OGLSprite::GetAbsX() const
{
  return AbsX;
}

// --------------------------------------------------------------------------------------
// Function: SetY
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::SetY(float y)
{
  AbsY = y;

  // We need to translate the absolute Y to projection Y
  Y = (1.0f - (2 * ((y + (PixelHeight / 2)) / WindowHeight))) / ScaleHeight;
}

// --------------------------------------------------------------------------------------
// Function: GetAbsY
// Notes: None
// --------------------------------------------------------------------------------------
float OGLSprite::GetAbsY() const
{
 return AbsY;
}

// --------------------------------------------------------------------------------------
// Function: SetDegree
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::SetDegree(uint32_t degree)
{
  Degree = (degree % 360) * 1.0f;
}

// --------------------------------------------------------------------------------------
// Function: SetSize
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::SetSize(float w, float h)
{
  PixelWidth = w;
  PixelHeight = h;
  ScaleWidth = PixelWidth / WindowWidth;
  ScaleHeight = PixelHeight / WindowHeight;
}

// --------------------------------------------------------------------------------------
// Function: GetPixelWidth
// Notes: None
// --------------------------------------------------------------------------------------
float OGLSprite::GetPixelWidth() const
{
  return PixelWidth;
}

// --------------------------------------------------------------------------------------
// Function: GetPixelHeight
// Notes: None
// --------------------------------------------------------------------------------------
float OGLSprite::GetPixelHeight() const
{
  return PixelHeight;
}

// --------------------------------------------------------------------------------------
// Function: BackupImage
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::BackupImage()
{
  if (nullptr != BackupBuffer)
  {
    delete[] BackupBuffer;
  }

  BackupBuffer = new uint8_t[SheetWidth * SheetHeight * sizeof(uint32_t)];
  memcpy(BackupBuffer, ImageBuffer, SheetWidth * SheetHeight * sizeof(uint32_t));
}

// --------------------------------------------------------------------------------------
// Function: RestoreImage
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::RestoreImage()
{
  memcpy(ImageBuffer, BackupBuffer, SheetWidth * SheetHeight * sizeof(uint32_t));
  UploadImage = true;
}

// --------------------------------------------------------------------------------------
// Function: UpdateImage
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::UpdateImage(int32_t x, 
                            int32_t y,
                            int32_t w, 
                            int32_t h,
                            uint8_t r, 
                            uint8_t g, 
                            uint8_t b)
{
  if (nullptr != ImageBuffer)
  {
    if (((x + w) < SheetWidth) &&
        ((y + h) < SheetHeight))
    {
      // Update the image buffer
      for (int t = 0; t < h; ++t)
      {
        for (int l = 0; l < w; ++l)
        {
          uint32_t* rgbWord = reinterpret_cast<uint32_t*>(ImageBuffer);
          uint32_t newColor = (r << 24) | (g << 16) | (b << 8);
          rgbWord[((y + t) * SheetWidth) + x + l] = newColor;
        }
      }

      UploadImage = true;
    }
  }
}

// --------------------------------------------------------------------------------------
// Function: InvertImage
// Notes: None
// --------------------------------------------------------------------------------------
void OGLSprite::InvertImage()
{
  if (nullptr != ImageBuffer)
  {
    uint32_t totalRGBWords = SheetWidth * SheetHeight;
    for (uint32_t i = 0; i < totalRGBWords; ++i)
    {
      uint32_t* rgbWord = reinterpret_cast<uint32_t*>(ImageBuffer);
      rgbWord[i] = ~rgbWord[i];
    }

    UploadImage = true;
  }
}

// --------------------------------------------------------------------------------------
// Function: GetTextureId
// Notes: None
// --------------------------------------------------------------------------------------
uint32_t OGLSprite::GetTextureId() const
{
  return TextureId;
}

// --------------------------------------------------------------------------------------
// Function: OGLSprite
// Notes: None
// --------------------------------------------------------------------------------------
OGLSprite::OGLSprite(uint32_t programId,
                     const char* imagePath,
                     float windowW,
                     float windowH, 
                     uint32_t hFrames, 
                     uint32_t vFrames) :
  ProgramId(programId),
  TextureId(0),
  TransformId(glGetUniformLocation(ProgramId, "transform")),
  TranslateId(glGetUniformLocation(ProgramId, "translate")),
  ImageBuffer(nullptr),
  BackupBuffer(nullptr),
  X(0.0f),
  Y(0.0f),
  AbsX(0.0f),
  AbsY(0.0f),
  Degree(0.0f),
  RenderEnabled(true),
  ActiveFrame(0),
  FrameWidth(0.0f),
  FrameHeight(0.0f),
  WindowWidth(windowW),
  WindowHeight(windowH),
  HFrameCount(hFrames),
  VFrameCount(vFrames),
  TotalFrames(hFrames * vFrames)
{
  // Initialize Buffers
  glGenVertexArrays(1, &VertexArrayObj);
  glGenBuffers(1, &VertexBufferObj);
  glGenBuffers(1, &IndexBufferObj);

  // Bind Vertex Array Object
  glBindVertexArray(VertexArrayObj);

  // Initialize texture
  glGenTextures(1, &TextureId);
  glBindTexture(GL_TEXTURE_2D, TextureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Load the image using SOIL
  ImageBuffer = SOIL_load_image(imagePath, 
                                &SheetWidth, 
                                &SheetHeight, 
                                0, 
                                SOIL_LOAD_RGBA);
  if (nullptr == ImageBuffer)
  {
    MessageBoxA(0, "Failed loading resource", "Error", MB_OK);
    return;
  }

  UploadImage = true;

  // Initialize Textures
  FrameWidth = static_cast<float>(SheetWidth) / HFrameCount;
  FrameHeight = static_cast<float>(SheetHeight) / VFrameCount;

  Textures.resize(TotalFrames);
  for (size_t v = 0; v < VFrameCount; ++v)
  {
    for (size_t h = 0; h < HFrameCount; ++h)
    {
      int32_t index = (v * HFrameCount) + h;
      Textures[index].left = FrameWidth * h;
      Textures[index].right = Textures[index].left + FrameWidth;
      Textures[index].top = FrameHeight * v;
      Textures[index].bottom = Textures[index].top + FrameHeight;
    }
  }

  // Initialize first frame
  SetActiveFrame(0);
}

// --------------------------------------------------------------------------------------
// Function: ~OGLSprite
// Notes: None
// --------------------------------------------------------------------------------------
OGLSprite::~OGLSprite()
{
  glDeleteVertexArrays(1, &VertexArrayObj);
  glDeleteBuffers(1, &VertexBufferObj);
  glDeleteBuffers(1, &IndexBufferObj);
  glDeleteTextures(1, &TextureId);
  glDeleteProgram(ProgramId);

  SOIL_free_image_data(ImageBuffer);
}

