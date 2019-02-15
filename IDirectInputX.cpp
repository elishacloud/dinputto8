/**
* Copyright (C) 2019 Elisha Riedlinger
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

HRESULT m_IDirectInputX::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyQueryInterface(ProxyInterface, riid, ppvObj, WrapperID, WrapperInterface);
}

ULONG m_IDirectInputX::AddRef()
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	ProxyInterface->AddRef();

	return InterlockedIncrement(&RefCount);
}

ULONG m_IDirectInputX::Release()
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	ProxyInterface->Release();
	
	ULONG x = InterlockedDecrement(&RefCount);

	if (x == 0)
	{
		WrapperInterface->DeleteMe();
	}

	return x;
}

template HRESULT m_IDirectInputX::EnumDevices<LPDIENUMDEVICESCALLBACKA>(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
template HRESULT m_IDirectInputX::EnumDevices<LPDIENUMDEVICESCALLBACKW>(DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags);
template <typename T>
HRESULT m_IDirectInputX::EnumDevices(DWORD dwDevType, T lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	ENUMDEVICE CallbackContext;
	CallbackContext.pvRef = pvRef;
	CallbackContext.lpCallback = lpCallback;

	return GetProxyInterface(lpCallback)->EnumDevices(dwDevType, m_IDirectInputEnumDevice::EnumDeviceCallback, &CallbackContext, dwFlags);
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

	return ProxyInterface->Initialize(hinst, dwVersion);
}

template HRESULT m_IDirectInputX::FindDevice<LPCSTR>(REFGUID rguidClass, LPCSTR ptszName, LPGUID pguidInstance);
template HRESULT m_IDirectInputX::FindDevice<LPCWSTR>(REFGUID rguidClass, LPCWSTR ptszName, LPGUID pguidInstance);
template <typename T>
HRESULT m_IDirectInputX::FindDevice(REFGUID rguidClass, T ptszName, LPGUID pguidInstance)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterface(ptszName)->FindDevice(rguidClass, ptszName, pguidInstance);
}

template HRESULT m_IDirectInputX::CreateDeviceEx<LPDIRECTINPUTDEVICE8A>(REFGUID rguid, REFIID riid, LPDIRECTINPUTDEVICE8A *ppvObj, LPUNKNOWN pUnkOuter);
template HRESULT m_IDirectInputX::CreateDeviceEx<LPDIRECTINPUTDEVICE8W>(REFGUID rguid, REFIID riid, LPDIRECTINPUTDEVICE8W *ppvObj, LPUNKNOWN pUnkOuter);
template <typename T>
HRESULT m_IDirectInputX::CreateDeviceEx(REFGUID rguid, REFIID riid, T *ppvObj, LPUNKNOWN pUnkOuter)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	HRESULT hr = GetProxyInterface(*ppvObj)->CreateDevice(rguid, ppvObj, pUnkOuter);

	if (SUCCEEDED(hr))
	{
		AddRef();
		genericQueryInterface(riid, (LPVOID *)ppvObj);
	}

	return hr;
}
