#include "OGLExt.h"
#include "SoundEffect.h"
#include "SoundEngine.h"
#include <dsound.h>
#include <mmsystem.h>
#include <Windows.h>

// --------------------------------------------------------------------------------------
// Function: Instance
// Notes: None
// --------------------------------------------------------------------------------------
SoundEngine* SoundEngine::Instance()
{
  static SoundEngine instance;
  return &instance;
}

// --------------------------------------------------------------------------------------
// Function: Initialize
// Notes: None
// --------------------------------------------------------------------------------------
bool SoundEngine::Initialize()
{
  SoundEngine* instance = SoundEngine::Instance();

  // Initialize direct sound interface
  HRESULT hResult = DirectSoundCreate8(nullptr, &instance->DirectSound, nullptr);
  if (S_OK != hResult)
  {
    return false;
  }

  // Set cooperative level to priority to allow format change
  HWND hWnd = GetForegroundWindow();
  if (0 == hWnd)
  {
    hWnd = GetDesktopWindow();
  }

  hResult = instance->DirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
  if (S_OK != hResult)
  {
    return false;
  }

  // Setup primary buffer to allow volume change
  DSBUFFERDESC bufferDesc =
  {
    sizeof(DSBUFFERDESC),                       // dwSize
    DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME, // dwFlags
    0,                                          // dwBufferBytes
    0,                                          // dwReserved
    0,                                          // lpwfxFormat
#if DIRECTSOUND_VERSION >= 0x0700
    GUID_NULL                                   // guid3DAlgorithm;
#endif
  };

  hResult = instance->DirectSound->CreateSoundBuffer(&bufferDesc, 
                                                     &instance->PrimaryBuffer, 
                                                     nullptr);

  return (S_OK == hResult);
}

// --------------------------------------------------------------------------------------
// Function: Destroy
// Notes: None
// --------------------------------------------------------------------------------------
void SoundEngine::Destroy()
{
  SoundEngine* instance = SoundEngine::Instance();

  // Release buffer
  if (nullptr != instance->PrimaryBuffer)
  {
    instance->PrimaryBuffer->Release();
    instance->PrimaryBuffer = nullptr;
  }

  // Release sound interface
  if (nullptr != instance->DirectSound)
  {
    instance->DirectSound->Release();
    instance->DirectSound = nullptr;
  }
}

// --------------------------------------------------------------------------------------
// Function: RegisterSound
// Notes: None
// --------------------------------------------------------------------------------------
bool SoundEngine::RegisterSound(const char* path, uint32_t& id)
{
  SoundEffect* sound = SoundEffect::Create(path);
  if (nullptr == sound)
  {
    return false;
  }

  id = ++SoundCount;
  Sounds[id] = sound;

  return true;
}

// --------------------------------------------------------------------------------------
// Function: GetSound
// Notes: None
// --------------------------------------------------------------------------------------
SoundEffect* SoundEngine::GetSound(uint32_t id)
{
  if (Sounds.end() == Sounds.find(id))
  {
    return nullptr;
  }

  return Sounds[id];
}

// --------------------------------------------------------------------------------------
// Function: Play
// Notes: None
// --------------------------------------------------------------------------------------
bool SoundEngine::Play(uint32_t id, float volume, bool loop)
{
  SoundEffect* sound = GetSound(id);
  if (nullptr == sound)
  {
    return false;
  }

  return sound->Play(volume, loop);
}

// --------------------------------------------------------------------------------------
// Function: Stop
// Notes: None
// --------------------------------------------------------------------------------------
bool SoundEngine::Stop(uint32_t id)
{
  SoundEffect* sound = GetSound(id);
  if (nullptr == sound)
  {
    return false;
  }

  return sound->Stop();
}

// --------------------------------------------------------------------------------------
// Function: IsActive
// Notes: None
// --------------------------------------------------------------------------------------
bool SoundEngine::IsActive(uint32_t id)
{
  SoundEffect* sound = GetSound(id);
  if (nullptr == sound)
  {
    return false;
  }

  return sound->IsActive();
}

// --------------------------------------------------------------------------------------
// Function: SetFormat
// Notes: None
// --------------------------------------------------------------------------------------
bool SoundEngine::SetFormat(void* format)
{
  return S_OK == PrimaryBuffer->SetFormat(reinterpret_cast<WAVEFORMATEX*>(format));
}

// --------------------------------------------------------------------------------------
// Function: GetDirectSound
// Notes: None
// --------------------------------------------------------------------------------------
IDirectSound8* SoundEngine::GetDirectSound() const
{
  return DirectSound;
}

// --------------------------------------------------------------------------------------
// Function: SoundEngine
// Notes: None
// --------------------------------------------------------------------------------------
SoundEngine::SoundEngine() :
  SoundCount(0),
  DirectSound(nullptr),
  PrimaryBuffer(nullptr)
{
}

// --------------------------------------------------------------------------------------
// Function: ~SoundEngine
// Notes: None
// --------------------------------------------------------------------------------------
SoundEngine::~SoundEngine()
{

}