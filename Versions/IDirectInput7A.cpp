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

#include "..\dinputto8.h"

HRESULT m_IDirectInput7A::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
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

ULONG m_IDirectInput7A::AddRef()
{
	if (!ProxyInterface)
	{
		return 0;
	}
	return ProxyInterface->AddRef();
}

ULONG m_IDirectInput7A::Release()
{
	if (!ProxyInterface)
	{
		return 0;
	}
	return ProxyInterface->Release();
}

HRESULT m_IDirectInput7A::CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	if (!ProxyInterface)
	{
		if (lplpDirectInputDevice)
		{
			*lplpDirectInputDevice = nullptr;
		}
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->CreateDeviceEx(rguid, IID_IDirectInputDevice7A, (LPDIRECTINPUTDEVICE8A *)lplpDirectInputDevice, pUnkOuter);
}

HRESULT m_IDirectInput7A::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT m_IDirectInput7A::GetDeviceStatus(REFGUID rguidInstance)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetDeviceStatus(rguidInstance);
}

HRESULT m_IDirectInput7A::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInput7A::Initialize(HINSTANCE hinst, DWORD dwVersion)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->Initialize(hinst, dwVersion);
}

HRESULT m_IDirectInput7A::FindDevice(REFGUID rguidClass, LPCSTR ptszName, LPGUID pguidInstance)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->FindDevice(rguidClass, ptszName, pguidInstance);
}

HRESULT m_IDirectInput7A::CreateDeviceEx(REFGUID rguid, REFIID riid, LPVOID * ppvObj, LPUNKNOWN pUnkOuter)
{
	if (!ProxyInterface)
	{
		if (ppvObj)
		{
			*ppvObj = nullptr;
		}
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->CreateDeviceEx(rguid, riid, (LPDIRECTINPUTDEVICE8A *)ppvObj, pUnkOuter);
}
