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

#include "dinputto8.h"

// Cached wrapper interface
namespace {
	m_IDirectInputEffect* WrapperInterfaceBackup = nullptr;
}

m_IDirectInputEffect* CreateEffectWrapper(IDirectInputEffect* aOriginal)
{
	SetCriticalSection();
	m_IDirectInputEffect* Interface = nullptr;
	if (WrapperInterfaceBackup)
	{
		Interface = WrapperInterfaceBackup;
		if (aOriginal)
		{
			WrapperInterfaceBackup = nullptr;
			Interface->SetProxy(aOriginal);
		}
	}
	else
	{
		Interface = new m_IDirectInputEffect(aOriginal);
		if (!aOriginal)
		{
			Interface->SetProxy(nullptr);
			WrapperInterfaceBackup = Interface;
		}
	}
	ReleaseCriticalSection();
	return Interface;
}

HRESULT m_IDirectInputEffect::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		if (ppvObj)
		{
			*ppvObj = nullptr;
		}
		return E_NOINTERFACE;
	}

	return ProxyQueryInterface(ProxyInterface, riid, ppvObj, WrapperID, WrapperInterface);
}

ULONG m_IDirectInputEffect::AddRef()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		return 0;
	}

	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputEffect::Release()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		return 0;
	}

	ULONG ref = ProxyInterface->Release();

	if (ref == 0)
	{
		// Don't delete wrapper interface
		SaveInterfaceAddress((m_IDirectInputEffect*&)WrapperInterface, WrapperInterfaceBackup);
	}

	return ref;
}

HRESULT m_IDirectInputEffect::Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}

	HRESULT hr = hresValidInstanceAndVersion(hinst, dwVersion);
	if (SUCCEEDED(hr))
	{
		hr = ProxyInterface->Initialize(hinst, 0x0800, rguid);

		if (SUCCEEDED(hr))
		{
			diVersion = dwVersion;
		}
	}

	return hr;
}

HRESULT m_IDirectInputEffect::GetEffectGuid(LPGUID pguid)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}

	return ProxyInterface->GetEffectGuid(pguid);
}

HRESULT m_IDirectInputEffect::GetParameters(LPDIEFFECT lpeff, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}

	if (!lpeff || !lpeff->dwSize)
	{
		return DIERR_INVALIDPARAM;
	}

	DIEFFECT eff = {};
	eff.dwSize = sizeof(DIEFFECT);

	HRESULT hr = ProxyInterface->GetParameters(&eff, dwFlags);

	if (SUCCEEDED(hr))
	{
		CopyMemory(lpeff, &eff, min(lpeff->dwSize, sizeof(DIEFFECT)));
	}

	return hr;
}

HRESULT m_IDirectInputEffect::SetParameters(LPCDIEFFECT lpeff, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ") Trying! " << Logging::hex(dwFlags);

	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}

#ifdef _DEBUG
	GUID guid;
	GetEffectGuid(&guid);
	LogEffectFormat(lpeff, guid);
#endif // DEBUG

	DIEFFECT eff = {};
	if (lpeff && lpeff->dwSize == sizeof(DIEFFECT_DX5))
	{
		*(DIEFFECT_DX5*)&eff = *(DIEFFECT_DX5*)lpeff;
		eff.dwSize = sizeof(DIEFFECT);
		lpeff = &eff;
	}

	HRESULT hr = ProxyInterface->SetParameters(lpeff, dwFlags);

	if (FAILED(hr))
	{
		Logging::LogDebug() << __FUNCTION__ << " (" << this << ") Failed! hr: " << (DIERR)hr;
	}

	return hr;
}

HRESULT m_IDirectInputEffect::Start(DWORD dwIterations, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ") Trying! " << dwIterations << " " << Logging::hex(dwFlags);

	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}

	HRESULT hr = ProxyInterface->Start(dwIterations, dwFlags);

	if (FAILED(hr))
	{
		Logging::LogDebug() << __FUNCTION__ << " (" << this << ") Failed! hr: " << (DIERR)hr;
	}

	return hr;
}

HRESULT m_IDirectInputEffect::Stop()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}

	return ProxyInterface->Stop();
}

HRESULT m_IDirectInputEffect::GetEffectStatus(LPDWORD pdwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}

	return ProxyInterface->GetEffectStatus(pdwFlags);
}

HRESULT m_IDirectInputEffect::Download()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}

	return ProxyInterface->Download();
}

HRESULT m_IDirectInputEffect::Unload()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}

	return ProxyInterface->Unload();
}

HRESULT m_IDirectInputEffect::Escape(LPDIEFFESCAPE pesc)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterface)
	{
		return DIERR_OBJECTNOTFOUND;
	}

	return ProxyInterface->Escape(pesc);
}
