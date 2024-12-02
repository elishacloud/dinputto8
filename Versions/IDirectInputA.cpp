/**
* Copyright (C) 2024 Elisha Riedlinger
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
	if (!ProxyInterface)
	{
		if (ppvObj)
		{
			*ppvObj = nullptr;
		}
		return E_NOINTERFACE;
	}
	return ProxyInterface->QueryInterface(ReplaceIIDUnknown(riid, WrapperID), ppvObj, DirectXVersion);
}

ULONG m_IDirectInputA::AddRef()
{
	if (!ProxyInterface)
	{
		return 0;
	}
	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputA::Release()
{
	if (!ProxyInterface)
	{
		return 0;
	}
	return ProxyInterface->Release();
}

HRESULT m_IDirectInputA::CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	if (!ProxyInterface)
	{
		if (lplpDirectInputDevice)
		{
			*lplpDirectInputDevice = nullptr;
		}
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->CreateDeviceEx(rguid, IID_IDirectInputDeviceA, (LPDIRECTINPUTDEVICE8A *)lplpDirectInputDevice, pUnkOuter);
}

HRESULT m_IDirectInputA::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT m_IDirectInputA::GetDeviceStatus(REFGUID rguidInstance)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetDeviceStatus(rguidInstance);
}

HRESULT m_IDirectInputA::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputA::Initialize(HINSTANCE hinst, DWORD dwVersion)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->Initialize(hinst, dwVersion);
}
