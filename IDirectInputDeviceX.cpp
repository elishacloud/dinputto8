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

HRESULT m_IDirectInputDeviceX::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyQueryInterface(ProxyInterface, riid, ppvObj, WrapperID, WrapperInterface);
}

ULONG m_IDirectInputDeviceX::AddRef()
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	ProxyInterface->AddRef();

	return InterlockedIncrement(&RefCount);
}

ULONG m_IDirectInputDeviceX::Release()
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	ProxyInterface->Release();

	ULONG ref = InterlockedDecrement(&RefCount);

	if (ref == 0)
	{
		WrapperInterface->DeleteMe();
	}

	return ref;
}

HRESULT m_IDirectInputDeviceX::GetCapabilities(LPDIDEVCAPS lpDIDevCaps)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->GetCapabilities(lpDIDevCaps);
}

HRESULT m_IDirectInputDeviceX::EnumObjectsA(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceA()->EnumObjects(lpCallback, pvRef, dwFlags);
}

HRESULT m_IDirectInputDeviceX::EnumObjectsW(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceW()->EnumObjects(lpCallback, pvRef, dwFlags);
}

HRESULT m_IDirectInputDeviceX::GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->GetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDeviceX::SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->SetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDeviceX::Acquire()
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	if (!CanAquireDevice)
	{
		return DIERR_NOTINITIALIZED;
	}

	return ProxyInterface->Acquire();
}

HRESULT m_IDirectInputDeviceX::Unacquire()
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->Unacquire();
}

HRESULT m_IDirectInputDeviceX::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->GetDeviceState(cbData, lpvData);
}

HRESULT m_IDirectInputDeviceX::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	if (pdwInOut && *pdwInOut == (DWORD)-1)
	{
		return DIERR_INVALIDPARAM;
	}

	EnterCriticalSection(&dics);

	// Check the size of the array
	if (rgdod && pdwInOut && *pdwInOut > pdod.size())
	{
		pdod.resize(*pdwInOut);

		Logging::LogDebug() << __FUNCTION__ << " Update dod memory! " << *pdwInOut;
	}

	HRESULT hr = ProxyInterface->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), (rgdod) ? &pdod[0] : nullptr, pdwInOut, dwFlags);

	// Copy array
	if (SUCCEEDED(hr) && rgdod && pdwInOut && cbObjectData)
	{
		for (UINT x = 0; x < *pdwInOut; x++)
		{
			CopyMemory((void*)((DWORD)rgdod + (cbObjectData * x)), &pdod[x], cbObjectData);
		}
	}

	LeaveCriticalSection(&dics);

	return hr;
}

HRESULT m_IDirectInputDeviceX::SetDataFormat(LPCDIDATAFORMAT lpdf)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	// Fix unsupported flags
	if (lpdf && lpdf->dwNumObjs && lpdf->dwObjSize == 16)
	{
		EnterCriticalSection(&dics);

		if (rgodf.size() < lpdf->dwNumObjs)
		{
			rgodf.resize(lpdf->dwNumObjs);
		}

		df = {
			sizeof(DIDATAFORMAT),
			lpdf->dwObjSize,
			lpdf->dwFlags,
			lpdf->dwDataSize,
			lpdf->dwNumObjs,
			&rgodf[0] };

		for (DWORD x = 0; x < df.dwNumObjs; x++)
		{
			rgodf[x].pguid = lpdf->rgodf[x].pguid;
			rgodf[x].dwOfs = lpdf->rgodf[x].dwOfs;
			rgodf[x].dwType = ((lpdf->rgodf[x].dwType & DIDFT_ANYINSTANCE) == 0xFF00) ? lpdf->rgodf[x].dwType | DIDFT_ANYINSTANCE : lpdf->rgodf[x].dwType;
			rgodf[x].dwFlags = lpdf->rgodf[x].dwFlags;
		}

		HRESULT hr = ProxyInterface->SetDataFormat(&df);

		LeaveCriticalSection(&dics);

		return hr;
	}

	return ProxyInterface->SetDataFormat(lpdf);
}

HRESULT m_IDirectInputDeviceX::SetEventNotification(HANDLE hEvent)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->SetEventNotification(hEvent);
}

HRESULT m_IDirectInputDeviceX::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	HRESULT hr = ProxyInterface->SetCooperativeLevel(hwnd, dwFlags);

	if (SUCCEEDED(hr))
	{
		CanAquireDevice = true;
	}

	return hr;
}

HRESULT m_IDirectInputDeviceX::GetObjectInfoA(LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceA()->GetObjectInfo(pdidoi, dwObj, dwHow);
}

HRESULT m_IDirectInputDeviceX::GetObjectInfoW(LPDIDEVICEOBJECTINSTANCEW pdidoi, DWORD dwObj, DWORD dwHow)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceW()->GetObjectInfo(pdidoi, dwObj, dwHow);
}

HRESULT m_IDirectInputDeviceX::GetDeviceInfoA(LPDIDEVICEINSTANCEA pdidi)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	HRESULT hr = GetProxyInterfaceA()->GetDeviceInfo(pdidi);

	if (SUCCEEDED(hr) && pdidi && pdidi->dwSize)
	{
		pdidi->dwDevType = ConvertDevTypeTo7(GET_DIDEVICE_TYPE(pdidi->dwDevType));
	}

	return hr;
}

HRESULT m_IDirectInputDeviceX::GetDeviceInfoW(LPDIDEVICEINSTANCEW pdidi)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	HRESULT hr = GetProxyInterfaceW()->GetDeviceInfo(pdidi);

	if (SUCCEEDED(hr) && pdidi && pdidi->dwSize)
	{
		pdidi->dwDevType = ConvertDevTypeTo7(GET_DIDEVICE_TYPE(pdidi->dwDevType));
	}

	return hr;
}

HRESULT m_IDirectInputDeviceX::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputDeviceX::Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->Initialize(hinst, dwVersion, rguid);
}

HRESULT m_IDirectInputDeviceX::CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT * ppdeff, LPUNKNOWN punkOuter)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	HRESULT hr = ProxyInterface->CreateEffect(rguid, lpeff, ppdeff, punkOuter);

	if (SUCCEEDED(hr) && ppdeff)
	{
		*ppdeff = ProxyAddressLookupTable.FindAddress<m_IDirectInputEffect>(*ppdeff);
	}

	return hr;
}

HRESULT m_IDirectInputDeviceX::EnumEffectsA(LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceA()->EnumEffects(lpCallback, pvRef, dwEffType);
}

HRESULT m_IDirectInputDeviceX::EnumEffectsW(LPDIENUMEFFECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwEffType)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceW()->EnumEffects(lpCallback, pvRef, dwEffType);
}

HRESULT m_IDirectInputDeviceX::GetEffectInfoA(LPDIEFFECTINFOA pdei, REFGUID rguid)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceA()->GetEffectInfo(pdei, rguid);
}

HRESULT m_IDirectInputDeviceX::GetEffectInfoW(LPDIEFFECTINFOW pdei, REFGUID rguid)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceW()->GetEffectInfo(pdei, rguid);
}

HRESULT m_IDirectInputDeviceX::GetForceFeedbackState(LPDWORD pdwOut)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->GetForceFeedbackState(pdwOut);
}

HRESULT m_IDirectInputDeviceX::SendForceFeedbackCommand(DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->SendForceFeedbackCommand(dwFlags);
}

HRESULT m_IDirectInputDeviceX::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	if (!lpCallback)
	{
		return DIERR_INVALIDPARAM;
	}

	ENUMEFFECT CallbackContext;
	CallbackContext.pvRef = pvRef;
	CallbackContext.lpCallback = lpCallback;

	return ProxyInterface->EnumCreatedEffectObjects(m_IDirectInputEnumEffect::EnumEffectCallback, &CallbackContext, fl);
}

HRESULT m_IDirectInputDeviceX::Escape(LPDIEFFESCAPE pesc)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->Escape(pesc);
}

HRESULT m_IDirectInputDeviceX::Poll()
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->Poll();
}

HRESULT m_IDirectInputDeviceX::SendDeviceData(DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	if (pdwInOut && *pdwInOut == (DWORD)-1)
	{
		return DIERR_INVALIDPARAM;
	}

	EnterCriticalSection(&dics);

	// Check the size of the array
	if (rgdod && pdwInOut && *pdwInOut > pdod.size())
	{
		pdod.resize(*pdwInOut);

		Logging::LogDebug() << __FUNCTION__ << " Update dod memory! " << *pdwInOut;
	}

	// Copy array
	if (rgdod && pdwInOut && cbObjectData)
	{
		ZeroMemory(&pdod[0], sizeof(DIDEVICEOBJECTDATA) * pdod.size());
		for (UINT x = 0; x < *pdwInOut; x++)
		{
			CopyMemory(&pdod[x], (void*)((DWORD)rgdod + (cbObjectData * x)), cbObjectData);
		}
	}

	HRESULT hr = ProxyInterface->SendDeviceData(sizeof(DIDEVICEOBJECTDATA), (rgdod) ? &pdod[0] : nullptr, pdwInOut, fl);

	LeaveCriticalSection(&dics);

	return hr;
}

HRESULT m_IDirectInputDeviceX::EnumEffectsInFileA(LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceA()->EnumEffectsInFile(lpszFileName, pec, pvRef, dwFlags);
}

HRESULT m_IDirectInputDeviceX::EnumEffectsInFileW(LPCWSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceW()->EnumEffectsInFile(lpszFileName, pec, pvRef, dwFlags);
}

HRESULT m_IDirectInputDeviceX::WriteEffectToFileA(LPCSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceA()->WriteEffectToFile(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
}

HRESULT m_IDirectInputDeviceX::WriteEffectToFileW(LPCWSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterfaceW()->WriteEffectToFile(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
}
