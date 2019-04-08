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

namespace dinputto8
{
	REFIID ConvertREFIID(REFIID riid);
}

DWORD dinputto8::GetStringType(REFIID riid)
{
	return (riid == IID_IDirectInputA || riid == IID_IDirectInput2A || riid == IID_IDirectInput7A ||
		riid == IID_IDirectInputDeviceA || riid == IID_IDirectInputDevice2A || riid == IID_IDirectInputDevice7A) ? ANSI_CHARSET :
		(riid == IID_IDirectInputW || riid == IID_IDirectInput2W || riid == IID_IDirectInput7W ||
		riid == IID_IDirectInputDeviceW || riid == IID_IDirectInputDevice2W || riid == IID_IDirectInputDevice7W) ? DEFAULT_CHARSET : DIERR_UNSUPPORTED;
}

REFIID dinputto8::ConvertREFIID(REFIID riid)
{
	if (riid == IID_IDirectInputA || riid == IID_IDirectInput2A || riid == IID_IDirectInput7A)
	{
		return IID_IDirectInput8A;
	}
	else if (riid == IID_IDirectInputW || riid == IID_IDirectInput2W || riid == IID_IDirectInput7W)
	{
		return IID_IDirectInput8W;
	}
	else if (riid == IID_IDirectInputDeviceA || riid == IID_IDirectInputDevice2A || riid == IID_IDirectInputDevice7A)
	{
		return IID_IDirectInputDevice8A;
	}
	else if (riid == IID_IDirectInputDeviceW || riid == IID_IDirectInputDevice2W || riid == IID_IDirectInputDevice7W)
	{
		return IID_IDirectInputDevice8W;
	}

	return riid;
}

HRESULT dinputto8::ProxyQueryInterface(LPVOID ProxyInterface, REFIID riid, LPVOID * ppvObj, REFIID WrapperID, LPVOID WrapperInterface)
{
	if (!ppvObj)
	{
		return E_FAIL;
	}

	if (riid == WrapperID || riid == IID_IUnknown)
	{
		((IUnknown*)ProxyInterface)->AddRef();

		*ppvObj = WrapperInterface;

		return S_OK;
	}

	HRESULT hr = ((IUnknown*)ProxyInterface)->QueryInterface(ConvertREFIID(riid), ppvObj);

	Logging::LogDebug() << __FUNCTION__ << " QueryInterface --> '" << riid << "' QueryInterface result: " << hr;

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
