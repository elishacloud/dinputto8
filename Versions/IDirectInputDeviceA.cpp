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

HRESULT m_IDirectInputDeviceA::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
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

ULONG m_IDirectInputDeviceA::AddRef()
{
	if (!ProxyInterface)
	{
		return 0;
	}
	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputDeviceA::Release()
{
	if (!ProxyInterface)
	{
		return 0;
	}
	return ProxyInterface->Release();
}

HRESULT m_IDirectInputDeviceA::GetCapabilities(LPDIDEVCAPS lpDIDevCaps)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetCapabilities<LPDIDEVCAPS, DIDEVICEINSTANCEA>(lpDIDevCaps);
}

HRESULT m_IDirectInputDeviceA::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->EnumObjects(lpCallback, pvRef, dwFlags);
}

HRESULT m_IDirectInputDeviceA::GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDeviceA::SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->SetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDeviceA::Acquire()
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->Acquire();
}

HRESULT m_IDirectInputDeviceA::Unacquire()
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->Unacquire();
}

HRESULT m_IDirectInputDeviceA::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetDeviceState(cbData, lpvData);
}

HRESULT m_IDirectInputDeviceA::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
}

HRESULT m_IDirectInputDeviceA::SetDataFormat(LPCDIDATAFORMAT lpdf)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->SetDataFormat(lpdf);
}

HRESULT m_IDirectInputDeviceA::SetEventNotification(HANDLE hEvent)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->SetEventNotification(hEvent);
}

HRESULT m_IDirectInputDeviceA::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->SetCooperativeLevel(hwnd, dwFlags);
}

HRESULT m_IDirectInputDeviceA::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetObjectInfo(pdidoi, dwObj, dwHow);
}

HRESULT m_IDirectInputDeviceA::GetDeviceInfo(LPDIDEVICEINSTANCEA pdidi)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetDeviceInfo(pdidi);
}

HRESULT m_IDirectInputDeviceA::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputDeviceA::Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->Initialize(hinst, dwVersion, rguid);
}
