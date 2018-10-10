/**
* Copyright (C) 2018 Elisha Riedlinger
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

#include "dinput.h"

HRESULT m_IDirectInputX::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	LogDebug() << __FUNCTION__;

	return ProxyQueryInterface(ProxyInterface, riid, ppvObj, WrapperID, WrapperInterface);
}

ULONG m_IDirectInputX::AddRef()
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputX::Release()
{
	LogDebug() << __FUNCTION__;

	ULONG x = ProxyInterface->Release();

	if (x == 0)
	{
		WrapperInterface->DeleteMe();
	}

	return x;
}

template HRESULT m_IDirectInputX::CreateDevice<LPDIRECTINPUTDEVICEA>(REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
template HRESULT m_IDirectInputX::CreateDevice<LPDIRECTINPUTDEVICEW>(REFGUID rguid, LPDIRECTINPUTDEVICEW *lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
template <typename T>
HRESULT m_IDirectInputX::CreateDevice(REFGUID rguid, T *lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	LogDebug() << __FUNCTION__;

	HRESULT hr = ProxyInterface->CreateDevice(rguid, (LPDIRECTINPUTDEVICE8W *)lplpDirectInputDevice, pUnkOuter);

	if (SUCCEEDED(hr) && lplpDirectInputDevice)
	{
		*lplpDirectInputDevice = (T)ProxyAddressLookupTable.FindAddress<m_IDirectInputDeviceW>(*lplpDirectInputDevice, DirectXVersion, StringType);
	}

	return hr;
}

template HRESULT m_IDirectInputX::EnumDevices<LPDIENUMDEVICESCALLBACKA>(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
template HRESULT m_IDirectInputX::EnumDevices<LPDIENUMDEVICESCALLBACKW>(DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags);
template <typename T>
HRESULT m_IDirectInputX::EnumDevices(DWORD dwDevType, T lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->EnumDevices(dwDevType, (LPDIENUMDEVICESCALLBACKW)lpCallback, pvRef, dwFlags);
}

HRESULT m_IDirectInputX::GetDeviceStatus(REFGUID rguidInstance)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->GetDeviceStatus(rguidInstance);
}

HRESULT m_IDirectInputX::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputX::Initialize(HINSTANCE hinst, DWORD dwVersion)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->Initialize(hinst, dwVersion);
}

template HRESULT m_IDirectInputX::FindDevice<LPCSTR>(REFGUID rguidClass, LPCSTR ptszName, LPGUID pguidInstance);
template HRESULT m_IDirectInputX::FindDevice<LPCWSTR>(REFGUID rguidClass, LPCWSTR ptszName, LPGUID pguidInstance);
template <typename T>
HRESULT m_IDirectInputX::FindDevice(REFGUID rguidClass, T ptszName, LPGUID pguidInstance)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->FindDevice(rguidClass, (LPCWSTR)ptszName, pguidInstance);
}

HRESULT m_IDirectInputX::CreateDeviceEx(REFGUID rguid, REFIID riid, LPVOID * ppvObj, LPUNKNOWN pUnkOuter)
{
	LogDebug() << __FUNCTION__;

	HRESULT hr = ProxyInterface->CreateDevice(rguid, (LPDIRECTINPUTDEVICE8W *)ppvObj, pUnkOuter);

	if (SUCCEEDED(hr))
	{
		genericQueryInterface(riid, ppvObj);
	}

	return hr;
}
