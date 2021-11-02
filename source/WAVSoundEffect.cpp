#include "SoundEngine.h"
#include "WAVSoundEffect.h"
#include <cstring>
#include <dsound.h>
#include <mmsystem.h>
#include <stdio.h>
#include <vector>


// --------------------------------------------------------------------------------------
// Function: Play
// Notes: None
// --------------------------------------------------------------------------------------
bool WAVSoundEffect::Play(float volume, bool loop)
{
  // Before playing the sound, we need to adjust the primary buffer descriptor.
  WAVEFORMATEX waveFormat;
  waveFormat.nChannels = Header.numChannels;
  waveFormat.wFormatTag = Header.audioFormat;
  waveFormat.nSamplesPerSec = Header.sampleRate;
  waveFormat.wBitsPerSample = Header.bitsPerSample;
  waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
  waveFormat.cbSize = 0;

  if (true == SoundEngine::Instance()->SetFormat(&waveFormat))
  {
    return SoundEffect::Play(volume, loop);
  }

  return false;
}

// --------------------------------------------------------------------------------------
// Function: WAVSoundEffect
// Notes: None
// --------------------------------------------------------------------------------------
WAVSoundEffect::WAVSoundEffect(const char* path) :
  SoundEffect()
{
  // Open the file
  FILE* pFile = nullptr;
  if (0 != fopen_s(&pFile, path, "rb"))
  {
    return;
  }

  if (1 != fread(&Header, sizeof(WAVHeader), 1, pFile))
  {
    fclose(pFile);
    return;
  }

  // ---------------------------------------------
  // Validate WAV header
  // ---------------------------------------------
  if (0 != memcmp(Header.chunkId, "RIFF", 4)) // ChunkId == RIFF
  {
    fclose(pFile);
    return;
  }

  if (0 != memcmp(Header.format, "WAVE", 4)) // format == WAVE
  {
    fclose(pFile);
    return;
  }

  if (0 != memcmp(Header.subChunkId, "fmt ", 4)) // subChunkId == 'fmt '
  {
    fclose(pFile);
    return;
  }

  if (WAVE_FORMAT_PCM != Header.audioFormat) // Validate audio format
  {
    fclose(pFile);
    return;
  }

  if (0 != memcmp(Header.dataChunkId, "data", 4)) // Validate data chunk
  {
    fclose(pFile);
    return;
  }

  // WAV header has been validated, setup secondary buffer.
  WAVEFORMATEX waveFormat;
  waveFormat.nChannels = Header.numChannels;
  waveFormat.wFormatTag = Header.audioFormat;
  waveFormat.nSamplesPerSec = Header.sampleRate;
  waveFormat.wBitsPerSample = Header.bitsPerSample;
  waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
  waveFormat.cbSize = 0;

  // Setup buffer descriptor for secondary buffer
  DSBUFFERDESC bufferDesc;
  bufferDesc.dwSize = sizeof(DSBUFFERDESC);
  bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
  bufferDesc.dwBufferBytes = Header.dataSize;
  bufferDesc.dwReserved = 0;
  bufferDesc.lpwfxFormat = &waveFormat;
  bufferDesc.guid3DAlgorithm = GUID_NULL;

  // Secondary buffer requires a temporary buffer..
  IDirectSoundBuffer* tempBuffer = nullptr;
  IDirectSound* directSound = SoundEngine::Instance()->GetDirectSound();
  if (0 > directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr))
  {
    fclose(pFile);
    return;
  }

  if (0 > tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, 
                                     reinterpret_cast<void**>(&SoundBuffer)))
  {
    tempBuffer->Release();
    fclose(pFile);
    return;
  }

  // Release temporary buffer
  tempBuffer->Release();
  tempBuffer = nullptr;

  // Copy file contents to sound buffer
  fseek(pFile, sizeof(WAVHeader), SEEK_SET);
  
  std::vector<uint8_t> data(Header.dataSize, 0);
  if (Header.dataSize != fread(&data[0], 1, Header.dataSize, pFile))
  {
    tempBuffer->Release();
    fclose(pFile);
    return;
  }

  // Close file
  fclose(pFile);

  // Lock the sound buffer and write contents
  uint32_t bufferSize = 0;
  uint8_t* bufferPointer = nullptr;
  if (S_OK != SoundBuffer->Lock(0, 
                                Header.dataSize, 
                                reinterpret_cast<void**>(&bufferPointer), 
                                reinterpret_cast<DWORD*>(&bufferSize), 
                                nullptr, 
                                nullptr, 
                                0))
  {
    tempBuffer->Release();
    return;
  }

  // Write to buffer
  memcpy(bufferPointer, data.data(), data.size());

  // Unlock the sound buffer
  if (S_OK != SoundBuffer->Unlock(bufferPointer, bufferSize, nullptr, 0))
  {
    tempBuffer->Release();
    return;
  }

  SoundValid = true;
}

// --------------------------------------------------------------------------------------
// Function: Instance
// Notes: None
// --------------------------------------------------------------------------------------
WAVSoundEffect::~WAVSoundEffect()
{

}