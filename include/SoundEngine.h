#pragma once
#include <map>
#include <stdint.h>

// Forward declarations
struct IDirectSound8;
struct IDirectSoundBuffer;
class SoundEffect;

class SoundEngine
{
public:
  
  static SoundEngine* Instance();
  static bool Initialize();
  static void Destroy();
  bool RegisterSound(const char* path, uint32_t& id);
  SoundEffect* GetSound(uint32_t id);
  bool Play(uint32_t id, float volume = 1.0f, bool loop = false);
  bool Stop(uint32_t id);
  bool IsActive(uint32_t id);
  bool SetFormat(void* format);
  IDirectSound8* GetDirectSound() const;

private:
  
  SoundEngine();
  ~SoundEngine();

  uint32_t SoundCount;
  IDirectSound8* DirectSound;
  IDirectSoundBuffer* PrimaryBuffer;
  std::map<uint32_t, SoundEffect*> Sounds;
};