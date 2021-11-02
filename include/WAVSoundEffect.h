#pragma once
#include "SoundEffect.h"


class WAVSoundEffect : public SoundEffect
{
public:

  virtual bool Play(float volume, bool loop);
  WAVSoundEffect(const char* path);
  virtual ~WAVSoundEffect();

private:
  
  struct WAVHeader
  {
    uint8_t chunkId[4];
    uint32_t chunkSize;
    uint8_t format[4];
    uint8_t subChunkId[4];
    uint32_t subChunkSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t bytesPerSec;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint8_t dataChunkId[4];
    uint32_t dataSize;
  };

  WAVHeader Header;
};