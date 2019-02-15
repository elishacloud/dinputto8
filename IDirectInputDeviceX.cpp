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

	ULONG x = InterlockedDecrement(&RefCount);

	if (x == 0)
	{
		WrapperInterface->DeleteMe();
	}

	return x;
}

HRESULT m_IDirectInputDeviceX::GetCapabilities(LPDIDEVCAPS lpDIDevCaps)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return ProxyInterface->GetCapabilities(lpDIDevCaps);
}

template HRESULT m_IDirectInputDeviceX::EnumObjects<LPDIENUMDEVICEOBJECTSCALLBACKA>(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
template HRESULT m_IDirectInputDeviceX::EnumObjects<LPDIENUMDEVICEOBJECTSCALLBACKW>(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags);
template <typename T>
HRESULT m_IDirectInputDeviceX::EnumObjects(T lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterface(lpCallback)->EnumObjects(lpCallback, pvRef, dwFlags);
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

	return ProxyInterface->GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
}

HRESULT m_IDirectInputDeviceX::SetDataFormat(LPCDIDATAFORMAT lpdf)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

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

	return ProxyInterface->SetCooperativeLevel(hwnd, dwFlags);
}

template HRESULT m_IDirectInputDeviceX::GetObjectInfo<LPDIDEVICEOBJECTINSTANCEA>(LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow);
template HRESULT m_IDirectInputDeviceX::GetObjectInfo<LPDIDEVICEOBJECTINSTANCEW>(LPDIDEVICEOBJECTINSTANCEW pdidoi, DWORD dwObj, DWORD dwHow);
template <typename T>
HRESULT m_IDirectInputDeviceX::GetObjectInfo(T pdidoi, DWORD dwObj, DWORD dwHow)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterface(pdidoi)->GetObjectInfo(pdidoi, dwObj, dwHow);
}

template HRESULT m_IDirectInputDeviceX::GetDeviceInfo<LPDIDEVICEINSTANCEA>(LPDIDEVICEINSTANCEA pdidi);
template HRESULT m_IDirectInputDeviceX::GetDeviceInfo<LPDIDEVICEINSTANCEW>(LPDIDEVICEINSTANCEW pdidi);
template <typename T>
HRESULT m_IDirectInputDeviceX::GetDeviceInfo(T pdidi)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	HRESULT hr = GetProxyInterface(pdidi)->GetDeviceInfo(pdidi);

	pdidi->dwDevType = ConvertDevTypeTo7(GET_DIDEVICE_TYPE(pdidi->dwDevType));

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

template HRESULT m_IDirectInputDeviceX::EnumEffects<LPDIENUMEFFECTSCALLBACKA>(LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType);
template HRESULT m_IDirectInputDeviceX::EnumEffects<LPDIENUMEFFECTSCALLBACKW>(LPDIENUMEFFECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwEffType);
template <typename T>
HRESULT m_IDirectInputDeviceX::EnumEffects(T lpCallback, LPVOID pvRef, DWORD dwEffType)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterface(lpCallback)->EnumEffects(lpCallback, pvRef, dwEffType);
}

template HRESULT m_IDirectInputDeviceX::GetEffectInfo<LPDIEFFECTINFOA>(LPDIEFFECTINFOA pdei, REFGUID rguid);
template HRESULT m_IDirectInputDeviceX::GetEffectInfo<LPDIEFFECTINFOW>(LPDIEFFECTINFOW pdei, REFGUID rguid);
template <typename T>
HRESULT m_IDirectInputDeviceX::GetEffectInfo(T pdei, REFGUID rguid)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterface(pdei)->GetEffectInfo(pdei, rguid);
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

	return ProxyInterface->SendDeviceData(cbObjectData, rgdod, pdwInOut, fl);
}

template HRESULT m_IDirectInputDeviceX::EnumEffectsInFile<LPCSTR>(LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags);
template HRESULT m_IDirectInputDeviceX::EnumEffectsInFile<LPCWSTR>(LPCWSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags);
template <typename T>
HRESULT m_IDirectInputDeviceX::EnumEffectsInFile(T lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterface(lpszFileName)->EnumEffectsInFile(lpszFileName, pec, pvRef, dwFlags);
}

template HRESULT m_IDirectInputDeviceX::WriteEffectToFile<LPCSTR>(LPCSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags);
template HRESULT m_IDirectInputDeviceX::WriteEffectToFile<LPCWSTR>(LPCWSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags);
template <typename T>
HRESULT m_IDirectInputDeviceX::WriteEffectToFile(T lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << "(" << this << ")";

	return GetProxyInterface(lpszFileName)->WriteEffectToFile(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
}
