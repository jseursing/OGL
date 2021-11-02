#include "SoundEffect.h"
#include "WAVSoundEffect.h"
#include <dsound.h>
#include <mmsystem.h>
#include <string>
#include <Windows.h>


// --------------------------------------------------------------------------------------
// Function: Instance
// Notes: None
// --------------------------------------------------------------------------------------
SoundEffect* SoundEffect::Create(const char* path)
{
  SoundEffect* sound = nullptr;

  // WAV file is the only one implemented..
  if (std::string::npos != std::string(path).find(".wav"))
  {
    sound = new WAVSoundEffect(path);
  }

  if ((nullptr != sound) &&
      (false == sound->IsValid()))
  {
    Destroy(sound);
  }

  return sound;
}

// --------------------------------------------------------------------------------------
// Function: Instance
// Notes: None
// --------------------------------------------------------------------------------------
void SoundEffect::Destroy(SoundEffect*& sound)
{
  delete sound;
  sound = nullptr;
}

// --------------------------------------------------------------------------------------
// Function: Instance
// Notes: None
// --------------------------------------------------------------------------------------
bool SoundEffect::Play(float volume, bool loop)
{
  if (nullptr == SoundBuffer)
  {
    return false;
  }

  Stop(); // Stop just in case

  long adjVolume = static_cast<long>(DSBVOLUME_MIN + (volume * abs(DSBVOLUME_MIN)));
  SoundBuffer->SetVolume(adjVolume);  // Set Volume
  SoundBuffer->SetCurrentPosition(0); // Reset position

  return (S_OK == SoundBuffer->Play(0, 0, true == loop ? DSBPLAY_LOOPING : 0));
}

// --------------------------------------------------------------------------------------
// Function: Instance
// Notes: None
// --------------------------------------------------------------------------------------
bool SoundEffect::Stop()
{
  if (nullptr == SoundBuffer)
  {
    return false;
  }

  bool success = true;
  if (true == IsActive())
  {
    success = (0 <= SoundBuffer->Stop());
  }

  return success;
}

// --------------------------------------------------------------------------------------
// Function: IsActive
// Notes: None
// --------------------------------------------------------------------------------------
bool SoundEffect::IsActive() const
{
  if (nullptr == SoundBuffer)
  {
    return false;
  }

  DWORD status = 0;
  SoundBuffer->GetStatus(&status);
  return DSBSTATUS_PLAYING == status;
}

// --------------------------------------------------------------------------------------
// Function: IsValid
// Notes: None
// --------------------------------------------------------------------------------------
bool SoundEffect::IsValid() const
{
  return SoundValid;
}

// --------------------------------------------------------------------------------------
// Function: Instance
// Notes: None
// --------------------------------------------------------------------------------------
SoundEffect::SoundEffect() :
  SoundValid(false),
  SoundBuffer(nullptr)
{

}

// --------------------------------------------------------------------------------------
// Function: Instance
// Notes: None
// --------------------------------------------------------------------------------------
SoundEffect::~SoundEffect()
{
  if (nullptr != SoundBuffer)
  {
    SoundBuffer->Release();
    SoundBuffer = nullptr;
  }
}