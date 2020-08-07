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

	return ProxyInterface->Initialize(hinst, dwVersion);
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
