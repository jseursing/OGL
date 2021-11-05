#pragma once
#include <stdint.h>

// Forward declarations
struct IDirectSoundBuffer8;

class SoundEffect
{
public:

  static SoundEffect* Create(const char* path);
  static void Destroy(SoundEffect*& sound);
  virtual bool Play(float volume = 1.0f, 
                    bool loop = false);
  bool Stop();
  bool IsActive();
  bool IsValid() const;

protected:

  SoundEffect();
  virtual ~SoundEffect();

  bool SoundValid;
  uint32_t RemainingTicks;
  IDirectSoundBuffer8* SoundBuffer;
};