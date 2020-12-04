/**
* Copyright (C) 2020 Elisha Riedlinger
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/

#include "..\dinputto8.h"

HRESULT m_IDirectInputA::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
{
	return ProxyInterface->QueryInterface(ReplaceIIDUnknown(riid, WrapperID), ppvObj, DirectXVersion);
}

ULONG m_IDirectInputA::AddRef()
{
	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputA::Release()
{
	return ProxyInterface->Release();
}

HRESULT m_IDirectInputA::CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	return ProxyInterface->CreateDeviceExA(rguid, IID_IDirectInputDeviceA, (LPDIRECTINPUTDEVICE8A *)lplpDirectInputDevice, pUnkOuter);
}

HRESULT m_IDirectInputA::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	// Instead of directly calling the ProxyInterface here, we are adding some extra code to sort the devices. This is an exclusive fix
	// for a known bug in Rayman 2 that would prevent any gamepad from working. The fix has been extracted from this old VS 2010 project:
	// https://code.google.com/archive/p/noser-sandbox/source/default/source
	// There is only one download link with a zip file containing other projects too. You will find the extracted code in the
	// Rayman2InputFix_DirectInputA.cpp file from the Rayman2InputFix project.
	// Thanks to Nolan Check for the fix!

	// The bug: Rayman 2 expects EnumDevices to give results in a certain
	// order, where gamepads come before the keyboard. DirectInput makes
	// no guarantee about the order.
	// The fix: Call DirectInput's EnumDevices, then sort the results in
	// an order where gamepads come first, then give them to Rayman 2.
	HRESULT hr;

	typedef std::list<DIDEVICEINSTANCEA> DeviceInstanceList;
	struct DeviceEnumerator
	{
		DeviceInstanceList devices;

		static BOOL FAR PASCAL Callback(LPCDIDEVICEINSTANCEA lpddi, LPVOID pvRef)
		{
			DeviceEnumerator* self = (DeviceEnumerator*)pvRef;
			self->devices.push_back(*lpddi);
			return DIENUM_CONTINUE;
		}

		bool Contains(const GUID& guidInstance)
		{
			for (DeviceInstanceList::const_iterator it = devices.begin();
				it != devices.end(); ++it)
			{
				if (it->guidInstance == guidInstance) {
					return true;
				}
			}
			return false;
		}
	};

	DeviceEnumerator joystickDevices;
	hr = ProxyInterface->EnumDevicesA(DIDEVTYPE_JOYSTICK, DeviceEnumerator::Callback, &joystickDevices, dwFlags);
	if (FAILED(hr)) {
		return hr;
	}

	DeviceEnumerator allDevices;
	hr = ProxyInterface->EnumDevicesA(0, DeviceEnumerator::Callback, &allDevices, dwFlags);
	if (FAILED(hr)) {
		return hr;
	}

	DeviceInstanceList sortedDevices;

	if (dwDevType == 0 || dwDevType == DIDEVTYPE_JOYSTICK)
	{
		// Add all devices in gameDevices
		for (DeviceInstanceList::const_iterator it = joystickDevices.devices.begin();
			it != joystickDevices.devices.end(); ++it)
		{
			sortedDevices.push_back(*it);
		}
	}

	if (dwDevType == 0)
	{
		// Then, add all devices in allDevices that aren't in gameDevices
		for (DeviceInstanceList::const_iterator it = allDevices.devices.begin();
			it != allDevices.devices.end(); ++it)
		{
			if (!joystickDevices.Contains(it->guidInstance)) {
				sortedDevices.push_back(*it);
			}
		}
	}

	for (DeviceInstanceList::const_iterator it = sortedDevices.begin();
		it != sortedDevices.end(); ++it)
	{
		OutputDebugStringA("Enumerating Product: ");
		OutputDebugStringA(it->tszProductName);
		OutputDebugStringA(" Instance: ");
		OutputDebugStringA(it->tszInstanceName);
		OutputDebugStringA("\n");
		if (lpCallback(&*it, pvRef) == DIENUM_STOP) {
			break;
		}
	}

	return DI_OK;
}

HRESULT m_IDirectInputA::GetDeviceStatus(REFGUID rguidInstance)
{
	return ProxyInterface->GetDeviceStatus(rguidInstance);
}

HRESULT m_IDirectInputA::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputA::Initialize(HINSTANCE hinst, DWORD dwVersion)
{
	return ProxyInterface->Initialize(hinst, dwVersion);
}
