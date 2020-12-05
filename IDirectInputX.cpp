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

#include "dinputto8.h"

HRESULT m_IDirectInputX::QueryInterface(REFIID riid, LPVOID FAR * ppvObj, DWORD DirectXVersion)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	DWORD DxVersion = (CheckWrapperType(riid)) ? GetGUIDVersion(riid) : DirectXVersion;

	return ProxyQueryInterface(ProxyInterface, riid, ppvObj, GetWrapperType(DxVersion), GetWrapperInterfaceX(DxVersion));
}

LPVOID m_IDirectInputX::GetWrapperInterfaceX(DWORD DirectXVersion)
{
	switch (DirectXVersion)
	{
	case 1:
		return WrapperInterface;
	case 2:
		return WrapperInterface2;
	case 7:
		return WrapperInterface7;
	default:
		return nullptr;
	}
}

ULONG m_IDirectInputX::AddRef()
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputX::Release()
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	ULONG ref = ProxyInterface->Release();

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT m_IDirectInputX::EnumDevicesA(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	if (!lpCallback)
	{
		return DIERR_INVALIDPARAM;
	}

	// Reorder to send game devices first
	if (dwDevType == DI8DEVCLASS_ALL)
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

		typedef std::list<DIDEVICEINSTANCEA> DeviceInstanceList;
		struct DeviceEnumerator
		{
			DeviceInstanceList devices;

			static BOOL CALLBACK Callback(LPCDIDEVICEINSTANCEA lpddi, LPVOID pvRef)
			{
				DeviceEnumerator* self = (DeviceEnumerator*)pvRef;
				self->devices.push_back(*lpddi);
				return DIENUM_CONTINUE;
			}

			bool Contains(const GUID& guidInstance)
			{
				for (DeviceInstanceList::const_iterator it = devices.begin(); it != devices.end(); ++it)
				{
					if (it->guidInstance == guidInstance)
					{
						return true;
					}
				}
				return false;
			}
		};

		DeviceEnumerator joystickDevices;
		HRESULT hr = GetProxyInterfaceA()->EnumDevices(DI8DEVCLASS_GAMECTRL, DeviceEnumerator::Callback, &joystickDevices, dwFlags);
		if (FAILED(hr))
		{
			return hr;
		}

		DeviceEnumerator allDevices;
		hr = GetProxyInterfaceA()->EnumDevices(DI8DEVCLASS_ALL, DeviceEnumerator::Callback, &allDevices, dwFlags);
		if (FAILED(hr))
		{
			return hr;
		}

		DeviceInstanceList sortedDevices;

		// Add all devices in gameDevices
		for (DeviceInstanceList::const_iterator it = joystickDevices.devices.begin(); it != joystickDevices.devices.end(); ++it)
		{
			sortedDevices.push_back(*it);
		}

		// Then, add all devices in allDevices that aren't in gameDevices
		for (DeviceInstanceList::const_iterator it = allDevices.devices.begin(); it != allDevices.devices.end(); ++it)
		{
			if (!joystickDevices.Contains(it->guidInstance))
			{
				sortedDevices.push_back(*it);
			}
		}

		ENUMDEVICE CallbackContext;
		CallbackContext.pvRef = pvRef;
		CallbackContext.lpCallback = lpCallback;

		for (DeviceInstanceList::const_iterator it = sortedDevices.begin(); it != sortedDevices.end(); ++it)
		{
			Logging::Log() << __FUNCTION__ << " Enumerating Product: " << it->tszProductName << " Instance: " << it->tszInstanceName;
			if (m_IDirectInputEnumDevice::EnumDeviceCallbackA(&*it, &CallbackContext) == DIENUM_STOP)
			{
				break;
			}
		}

		return DI_OK;
	}

	ENUMDEVICE CallbackContext;
	CallbackContext.pvRef = pvRef;
	CallbackContext.lpCallback = lpCallback;

	return GetProxyInterfaceA()->EnumDevices(dwDevType, m_IDirectInputEnumDevice::EnumDeviceCallbackA, &CallbackContext, dwFlags);
}

HRESULT m_IDirectInputX::EnumDevicesW(DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	if (!lpCallback)
	{
		return DIERR_INVALIDPARAM;
	}

	// Reorder to send game devices first
	if (dwDevType == DI8DEVCLASS_ALL)
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

		typedef std::list<DIDEVICEINSTANCEW> DeviceInstanceList;
		struct DeviceEnumerator
		{
			DeviceInstanceList devices;

			static BOOL CALLBACK Callback(LPCDIDEVICEINSTANCEW lpddi, LPVOID pvRef)
			{
				DeviceEnumerator* self = (DeviceEnumerator*)pvRef;
				self->devices.push_back(*lpddi);
				return DIENUM_CONTINUE;
			}

			bool Contains(const GUID& guidInstance)
			{
				for (DeviceInstanceList::const_iterator it = devices.begin(); it != devices.end(); ++it)
				{
					if (it->guidInstance == guidInstance)
					{
						return true;
					}
				}
				return false;
			}
		};

		DeviceEnumerator joystickDevices;
		HRESULT hr = GetProxyInterfaceW()->EnumDevices(DI8DEVCLASS_GAMECTRL, DeviceEnumerator::Callback, &joystickDevices, dwFlags);
		if (FAILED(hr))
		{
			return hr;
		}

		DeviceEnumerator allDevices;
		hr = GetProxyInterfaceW()->EnumDevices(DI8DEVCLASS_ALL, DeviceEnumerator::Callback, &allDevices, dwFlags);
		if (FAILED(hr))
		{
			return hr;
		}

		DeviceInstanceList sortedDevices;

		// Add all devices in gameDevices
		for (DeviceInstanceList::const_iterator it = joystickDevices.devices.begin(); it != joystickDevices.devices.end(); ++it)
		{
			sortedDevices.push_back(*it);
		}

		// Then, add all devices in allDevices that aren't in gameDevices
		for (DeviceInstanceList::const_iterator it = allDevices.devices.begin(); it != allDevices.devices.end(); ++it)
		{
			if (!joystickDevices.Contains(it->guidInstance))
			{
				sortedDevices.push_back(*it);
			}
		}

		ENUMDEVICE CallbackContext;
		CallbackContext.pvRef = pvRef;
		CallbackContext.lpCallback = lpCallback;

		for (DeviceInstanceList::const_iterator it = sortedDevices.begin(); it != sortedDevices.end(); ++it)
		{
			Logging::Log() << __FUNCTION__ << " Enumerating Product: " << it->tszProductName << " Instance: " << it->tszInstanceName;
			if (m_IDirectInputEnumDevice::EnumDeviceCallbackW(&*it, &CallbackContext) == DIENUM_STOP)
			{
				break;
			}
		}

		return DI_OK;
	}

	ENUMDEVICE CallbackContext;
	CallbackContext.pvRef = pvRef;
	CallbackContext.lpCallback = lpCallback;

	return GetProxyInterfaceW()->EnumDevices(dwDevType, m_IDirectInputEnumDevice::EnumDeviceCallbackW, &CallbackContext, dwFlags);
}

HRESULT m_IDirectInputX::GetDeviceStatus(REFGUID rguidInstance)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->GetDeviceStatus(rguidInstance);
}

HRESULT m_IDirectInputX::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputX::Initialize(HINSTANCE hinst, DWORD dwVersion)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	HRESULT hr = ProxyInterface->Initialize(hinst, 0x0800);

	if (SUCCEEDED(hr))
	{
		diVersion = dwVersion;
	}

	return hr;
}

HRESULT m_IDirectInputX::FindDeviceA(REFGUID rguidClass, LPCSTR ptszName, LPGUID pguidInstance)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceA()->FindDevice(rguidClass, ptszName, pguidInstance);
}

HRESULT m_IDirectInputX::FindDeviceW(REFGUID rguidClass, LPCWSTR ptszName, LPGUID pguidInstance)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceW()->FindDevice(rguidClass, ptszName, pguidInstance);
}

HRESULT m_IDirectInputX::CreateDeviceExA(REFGUID rguid, REFIID riid, LPDIRECTINPUTDEVICE8A *ppvObj, LPUNKNOWN pUnkOuter)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	HRESULT hr = GetProxyInterfaceA()->CreateDevice(rguid, ppvObj, pUnkOuter);

	if (SUCCEEDED(hr) && ppvObj)
	{
		m_IDirectInputDeviceX *DIDevice = new m_IDirectInputDeviceX((IDirectInputDevice8W*)*ppvObj, riid);

		*ppvObj = (LPDIRECTINPUTDEVICE8A)DIDevice->GetWrapperInterfaceX(GetGUIDVersion(riid));
	}

	return hr;
}

HRESULT m_IDirectInputX::CreateDeviceExW(REFGUID rguid, REFIID riid, LPDIRECTINPUTDEVICE8W *ppvObj, LPUNKNOWN pUnkOuter)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	HRESULT hr = GetProxyInterfaceW()->CreateDevice(rguid, ppvObj, pUnkOuter);

	if (SUCCEEDED(hr) && ppvObj)
	{
		m_IDirectInputDeviceX *DIDevice = new m_IDirectInputDeviceX((IDirectInputDevice8W*)*ppvObj, riid);

		*ppvObj = (LPDIRECTINPUTDEVICE8W)DIDevice->GetWrapperInterfaceX(GetGUIDVersion(riid));
	}

	return hr;
}
