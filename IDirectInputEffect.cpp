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

HRESULT m_IDirectInputEffect::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	LogDebug() << __FUNCTION__;

	return ProxyQueryInterface(ProxyInterface, riid, ppvObj, WrapperID, WrapperInterface);
}

ULONG m_IDirectInputEffect::AddRef()
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputEffect::Release()
{
	LogDebug() << __FUNCTION__;

	ULONG x = ProxyInterface->Release();

	if (x == 0)
	{
		WrapperInterface->DeleteMe();
	}

	return x;
}

HRESULT m_IDirectInputEffect::Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->Initialize(hinst, dwVersion, rguid);
}

HRESULT m_IDirectInputEffect::GetEffectGuid(LPGUID pguid)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->GetEffectGuid(pguid);
}

HRESULT m_IDirectInputEffect::GetParameters(LPDIEFFECT peff, DWORD dwFlags)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->GetParameters(peff, dwFlags);
}

HRESULT m_IDirectInputEffect::SetParameters(LPCDIEFFECT peff, DWORD dwFlags)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->SetParameters(peff, dwFlags);
}

HRESULT m_IDirectInputEffect::Start(DWORD dwIterations, DWORD dwFlags)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->Start(dwIterations, dwFlags);
}

HRESULT m_IDirectInputEffect::Stop()
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->Stop();
}

HRESULT m_IDirectInputEffect::GetEffectStatus(LPDWORD pdwFlags)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->GetEffectStatus(pdwFlags);
}

HRESULT m_IDirectInputEffect::Download()
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->Download();
}

HRESULT m_IDirectInputEffect::Unload()
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->Unload();
}

HRESULT m_IDirectInputEffect::Escape(LPDIEFFESCAPE pesc)
{
	LogDebug() << __FUNCTION__;

	return ProxyInterface->Escape(pesc);
}
