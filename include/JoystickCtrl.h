#pragma once
#include <dinput.h>
#include <vector>


// Class definition
class JoystickCtrl
{
public:
  static JoystickCtrl* Instance();
  size_t QueryControllerDevices(size_t maxDevices = 0);
  bool CreateDeviceInterface(uint32_t devIndex);
  bool PollDevice(uint32_t devIndex, DIJOYSTATE* state);

private:
  
  static int __stdcall EnumDevicesCallback(const DIDEVICEINSTANCE* devInst, 
                                           void* param);
  static int __stdcall EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* objInst,
                                           void* param);
  JoystickCtrl();
  ~JoystickCtrl();

  IDirectInput8* DirectInput;
  std::vector<GUID> DeviceGUIDs;
  std::vector<LPDIRECTINPUTDEVICE8W> InputDevices;
};