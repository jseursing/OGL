#include "IMGProcessor.h"
#include <atlimage.h>
#include <string>
#include <windows.h>


// --------------------------------------------------------------------------------------
// Function: ReadImage
// Notes: None
// --------------------------------------------------------------------------------------
bool IMGProcessor::ReadImage(const char* path,
                             uint8_t*& buffer,
                             int32_t& width,
                             int32_t& height)
{
  // Reset output
  buffer = nullptr;
  width = 0;
  height = 0;

  // Bitmap
  if (std::string::npos != std::string(path).find(".bmp"))
  {
    return ReadBitmap(path, buffer, width, height);
  }

  // PNG
  if (std::string::npos != std::string(path).find(".png"))
  {
    return ReadPNG(path, buffer, width, height);
  }

  return false;
}


// --------------------------------------------------------------------------------------
// Function: ReadBitmap
// Notes: None
// --------------------------------------------------------------------------------------
bool IMGProcessor::ReadBitmap(const char* path,
                              uint8_t*& buffer,
                              int32_t& width,
                              int32_t& height)
{
  FILE* pFile = nullptr;
  if (0 != fopen_s(&pFile, path, "rb"))
  {
    return false;
  }

  // Retrieve bitmap header
  BITMAPFILEHEADER bitmapHeader;
  if (1 != fread(&bitmapHeader, sizeof(BITMAPFILEHEADER), 1, pFile))
  {
    fclose(pFile);
    return false;
  }

  // Retrieve bitmap info header
  BITMAPINFOHEADER bitmapInfoHdr;
  fseek(pFile, sizeof(BITMAPFILEHEADER), SEEK_SET);
  if (1 != fread(&bitmapInfoHdr, sizeof(BITMAPINFOHEADER), 1, pFile))
  {
    fclose(pFile);
    return false;
  }

  // Validate header contents
  if (0x4D42 != bitmapHeader.bfType)
  {
    fclose(pFile);
    return false;
  }

  // Allocate buffer and read
  buffer = new uint8_t[bitmapInfoHdr.biSizeImage];
  
  fseek(pFile, sizeof(BITMAPINFOHEADER), SEEK_CUR);
  if (bitmapInfoHdr.biSizeImage != fread(buffer, 1, bitmapInfoHdr.biSizeImage, pFile))
  {
    delete[] buffer;
    buffer = nullptr;
    fclose(pFile);
    return false;
  }
  
  // Close the file
  fclose(pFile);

  // Update dimensions
  width = bitmapInfoHdr.biWidth;
  height = bitmapInfoHdr.biHeight;
  
  // Swap RGB bytes due to Bitmap storing as BGR
  for (size_t i = 0; i < bitmapInfoHdr.biSizeImage; i += 3)
  {
    uint8_t rgb[] = 
    {
      buffer[i + 2],
      buffer[i + 1],
      buffer[i]
    };
    memcpy(&buffer[i], rgb, sizeof(rgb));
  }

  return true;
}


// --------------------------------------------------------------------------------------
// Function: ReadPNG
// Notes: None
// --------------------------------------------------------------------------------------
bool IMGProcessor::ReadPNG(const char* path,
                           uint8_t*& buffer,
                           int32_t& width,
                           int32_t& height)
{
  // PNG files are complicated, so we'll use CImage to carry us...
  CImage png;
  if (S_OK != png.Load(CA2W(path)))
  {
    return false;
  }

  // Copy over data
  width = png.GetWidth();
  height = png.GetHeight();
  
  uint32_t totalSize = width * height * sizeof(uint32_t);
  buffer = new uint8_t[totalSize];

  uint32_t pixelIdx = 0;
  png.SetHasAlphaChannel(true);
  for (int32_t y = 0; y < height; ++y)
  {
    for (int32_t x = 0; x < width; ++x)
    {
      buffer[pixelIdx++] = *reinterpret_cast<uint32_t*>(png.GetPixelAddress(x, y));
    }
  }

  // Free image, return
  png.Destroy();

  return true;
}