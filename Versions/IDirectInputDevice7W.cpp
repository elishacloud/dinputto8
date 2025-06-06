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

HRESULT m_IDirectInputDevice7W::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
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

ULONG m_IDirectInputDevice7W::AddRef()
{
	if (!ProxyInterface)
	{
		return 0;
	}
	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputDevice7W::Release()
{
	if (!ProxyInterface)
	{
		return 0;
	}
	return ProxyInterface->Release();
}

HRESULT m_IDirectInputDevice7W::GetCapabilities(LPDIDEVCAPS lpDIDevCaps)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetCapabilities<LPDIDEVCAPS, DIDEVICEINSTANCEW>(lpDIDevCaps);
}

HRESULT m_IDirectInputDevice7W::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->EnumObjects(lpCallback, pvRef, dwFlags);
}

HRESULT m_IDirectInputDevice7W::GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDevice7W::SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->SetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDevice7W::Acquire()
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->Acquire();
}

HRESULT m_IDirectInputDevice7W::Unacquire()
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->Unacquire();
}

HRESULT m_IDirectInputDevice7W::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetDeviceState(cbData, lpvData);
}

HRESULT m_IDirectInputDevice7W::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
}

HRESULT m_IDirectInputDevice7W::SetDataFormat(LPCDIDATAFORMAT lpdf)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->SetDataFormat(lpdf);
}

HRESULT m_IDirectInputDevice7W::SetEventNotification(HANDLE hEvent)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->SetEventNotification(hEvent);
}

HRESULT m_IDirectInputDevice7W::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->SetCooperativeLevel(hwnd, dwFlags);
}

HRESULT m_IDirectInputDevice7W::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEW pdidoi, DWORD dwObj, DWORD dwHow)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetObjectInfo(pdidoi, dwObj, dwHow);
}

HRESULT m_IDirectInputDevice7W::GetDeviceInfo(LPDIDEVICEINSTANCEW pdidi)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetDeviceInfo(pdidi);
}

HRESULT m_IDirectInputDevice7W::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputDevice7W::Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->Initialize(hinst, dwVersion, rguid);
}

HRESULT m_IDirectInputDevice7W::CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT * ppdeff, LPUNKNOWN punkOuter)
{
	if (!ProxyInterface)
	{
		if (ppdeff)
		{
			*ppdeff = nullptr;
		}
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->CreateEffect(rguid, lpeff, ppdeff, punkOuter);
}

HRESULT m_IDirectInputDevice7W::EnumEffects(LPDIENUMEFFECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwEffType)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->EnumEffects(lpCallback, pvRef, dwEffType);
}

HRESULT m_IDirectInputDevice7W::GetEffectInfo(LPDIEFFECTINFOW pdei, REFGUID rguid)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetEffectInfo(pdei, rguid);
}

HRESULT m_IDirectInputDevice7W::GetForceFeedbackState(LPDWORD pdwOut)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->GetForceFeedbackState(pdwOut);
}

HRESULT m_IDirectInputDevice7W::SendForceFeedbackCommand(DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->SendForceFeedbackCommand(dwFlags);
}

HRESULT m_IDirectInputDevice7W::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->EnumCreatedEffectObjects(lpCallback, pvRef, fl);
}

HRESULT m_IDirectInputDevice7W::Escape(LPDIEFFESCAPE pesc)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->Escape(pesc);
}

HRESULT m_IDirectInputDevice7W::Poll()
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->Poll();
}

HRESULT m_IDirectInputDevice7W::SendDeviceData(DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->SendDeviceData(cbObjectData, rgdod, pdwInOut, fl);
}

HRESULT m_IDirectInputDevice7W::EnumEffectsInFile(LPCWSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->EnumEffectsInFile(lpszFileName, pec, pvRef, dwFlags);
}

HRESULT m_IDirectInputDevice7W::WriteEffectToFile(LPCWSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
{
	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}
	return ProxyInterface->WriteEffectToFile(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
}
