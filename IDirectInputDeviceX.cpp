/**
* Copyright (C) 2022 Elisha Riedlinger
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

const DIDATAFORMAT c_dfDIKeyboard = {
	sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),
	DIDF_RELAXIS,
	MAX_KEYBAORD,
	sizeof(dfDIKeyboard) / sizeof(*dfDIKeyboard),
	(LPDIOBJECTDATAFORMAT)dfDIKeyboard
};

HRESULT m_IDirectInputDeviceX::QueryInterface(REFIID riid, LPVOID FAR * ppvObj, DWORD DirectXVersion)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	DWORD DxVersion = (CheckWrapperType(riid)) ? GetGUIDVersion(riid) : DirectXVersion;

	return ProxyQueryInterface(ProxyInterface, riid, ppvObj, GetWrapperType(DxVersion), GetWrapperInterfaceX(DxVersion));
}

LPVOID m_IDirectInputDeviceX::GetWrapperInterfaceX(DWORD DirectXVersion)
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

ULONG m_IDirectInputDeviceX::AddRef()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputDeviceX::Release()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	ULONG ref = ProxyInterface->Release();

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT m_IDirectInputDeviceX::GetCapabilities(LPDIDEVCAPS lpDIDevCaps)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetCapabilities(lpDIDevCaps);
}

template HRESULT m_IDirectInputDeviceX::EnumObjectsX<IDirectInputDevice8A, LPDIENUMDEVICEOBJECTSCALLBACKA>(LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD);
template HRESULT m_IDirectInputDeviceX::EnumObjectsX<IDirectInputDevice8W, LPDIENUMDEVICEOBJECTSCALLBACKW>(LPDIENUMDEVICEOBJECTSCALLBACKW, LPVOID, DWORD);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::EnumObjectsX(V lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return GetProxyInterface<T>()->EnumObjects(lpCallback, pvRef, dwFlags);
}

HRESULT m_IDirectInputDeviceX::GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDeviceX::SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->SetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDeviceX::Acquire()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!CanAquireDevice)
	{
		CanAquireDevice = true;
		return DIERR_NOTINITIALIZED;
	}

	return ProxyInterface->Acquire();
}

HRESULT m_IDirectInputDeviceX::Unacquire()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->Unacquire();
}

HRESULT m_IDirectInputDeviceX::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	// Handle data format offset
	if (Offset)
	{
		UCHAR tmp[MAX_KEYBAORD] = {};

		HRESULT hr = ProxyInterface->GetDeviceState(MAX_KEYBAORD, tmp);

		if (SUCCEEDED(hr))
		{
			ZeroMemory(lpvData, cbData);

			for (DWORD x = Offset; x < min(cbData, Offset + MAX_KEYBAORD); x++)
			{
				((UCHAR*)lpvData)[x] = tmp[x - Offset];
			}
		}

		return hr;
	}

	return ProxyInterface->GetDeviceState(cbData, lpvData);
}

HRESULT m_IDirectInputDeviceX::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	//  If just peeking at data
	if (dwFlags == DIGDD_PEEK || !rgdod)
	{
		return ProxyInterface->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, pdwInOut, dwFlags);
	}

	// Check for valid parameters
	if (!pdwInOut || !*pdwInOut || *pdwInOut == (DWORD)-1 || !cbObjectData)
	{
		return DIERR_INVALIDPARAM;
	}

	EnterCriticalSection(&dics);

	// Check the size of the array
	if (*pdwInOut > pdod.size())
	{
		pdod.resize(*pdwInOut);

		Logging::LogDebug() << __FUNCTION__ << " Update dod memory! " << *pdwInOut;
	}

	HRESULT hr = ProxyInterface->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &pdod[0], pdwInOut, dwFlags);

	// Copy array
	if (SUCCEEDED(hr))
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
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	// Fix unsupported flags
	if (lpdf && lpdf->dwNumObjs && lpdf->dwObjSize == sizeof(DIOBJECTDATAFORMAT))
	{
		// Handle data format offset
		if (lpdf->dwNumObjs < MAX_KEYBAORD && (lpdf->rgodf[0].dwType & DIDFT_BUTTON) && lpdf->rgodf[0].dwOfs != 0 && *lpdf->rgodf[0].pguid == GUID_Key)
		{
			Offset = lpdf->rgodf[0].dwOfs - 1;

			if (SUCCEEDED(ProxyInterface->SetDataFormat(&c_dfDIKeyboard)))
			{
				return DI_OK;
			}
		}

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

		Offset = 0;

		HRESULT hr = ProxyInterface->SetDataFormat(&df);

		LeaveCriticalSection(&dics);

		return hr;
	}

	return ProxyInterface->SetDataFormat(lpdf);
}

HRESULT m_IDirectInputDeviceX::SetEventNotification(HANDLE hEvent)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->SetEventNotification(hEvent);
}

HRESULT m_IDirectInputDeviceX::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	HRESULT hr = ProxyInterface->SetCooperativeLevel(hwnd, dwFlags);

	if (SUCCEEDED(hr))
	{
		CanAquireDevice = true;
	}

	return hr;
}

template HRESULT m_IDirectInputDeviceX::GetObjectInfoX<IDirectInputDevice8A, LPDIDEVICEOBJECTINSTANCEA>(LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD);
template HRESULT m_IDirectInputDeviceX::GetObjectInfoX<IDirectInputDevice8W, LPDIDEVICEOBJECTINSTANCEW>(LPDIDEVICEOBJECTINSTANCEW, DWORD, DWORD);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::GetObjectInfoX(V pdidoi, DWORD dwObj, DWORD dwHow)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return GetProxyInterface<T>()->GetObjectInfo(pdidoi, dwObj, dwHow);
}

template HRESULT m_IDirectInputDeviceX::GetDeviceInfoX<IDirectInputDevice8A, LPDIDEVICEINSTANCEA>(LPDIDEVICEINSTANCEA);
template HRESULT m_IDirectInputDeviceX::GetDeviceInfoX<IDirectInputDevice8W, LPDIDEVICEINSTANCEW>(LPDIDEVICEINSTANCEW);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::GetDeviceInfoX(V pdidi)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	HRESULT hr = GetProxyInterface<T>()->GetDeviceInfo(pdidi);

	if (SUCCEEDED(hr) && pdidi && pdidi->dwSize)
	{
		pdidi->dwDevType = ConvertDevTypeTo7(GET_DIDEVICE_TYPE(pdidi->dwDevType));
	}

	return hr;
}

HRESULT m_IDirectInputDeviceX::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputDeviceX::Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	HRESULT hr = ProxyInterface->Initialize(hinst, 0x0800, rguid);

	if (SUCCEEDED(hr))
	{
		diVersion = dwVersion;
	}

	return hr;
}

HRESULT m_IDirectInputDeviceX::CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT * ppdeff, LPUNKNOWN punkOuter)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ") Trying! " << rguid << " " << punkOuter;

	DIEFFECT eff;
	eff.dwSize = sizeof(DIEFFECT);
	if (lpeff)
	{
		ConvertEffect(eff, *lpeff);
		lpeff = &eff;
	}

	HRESULT hr = ProxyInterface->CreateEffect(rguid, lpeff, ppdeff, punkOuter);

	if (SUCCEEDED(hr) && ppdeff)
	{
		m_IDirectInputEffect* DIEffect = new m_IDirectInputEffect((IDirectInputEffect*)*ppdeff);
		DIEffect->SetVersion(diVersion);

		*ppdeff = DIEffect;
	}
	else
	{
		Logging::LogDebug() << __FUNCTION__ << " (" << this << ") Failed! hr: " << (DIERR)hr;
	}

	return hr;
}

template HRESULT m_IDirectInputDeviceX::EnumEffectsX<IDirectInputDevice8A, LPDIENUMEFFECTSCALLBACKA>(LPDIENUMEFFECTSCALLBACKA, LPVOID, DWORD);
template HRESULT m_IDirectInputDeviceX::EnumEffectsX<IDirectInputDevice8W, LPDIENUMEFFECTSCALLBACKW>(LPDIENUMEFFECTSCALLBACKW, LPVOID, DWORD);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::EnumEffectsX(V lpCallback, LPVOID pvRef, DWORD dwEffType)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return GetProxyInterface<T>()->EnumEffects(lpCallback, pvRef, dwEffType);
}

template HRESULT m_IDirectInputDeviceX::GetEffectInfoX<IDirectInputDevice8A, LPDIEFFECTINFOA>(LPDIEFFECTINFOA, REFGUID);
template HRESULT m_IDirectInputDeviceX::GetEffectInfoX<IDirectInputDevice8W, LPDIEFFECTINFOW>(LPDIEFFECTINFOW, REFGUID);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::GetEffectInfoX(V pdei, REFGUID rguid)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return GetProxyInterface<T>()->GetEffectInfo(pdei, rguid);
}

HRESULT m_IDirectInputDeviceX::GetForceFeedbackState(LPDWORD pdwOut)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetForceFeedbackState(pdwOut);
}

HRESULT m_IDirectInputDeviceX::SendForceFeedbackCommand(DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->SendForceFeedbackCommand(dwFlags);
}

HRESULT m_IDirectInputDeviceX::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!lpCallback)
	{
		return DIERR_INVALIDPARAM;
	}

	// Callback structure
	struct EffectEnumerator
	{
		LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback;
		LPVOID pvRef;

		static BOOL CALLBACK EnumEffectCallback(LPDIRECTINPUTEFFECT pdeff, LPVOID pvRef)
		{
			EffectEnumerator* self = (EffectEnumerator*)pvRef;

			if (pdeff)
			{
				pdeff = ProxyAddressLookupTable.FindAddress<m_IDirectInputEffect>(pdeff);
			}

			return self->lpCallback(pdeff, self->pvRef);
		}
	} CallbackContext;
	CallbackContext.pvRef = pvRef;
	CallbackContext.lpCallback = lpCallback;

	return ProxyInterface->EnumCreatedEffectObjects(EffectEnumerator::EnumEffectCallback, &CallbackContext, fl);
}

HRESULT m_IDirectInputDeviceX::Escape(LPDIEFFESCAPE pesc)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->Escape(pesc);
}

HRESULT m_IDirectInputDeviceX::Poll()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	HRESULT hr = ProxyInterface->Poll();

	return (hr == DI_NOEFFECT) ? DI_OK : hr;
}

HRESULT m_IDirectInputDeviceX::SendDeviceData(DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!pdwInOut || !rgdod || !cbObjectData)
	{
		return DIERR_INVALIDPARAM;
	}

	EnterCriticalSection(&dics);

	// Check the size of the array
	if (*pdwInOut > pdod.size())
	{
		pdod.resize(*pdwInOut);

		Logging::LogDebug() << __FUNCTION__ << " Update dod memory! " << *pdwInOut;
	}

	// Copy array
	ZeroMemory(&pdod[0], sizeof(DIDEVICEOBJECTDATA) * pdod.size());
	for (UINT x = 0; x < *pdwInOut; x++)
	{
		CopyMemory(&pdod[x], (void*)((DWORD)rgdod + (cbObjectData * x)), cbObjectData);
	}

	HRESULT hr = ProxyInterface->SendDeviceData(sizeof(DIDEVICEOBJECTDATA), &pdod[0], pdwInOut, fl);

	LeaveCriticalSection(&dics);

	return hr;
}

template HRESULT m_IDirectInputDeviceX::EnumEffectsInFileX<IDirectInputDevice8A, LPCSTR>(LPCSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
template HRESULT m_IDirectInputDeviceX::EnumEffectsInFileX<IDirectInputDevice8W, LPCWSTR>(LPCWSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::EnumEffectsInFileX(V lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return GetProxyInterface<T>()->EnumEffectsInFile(lpszFileName, pec, pvRef, dwFlags);
}

template HRESULT m_IDirectInputDeviceX::WriteEffectToFileX<IDirectInputDevice8A, LPCSTR>(LPCSTR, DWORD, LPDIFILEEFFECT, DWORD);
template HRESULT m_IDirectInputDeviceX::WriteEffectToFileX<IDirectInputDevice8W, LPCWSTR>(LPCWSTR, DWORD, LPDIFILEEFFECT, DWORD);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::WriteEffectToFileX(V lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return GetProxyInterface<T>()->WriteEffectToFile(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
}
