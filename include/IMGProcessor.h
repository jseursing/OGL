#pragma once
#include <stdint.h>

class IMGProcessor
{
public:
  enum ImageFormatType
  {
    IMAGE_RGB,
    IMAGE_RGBA
  };

  static bool ReadImage(const char* path, 
                        uint8_t*& buffer,
                        int32_t& width,
                        int32_t& height);

private:

  static bool ReadBitmap(const char* path,
                         uint8_t*& buffer,
                         int32_t& width,
                         int32_t& height);

  static bool ReadPNG(const char* path,
                      uint8_t*& buffer,
                      int32_t& width,
                      int32_t& height);
};