#include "JoystickCtrl.h"
#include "OGLExt.h"

#if _DEBUG
#include <iostream>
#endif

// --------------------------------------------------------------------------------------
// Function: Instance
// Notes: None
// --------------------------------------------------------------------------------------
JoystickCtrl* JoystickCtrl::Instance()
{
  static JoystickCtrl instance;
  return &instance;
}

// --------------------------------------------------------------------------------------
// Function: QueryControllerDevices
// Notes: None
// --------------------------------------------------------------------------------------
size_t JoystickCtrl::QueryControllerDevices(size_t maxDevices)
{
  // Clear list
  DeviceGUIDs.clear();

  if (nullptr != DirectInput)
  {
    // Retrieve all available joysticks
    DirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, 
                             EnumDevicesCallback, 
                             &maxDevices, 
                             DIEDFL_ATTACHEDONLY);
  }

#if _DEBUG
  std::cout << __FUNCSIG__ 
            << ": Found " << DeviceGUIDs.size() << " devices" << std::endl;
#endif

  return DeviceGUIDs.size();
}

// --------------------------------------------------------------------------------------
// Function: CreateDeviceInterface
// Notes: None
// --------------------------------------------------------------------------------------
bool JoystickCtrl::CreateDeviceInterface(uint32_t devIndex)
{
  if (devIndex < DeviceGUIDs.size())
  {
    // Create Virtual Device
    LPDIRECTINPUTDEVICE8W device = nullptr;
    if (0 > DirectInput->CreateDevice(DeviceGUIDs[devIndex], &device, nullptr))
    {
      return false;
    }
      
    // Set data format
    if (0 > device->SetDataFormat(&c_dfDIJoystick))
    {
      return false;
    }

    // Set cooperative level 
    HWND hWnd = OGLExt::GetThisWindow();

    if (0 > device->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND))
    {
      return false;
    }

    // Enumerate objects for device in order to set properties
    if (0 > device->EnumObjects(EnumObjectsCallback, device, DIDFT_ALL))
    {
      return false;
    }

    InputDevices.push_back(device);
  
    return true;
  }

  return false;
}

// --------------------------------------------------------------------------------------
// Function: PollDevice
// Notes: None
// --------------------------------------------------------------------------------------
bool JoystickCtrl::PollDevice(uint32_t devIndex, DIJOYSTATE* state)
{
  size_t failCount = 10;

  if (devIndex < InputDevices.size())
  {
    HRESULT res = 0;

    // Attempt to poll the device
    if (0 > InputDevices[devIndex]->Poll())
    {
      // We lost the input stream. Attempt to re-acquire. If we continue to fail,
      // remove the device.
      res = InputDevices[devIndex]->Acquire();
      while (DIERR_INPUTLOST == res)
      {
        --failCount;
        if (0 == failCount)
        {
#if _DEBUG
          std::cout << __FUNCSIG__ 
                    << ": Failed to acquire device" << std::endl;
#endif
          InputDevices.erase(InputDevices.begin() + devIndex);
          break;
        }

        res = InputDevices[devIndex]->Acquire();
      }
    }
      
    // Retrieve device state
    if (0 <= res)
    {
      res = InputDevices[devIndex]->GetDeviceState(sizeof(DIJOYSTATE), state);
      if ((0 != failCount) &&
          (0 <= res))
      {
        return true;
      }
#if _DEBUG
      else
      {
        std::cout << __FUNCSIG__ 
                  << ": GetDeviceState() returned " << res << std::endl;
      }
#endif
    }
  }

  return false;
}

// --------------------------------------------------------------------------------------
// Function: EnumDevicesCallback
// Notes: None
// --------------------------------------------------------------------------------------
int __stdcall JoystickCtrl::EnumDevicesCallback(const DIDEVICEINSTANCE* devInst,
                                                void* param)
{
  JoystickCtrl* instance = Instance();
  size_t maxDevices = *reinterpret_cast<size_t*>(param);
  instance->DeviceGUIDs.push_back(devInst->guidInstance);
  
#if _DEBUG
  char productName[MAX_PATH] = {0};
  for (size_t i = 0; devInst->tszProductName[i]; ++i)
  {
    productName[i] = devInst->tszProductName[i];
  }

  std::cout << __FUNCSIG__ 
            << ": Product Name=" << productName << std::endl;
#endif

  if ((0 != maxDevices) &&
      (instance->DeviceGUIDs.size() >= maxDevices))
  {
    return DIENUM_STOP;
  }

  return DIENUM_CONTINUE;
}

// --------------------------------------------------------------------------------------
// Function: EnumObjectsCallback
// Notes: None
// --------------------------------------------------------------------------------------
int __stdcall JoystickCtrl::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* objInst,
                                                void* param)
{
  // Set joystick properties
  if (DIDFT_AXIS & objInst->dwType)
  {
    DIPROPRANGE properties;
    properties.diph.dwSize = sizeof(DIPROPRANGE);
    properties.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    properties.diph.dwHow = DIPH_BYID;
    properties.diph.dwObj = objInst->dwType;
    
    // We only care about directions..
    properties.lMin = -100;
    properties.lMax = 100;

    LPDIRECTINPUTDEVICE8W device = reinterpret_cast<LPDIRECTINPUTDEVICE8W>(param);
    if (0 < device->SetProperty(DIPROP_RANGE, &properties.diph))
    {
      return DIENUM_STOP;
    }
  }

  return DIENUM_CONTINUE;
}

// --------------------------------------------------------------------------------------
// Function: JoystickCtrl
// Notes: None
// --------------------------------------------------------------------------------------
JoystickCtrl::JoystickCtrl()
{
  DirectInput = nullptr;
  DirectInput8Create(GetModuleHandleA(nullptr), 
                     DIRECTINPUT_VERSION, 
                     IID_IDirectInput8, 
                     reinterpret_cast<void**>(&DirectInput), 
                     nullptr);
}

// --------------------------------------------------------------------------------------
// Function: ~JoystickCtrl
// Notes: None
// --------------------------------------------------------------------------------------
JoystickCtrl::~JoystickCtrl()
{

}