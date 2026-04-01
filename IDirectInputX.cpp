/**
* Copyright (C) 2025 Elisha Riedlinger
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

HRESULT m_IDirectInputX::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (ppvObj == nullptr)
	{
		return E_POINTER;
	}

	if (riid == IID_IUnknown ||
		riid == IID_IDirectInputA || riid == IID_IDirectInput2A || riid == IID_IDirectInput7A)
	{
		*ppvObj = static_cast<IDirectInput7A*>(this);
	}
	else if (riid == IID_IDirectInputW || riid == IID_IDirectInput2W || riid == IID_IDirectInput7W)
	{
		*ppvObj = static_cast<IDirectInput7W*>(this);
	}
	else
	{
		return ProxyInterface->QueryInterface(riid, ppvObj);
	}

	AddRef();
	return S_OK;
}

ULONG m_IDirectInputX::AddRef()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	const ULONG ref = ProxyInterface->AddRef();
	// This is technically not necessary, but it makes managing the two-references-to-one-object easier
	ProxyInterfaceA->AddRef();
	return ref;
}

ULONG m_IDirectInputX::Release()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	ProxyInterfaceA->Release();
	const ULONG ref = ProxyInterface->Release();
	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

template HRESULT m_IDirectInputX::EnumDevicesX<false, LPDIENUMDEVICESCALLBACKA, DIDEVICEINSTANCEA, DIDEVICEINSTANCE_DX3A>(DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
template HRESULT m_IDirectInputX::EnumDevicesX<true, LPDIENUMDEVICESCALLBACKW, DIDEVICEINSTANCEW, DIDEVICEINSTANCE_DX3W>(DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
template <bool bUnicode, class V, class D, class D_Old>
HRESULT m_IDirectInputX::EnumDevicesX(DWORD dwDevType, V lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!lpCallback)
	{
		return DIERR_INVALIDPARAM;
	}

	// Callback structure
	struct DeviceEnumerator
	{
		V lpCallback = nullptr;
		LPVOID pvRef = nullptr;
		DWORD diVersion = 0;

		static BOOL CALLBACK EnumDeviceCallback(const D *lpddi, LPVOID pvRef)
		{
			const DeviceEnumerator* self = static_cast<DeviceEnumerator*>(pvRef);
			DWORD devType = GET_DIDEVICE_TYPE(lpddi->dwDevType);
			DWORD devSubType = GET_DIDEVICE_SUBTYPE(lpddi->dwDevType);
			DWORD hidDevice = lpddi->dwDevType & DIDEVTYPE_HID;
			BOOL IsGamepad = FALSE;
			DWORD devType7 = ConvertDevTypeTo7(devType, lpddi->wUsagePage, lpddi->wUsage, hidDevice, IsGamepad);
			DWORD devSubType7 = ConvertDevSubTypeTo7(devType, devType7, devSubType, IsGamepad);

			// DirectInput 0x300 and earlier do not enumerate any game controllers
			if (devType7 == DIDEVTYPE_JOYSTICK && self->diVersion <= 0x300)
			{
				return DIENUM_CONTINUE;
			}

			D DI = {};
			CopyMemory(&DI, lpddi, lpddi->dwSize);

			// Prevent DInput3 games from encountering a structure bigger than they might expect.
			DI.dwSize = self->diVersion >= 0x0500 ? sizeof(D) : sizeof(D_Old);

			DI.dwDevType = devType7 | (devSubType7 << 8) | hidDevice;

			// Filter unknown device types
			if (DI.dwDevType == 0x10001)
			{
				return DIENUM_CONTINUE;
			}

			return self->lpCallback(&DI, self->pvRef);
		}
	} CallbackContext;
	CallbackContext.pvRef = pvRef;
	CallbackContext.lpCallback = lpCallback;
	CallbackContext.diVersion = diVersion;

	// Reorder to send game devices first
	if (dwDevType == DI8DEVCLASS_ALL)
	{
		// Instead of directly calling the ProxyInterface here, we are adding some extra code to sort the devices. This is an exclusive fix
		// for a known bug in Rayman 2 that would prevent any gamepad from working. A similar fix can be seen in this old VS 2010 project:
		// https://code.google.com/archive/p/noser-sandbox/source/default/source
		// There is only one download link with a zip file containing other projects too. You will find the extracted code in the
		// Rayman2InputFix_DirectInputA.cpp file from the Rayman2InputFix project.
		// Thanks Nolan Check for an example of how to fix this!

		// The bug: Rayman 2 expects EnumDevices to give results in a certain order, where gamepads come before the keyboard. DirectInput makes
		// no guarantee about the order.
		// The fix: Call DirectInput's EnumDevices, then sort the results in an order where gamepads come first, then give them to Rayman 2.

		// Get devices and sort them
		using DeviceInstanceList = std::list<D>;

		DeviceInstanceList gameDevices;
		DeviceInstanceList allDevices;

		std::list<std::reference_wrapper<const D>> sortedDevices;
		{
			auto StoreCallback = [](const D *lpddi, LPVOID pvRef) -> BOOL
				{
					DeviceInstanceList* self = static_cast<DeviceInstanceList*>(pvRef);
					self->emplace_back(*lpddi);
					return DIENUM_CONTINUE;
				};

			// DirectInput 0x300 and earlier do not enumerate any game controllers
			if (diVersion > 0x300)
			{
				HRESULT hr;
				if constexpr (bUnicode)
				{
					hr = ProxyInterface->EnumDevices(DI8DEVCLASS_GAMECTRL, StoreCallback, &gameDevices, dwFlags);
				}
				else
				{
					hr = ProxyInterfaceA->EnumDevices(DI8DEVCLASS_GAMECTRL, StoreCallback, &gameDevices, dwFlags);
				}
				if (FAILED(hr))
				{
					return hr;
				}
			}

			HRESULT hr;
			if constexpr (bUnicode)
			{
				hr = ProxyInterface->EnumDevices(DI8DEVCLASS_ALL, StoreCallback, &allDevices, dwFlags);
			}
			else
			{
				hr = ProxyInterfaceA->EnumDevices(DI8DEVCLASS_ALL, StoreCallback, &allDevices, dwFlags);
			}
			if (FAILED(hr))
			{
				return hr;
			}

			// Add all devices in gameDevices
			for (const D& device : gameDevices)
			{
				sortedDevices.emplace_back(device);
			}

			// Then, add all devices in allDevices that aren't in gameDevices
			for (const D& device : allDevices)
			{
				if (std::find_if(gameDevices.begin(), gameDevices.end(), [&guidInstance = device.guidInstance](const D& e)
					{
						return e.guidInstance == guidInstance;
					}) == gameDevices.end())
				{
					sortedDevices.emplace_back(device);
				}
			}
		}

		// Execute Callback
		for (const D& device : sortedDevices)
		{
			Logging::LogDebug() << __FUNCTION__ << " Enumerating Product: " << device.tszProductName << " Instance: " << device.tszInstanceName;

			if (DeviceEnumerator::EnumDeviceCallback(&device, &CallbackContext) == DIENUM_STOP)
			{
				break;
			}
		}

		return DI_OK;
	}

	if constexpr (bUnicode)
	{
		return ProxyInterface->EnumDevices(dwDevType, DeviceEnumerator::EnumDeviceCallback, &CallbackContext, dwFlags);
	}
	else
	{
		return ProxyInterfaceA->EnumDevices(dwDevType, DeviceEnumerator::EnumDeviceCallback, &CallbackContext, dwFlags);
	}
}

HRESULT m_IDirectInputX::GetDeviceStatus(REFGUID rguidInstance)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetDeviceStatus(rguidInstance);
}

HRESULT m_IDirectInputX::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputX::Initialize(HINSTANCE hinst, DWORD dwVersion)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	HRESULT hr = hresValidInstanceAndVersion(hinst, dwVersion);
	if (SUCCEEDED(hr))
	{
		hr = ProxyInterface->Initialize(hinst, 0x0800);

		if (SUCCEEDED(hr))
		{
			diVersion = dwVersion;
		}
	}

	return hr;
}

template HRESULT m_IDirectInputX::FindDeviceX<false, LPCSTR>(REFGUID, LPCSTR, LPGUID);
template HRESULT m_IDirectInputX::FindDeviceX<true, LPCWSTR>(REFGUID, LPCWSTR, LPGUID);
template <bool bUnicode, class V>
HRESULT m_IDirectInputX::FindDeviceX(REFGUID rguidClass, V ptszName, LPGUID pguidInstance)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if constexpr (bUnicode)
	{
		return ProxyInterface->FindDevice(rguidClass, ptszName, pguidInstance);
	}
	else
	{
		return ProxyInterfaceA->FindDevice(rguidClass, ptszName, pguidInstance);
	}
}

HRESULT m_IDirectInputX::CreateDeviceEx(REFGUID rguid, REFIID riid, LPVOID *ppvObj, LPUNKNOWN pUnkOuter)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ") " << rguid;

	if (pUnkOuter)
	{
		LOG_LIMIT(3, __FUNCTION__ << " Warning: 'pUnkOuter' is not null: " << pUnkOuter);
	}

	if (ppvObj == nullptr)
	{
		return E_POINTER;
	}

	IDirectInputDevice8W* ProxyDevice;
	HRESULT hr = ProxyInterface->CreateDevice(rguid, &ProxyDevice, nullptr);

	if (SUCCEEDED(hr))
	{
		m_IDirectInputDeviceX *DIDevice = new m_IDirectInputDeviceX(ProxyDevice);
		DIDevice->SetVersion(diVersion);

		if (IsEqualIID(GUID_SysMouse, rguid) || IsEqualIID(GUID_SysMouseEm, rguid) || IsEqualIID(GUID_SysMouseEm2, rguid))
		{
			DIDevice->SetAsMouse();
		}

		hr = DIDevice->QueryInterface(riid, ppvObj);
		DIDevice->Release();
	}

	return hr;
}
