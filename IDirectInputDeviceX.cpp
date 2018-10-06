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

HRESULT m_IDirectInputDeviceX::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	return ProxyQueryInterface(ProxyInterface, riid, ppvObj, WrapperID, WrapperInterface);
}

ULONG m_IDirectInputDeviceX::AddRef()
{
	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputDeviceX::Release()
{
	ULONG x = ProxyInterface->Release();

	if (x == 0)
	{
		WrapperInterface->DeleteMe();
	}

	return x;
}

HRESULT m_IDirectInputDeviceX::GetCapabilities(LPDIDEVCAPS lpDIDevCaps)
{
	return ProxyInterface->GetCapabilities(lpDIDevCaps);
}

template HRESULT m_IDirectInputDeviceX::EnumObjects<LPDIENUMDEVICEOBJECTSCALLBACKA>(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
template HRESULT m_IDirectInputDeviceX::EnumObjects<LPDIENUMDEVICEOBJECTSCALLBACKW>(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags);
template <typename T>
HRESULT m_IDirectInputDeviceX::EnumObjects(T lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return ProxyInterface->EnumObjects((LPDIENUMDEVICEOBJECTSCALLBACKW)lpCallback, pvRef, dwFlags);
}

HRESULT m_IDirectInputDeviceX::GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	return ProxyInterface->GetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDeviceX::SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	return ProxyInterface->SetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDeviceX::Acquire()
{
	return ProxyInterface->Acquire();
}

HRESULT m_IDirectInputDeviceX::Unacquire()
{
	return ProxyInterface->Unacquire();
}

HRESULT m_IDirectInputDeviceX::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	return ProxyInterface->GetDeviceState(cbData, lpvData);
}

HRESULT m_IDirectInputDeviceX::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	return ProxyInterface->GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
}

HRESULT m_IDirectInputDeviceX::SetDataFormat(LPCDIDATAFORMAT lpdf)
{
	return ProxyInterface->SetDataFormat(lpdf);
}

HRESULT m_IDirectInputDeviceX::SetEventNotification(HANDLE hEvent)
{
	return ProxyInterface->SetEventNotification(hEvent);
}

HRESULT m_IDirectInputDeviceX::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	return ProxyInterface->SetCooperativeLevel(hwnd, dwFlags);
}

template HRESULT m_IDirectInputDeviceX::GetObjectInfo<LPDIDEVICEOBJECTINSTANCEA>(LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow);
template HRESULT m_IDirectInputDeviceX::GetObjectInfo<LPDIDEVICEOBJECTINSTANCEW>(LPDIDEVICEOBJECTINSTANCEW pdidoi, DWORD dwObj, DWORD dwHow);
template <typename T>
HRESULT m_IDirectInputDeviceX::GetObjectInfo(T pdidoi, DWORD dwObj, DWORD dwHow)
{
	return ProxyInterface->GetObjectInfo((LPDIDEVICEOBJECTINSTANCEW)pdidoi, dwObj, dwHow);
}

template HRESULT m_IDirectInputDeviceX::GetDeviceInfo<LPDIDEVICEINSTANCEA>(LPDIDEVICEINSTANCEA pdidi);
template HRESULT m_IDirectInputDeviceX::GetDeviceInfo<LPDIDEVICEINSTANCEW>(LPDIDEVICEINSTANCEW pdidi);
template <typename T>
HRESULT m_IDirectInputDeviceX::GetDeviceInfo(T pdidi)
{
	return ProxyInterface->GetDeviceInfo((LPDIDEVICEINSTANCEW)pdidi);
}

HRESULT m_IDirectInputDeviceX::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputDeviceX::Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	return ProxyInterface->Initialize(hinst, dwVersion, rguid);
}

HRESULT m_IDirectInputDeviceX::CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT * ppdeff, LPUNKNOWN punkOuter)
{
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
	return ProxyInterface->EnumEffects((LPDIENUMEFFECTSCALLBACKW)lpCallback, pvRef, dwEffType);
}

template HRESULT m_IDirectInputDeviceX::GetEffectInfo<LPDIEFFECTINFOA>(LPDIEFFECTINFOA pdei, REFGUID rguid);
template HRESULT m_IDirectInputDeviceX::GetEffectInfo<LPDIEFFECTINFOW>(LPDIEFFECTINFOW pdei, REFGUID rguid);
template <typename T>
HRESULT m_IDirectInputDeviceX::GetEffectInfo(T pdei, REFGUID rguid)
{
	return ProxyInterface->GetEffectInfo((LPDIEFFECTINFOW)pdei, rguid);
}

HRESULT m_IDirectInputDeviceX::GetForceFeedbackState(LPDWORD pdwOut)
{
	return ProxyInterface->GetForceFeedbackState(pdwOut);
}

HRESULT m_IDirectInputDeviceX::SendForceFeedbackCommand(DWORD dwFlags)
{
	return ProxyInterface->SendForceFeedbackCommand(dwFlags);
}

HRESULT m_IDirectInputDeviceX::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl)
{
	ENUMEFFECT CallbackContext;
	CallbackContext.pvRef = pvRef;
	CallbackContext.lpCallback = lpCallback;

	return ProxyInterface->EnumCreatedEffectObjects(m_IDirectInputEnumEffect::EnumEffectCallback, &CallbackContext, fl);
}

HRESULT m_IDirectInputDeviceX::Escape(LPDIEFFESCAPE pesc)
{
	return ProxyInterface->Escape(pesc);
}

HRESULT m_IDirectInputDeviceX::Poll()
{
	return ProxyInterface->Poll();
}

HRESULT m_IDirectInputDeviceX::SendDeviceData(DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl)
{
	return ProxyInterface->SendDeviceData(cbObjectData, rgdod, pdwInOut, fl);
}

template HRESULT m_IDirectInputDeviceX::EnumEffectsInFile<LPCSTR>(LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags);
template HRESULT m_IDirectInputDeviceX::EnumEffectsInFile<LPCWSTR>(LPCWSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags);
template <typename T>
HRESULT m_IDirectInputDeviceX::EnumEffectsInFile(T lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
{
	return ProxyInterface->EnumEffectsInFile((LPCWSTR)lpszFileName, pec, pvRef, dwFlags);
}

template HRESULT m_IDirectInputDeviceX::WriteEffectToFile<LPCSTR>(LPCSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags);
template HRESULT m_IDirectInputDeviceX::WriteEffectToFile<LPCWSTR>(LPCWSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags);
template <typename T>
HRESULT m_IDirectInputDeviceX::WriteEffectToFile(T lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
{
	return ProxyInterface->WriteEffectToFile((LPCWSTR)lpszFileName, dwEntries, rgDiFileEft, dwFlags);
}
