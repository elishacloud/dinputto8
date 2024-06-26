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

#include "dinputto8.h"

DWORD dinputto8::GetStringType(REFIID riid)
{
	return (riid == IID_IDirectInputA || riid == IID_IDirectInput2A || riid == IID_IDirectInput7A ||
		riid == IID_IDirectInputDeviceA || riid == IID_IDirectInputDevice2A || riid == IID_IDirectInputDevice7A) ? ANSI_CHARSET :
		(riid == IID_IDirectInputW || riid == IID_IDirectInput2W || riid == IID_IDirectInput7W ||
		riid == IID_IDirectInputDeviceW || riid == IID_IDirectInputDevice2W || riid == IID_IDirectInputDevice7W) ? UNICODE_CHARSET : DIERR_UNSUPPORTED;
}

DWORD dinputto8::GetGUIDVersion(REFIID riid)
{
	return (riid == IID_IDirectInputA || riid == IID_IDirectInputW || riid == IID_IDirectInputDeviceA || riid == IID_IDirectInputDeviceW) ? 1 :
		(riid == IID_IDirectInput2A || riid == IID_IDirectInput2W || riid == IID_IDirectInputDevice2A || riid == IID_IDirectInputDevice2W) ? 2 :
		(riid == IID_IDirectInput7A || riid == IID_IDirectInput7W || riid == IID_IDirectInputDevice7A || riid == IID_IDirectInputDevice7W) ? 7 : 0;
}

REFIID dinputto8::ReplaceIIDUnknown(REFIID riid, REFIID guid)
{
	return (riid == IID_IUnknown) ? guid : riid;
}

REFCLSID dinputto8::ConvertREFCLSID(REFCLSID rclsid)
{
	return (rclsid == CLSID_DirectInput) ? CLSID_DirectInput8 :
		(rclsid == CLSID_DirectInputDevice) ? CLSID_DirectInputDevice8 : rclsid;
}

REFIID dinputto8::ConvertREFIID(REFIID riid)
{
	return (riid == IID_IDirectInputA || riid == IID_IDirectInput2A || riid == IID_IDirectInput7A) ? IID_IDirectInput8A :
		(riid == IID_IDirectInputW || riid == IID_IDirectInput2W || riid == IID_IDirectInput7W) ? IID_IDirectInput8W :
		(riid == IID_IDirectInputDeviceA || riid == IID_IDirectInputDevice2A || riid == IID_IDirectInputDevice7A) ? IID_IDirectInputDevice8A :
		(riid == IID_IDirectInputDeviceW || riid == IID_IDirectInputDevice2W || riid == IID_IDirectInputDevice7W) ? IID_IDirectInputDevice8W : riid;
}

HRESULT dinputto8::ProxyQueryInterface(LPVOID ProxyInterface, REFIID riid, LPVOID * ppvObj, REFIID WrapperID, LPVOID WrapperInterface)
{
	Logging::LogDebug() << __FUNCTION__ << " Query for " << riid << " from " << WrapperID;

	if (!ppvObj)
	{
		return E_POINTER;
	}

	if ((riid == WrapperID || riid == IID_IUnknown) && ppvObj)
	{
		((IUnknown*)WrapperInterface)->AddRef();

		*ppvObj = WrapperInterface;

		return S_OK;
	}

	HRESULT hr = ((IUnknown*)ProxyInterface)->QueryInterface(ConvertREFIID(riid), ppvObj);

	Logging::LogDebug() << __FUNCTION__ << " QueryInterface --> '" << riid << "' QueryInterface result: " << (DIERR)hr;

	if (SUCCEEDED(hr))
	{
		genericQueryInterface(riid, ppvObj);
	}

	return hr;
}

void WINAPI dinputto8::genericQueryInterface(REFIID riid, LPVOID * ppvObj)
{
	if (!ppvObj || !*ppvObj)
	{
		return;
	}

#define QUERYINTERFACE(x) \
	if (riid == IID_ ## x) \
		{ \
			Logging::LogDebug() << "Getting device for: m_" ## #x; \
			*ppvObj = ProxyAddressLookupTable.FindAddress<m_ ## x>(*ppvObj); \
		}

	QUERYINTERFACE(IDirectInputA);
	QUERYINTERFACE(IDirectInputW);
	QUERYINTERFACE(IDirectInput2A);
	QUERYINTERFACE(IDirectInput2W);
	QUERYINTERFACE(IDirectInput7A);
	QUERYINTERFACE(IDirectInput7W);
	QUERYINTERFACE(IDirectInputDeviceA);
	QUERYINTERFACE(IDirectInputDeviceW);
	QUERYINTERFACE(IDirectInputDevice2A);
	QUERYINTERFACE(IDirectInputDevice2W);
	QUERYINTERFACE(IDirectInputDevice7A);
	QUERYINTERFACE(IDirectInputDevice7W);
	QUERYINTERFACE(IDirectInputEffect);
}

HRESULT dinputto8::hresValidInstanceAndVersion(HINSTANCE& hinst, DWORD dwVersion)
{
	bool bValidInstance;
	if (hinst != nullptr)
	{
		wchar_t path[4];
		bValidInstance = GetModuleFileNameW(hinst, path, std::size(path) - 1) != 0;
	}
	else
	{
		// DInput version 0x300 permits no instance...
		bValidInstance = dwVersion == 0x300;
	}

	if (!bValidInstance)
	{
		return DIERR_INVALIDPARAM;
	}

	// ...but DInput8 does not, so if the instance is empty, give it one or else it'll fail.
	if (hinst == nullptr && dwVersion == 0x300)
	{
		hinst = ::GetModuleHandle(nullptr);
	}

	if (dwVersion == 0x300 || dwVersion == 0x500 || dwVersion == 0x50A || dwVersion == 0x5B2 || dwVersion == 0x602 || dwVersion == 0x61A || dwVersion == 0x700)
	{
		return DI_OK;
	}

	if (dwVersion == 0)
	{
		return DIERR_NOTINITIALIZED;
	}
	return dwVersion < 0x700 ? DIERR_BETADIRECTINPUTVERSION : DIERR_OLDDIRECTINPUTVERSION;
}
