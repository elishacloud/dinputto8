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

template HRESULT m_IDirectInputX::EnumDevicesX<IDirectInput8A, LPDIENUMDEVICESCALLBACKA, DIDEVICEINSTANCEA>(DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
template HRESULT m_IDirectInputX::EnumDevicesX<IDirectInput8W, LPDIENUMDEVICESCALLBACKW, DIDEVICEINSTANCEW>(DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
template <class T, class V, class D>
HRESULT m_IDirectInputX::EnumDevicesX(DWORD dwDevType, V lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	if (!lpCallback)
	{
		return DIERR_INVALIDPARAM;
	}

	// Callback structure
	typedef std::list<D> DeviceInstanceList;
	struct DeviceEnumerator
	{
		DeviceInstanceList devices;
		V lpCallback;
		LPVOID pvRef;

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

		static BOOL CALLBACK StoreCallback(const D *lpddi, LPVOID pvRef)
		{
			DeviceEnumerator* self = (DeviceEnumerator*)pvRef;
			self->devices.push_back(*lpddi);
			return DIENUM_CONTINUE;
		}

		static BOOL CALLBACK EnumDeviceCallback(const D *lpddi, LPVOID pvRef)
		{
			DeviceEnumerator* self = (DeviceEnumerator*)pvRef;

			D DI;
			CopyMemory(&DI, lpddi, lpddi->dwSize);

			DI.dwDevType = (lpddi->dwDevType & ~0xFFFF) |													// Remove device type and sub type
				ConvertDevSubTypeTo7(lpddi->dwDevType & 0xFF, (lpddi->dwDevType & 0xFF00) >> 8) << 8 |		// Add converted sub type
				ConvertDevTypeTo7(lpddi->dwDevType & 0xFF);													// Add converted device type

			return self->lpCallback(&DI, self->pvRef);
		}
	} CallbackContext;
	CallbackContext.pvRef = pvRef;
	CallbackContext.lpCallback = lpCallback;

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

		// Get devices and sort them
		DeviceInstanceList sortedDevices;
		{
			DeviceEnumerator gameDevices;
			HRESULT hr = GetProxyInterface<T>()->EnumDevices(DI8DEVCLASS_GAMECTRL, DeviceEnumerator::StoreCallback, &gameDevices, dwFlags);
			if (FAILED(hr))
			{
				return hr;
			}

			DeviceEnumerator allDevices;
			hr = GetProxyInterface<T>()->EnumDevices(DI8DEVCLASS_ALL, DeviceEnumerator::StoreCallback, &allDevices, dwFlags);
			if (FAILED(hr))
			{
				return hr;
			}

			// Add all devices in gameDevices
			for (DeviceInstanceList::const_iterator it = gameDevices.devices.begin(); it != gameDevices.devices.end(); ++it)
			{
				sortedDevices.push_back(*it);
			}

			// Then, add all devices in allDevices that aren't in gameDevices
			for (DeviceInstanceList::const_iterator it = allDevices.devices.begin(); it != allDevices.devices.end(); ++it)
			{
				if (!gameDevices.Contains(it->guidInstance))
				{
					sortedDevices.push_back(*it);
				}
			}
		}

		// Execute Callback
		for (DeviceInstanceList::const_iterator it = sortedDevices.begin(); it != sortedDevices.end(); ++it)
		{
			Logging::Log() << __FUNCTION__ << " Enumerating Product: " << it->tszProductName << " Instance: " << it->tszInstanceName;

			if (DeviceEnumerator::EnumDeviceCallback(&*it, &CallbackContext) == DIENUM_STOP)
			{
				break;
			}
		}

		return DI_OK;
	}

	return GetProxyInterface<T>()->EnumDevices(dwDevType, DeviceEnumerator::EnumDeviceCallback, &CallbackContext, dwFlags);
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

template HRESULT m_IDirectInputX::FindDeviceX<IDirectInput8A, LPCSTR>(REFGUID, LPCSTR, LPGUID);
template HRESULT m_IDirectInputX::FindDeviceX<IDirectInput8W, LPCWSTR>(REFGUID, LPCWSTR, LPGUID);
template <class T, class V>
HRESULT m_IDirectInputX::FindDeviceX(REFGUID rguidClass, V ptszName, LPGUID pguidInstance)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterface<T>()->FindDevice(rguidClass, ptszName, pguidInstance);
}

template HRESULT m_IDirectInputX::CreateDeviceExX<IDirectInput8A, LPDIRECTINPUTDEVICE8A>(REFGUID, REFIID, LPDIRECTINPUTDEVICE8A*, LPUNKNOWN);
template HRESULT m_IDirectInputX::CreateDeviceExX<IDirectInput8W, LPDIRECTINPUTDEVICE8W>(REFGUID, REFIID, LPDIRECTINPUTDEVICE8W*, LPUNKNOWN);
template <class T, class V>
HRESULT m_IDirectInputX::CreateDeviceExX(REFGUID rguid, REFIID riid, V *ppvObj, LPUNKNOWN pUnkOuter)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	HRESULT hr = GetProxyInterface<T>()->CreateDevice(rguid, ppvObj, pUnkOuter);

	if (SUCCEEDED(hr) && ppvObj)
	{
		m_IDirectInputDeviceX *DIDevice = new m_IDirectInputDeviceX((IDirectInputDevice8W*)*ppvObj, riid);

		*ppvObj = (V)DIDevice->GetWrapperInterfaceX(GetGUIDVersion(riid));
	}

	return hr;
}
