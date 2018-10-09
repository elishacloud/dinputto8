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

#include "dllmain.h"
#include "dinput.h"

#ifndef DINPUTTO8NOLOG
std::ofstream Logging::LOG("dinput.log");
#else
std::ofstream Logging::LOG;
#endif
AddressLookupTableDinput<void> ProxyAddressLookupTable = AddressLookupTableDinput<void>();

DirectInput8CreateProc m_pDirectInput8Create;
DllCanUnloadNowProc m_pDllCanUnloadNow;
DllGetClassObjectProc m_pDllGetClassObject;
DllRegisterServerProc m_pDllRegisterServer;
DllUnregisterServerProc m_pDllUnregisterServer;

bool WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	UNREFERENCED_PARAMETER(hModule);

	static HMODULE dinput8dll = nullptr;

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Load dll
		char path[MAX_PATH];
		strcpy_s(path, "dinput8.dll");
		Logging::Log() << "Loading " << path;
		dinput8dll = LoadLibraryA(path);

		// Get function addresses
		m_pDirectInput8Create = (DirectInput8CreateProc)GetProcAddress(dinput8dll, "DirectInput8Create");
		m_pDllCanUnloadNow = (DllCanUnloadNowProc)GetProcAddress(dinput8dll, "DllCanUnloadNow");
		m_pDllGetClassObject = (DllGetClassObjectProc)GetProcAddress(dinput8dll, "DllGetClassObject");
		m_pDllRegisterServer = (DllRegisterServerProc)GetProcAddress(dinput8dll, "DllRegisterServer");
		m_pDllUnregisterServer = (DllUnregisterServerProc)GetProcAddress(dinput8dll, "DllUnregisterServer");
		break;

	case DLL_PROCESS_DETACH:
		FreeLibrary(dinput8dll);
		break;
	}

	return true;
}

HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* lplpDirectInput, LPUNKNOWN punkOuter)
{
	if (!m_pDirectInput8Create || !lplpDirectInput)
	{
		return E_FAIL;
	}

	Logging::Log() << __FUNCTION__ << " Redirecting to --> 'DirectInput8Create'";

	HRESULT hr = m_pDirectInput8Create(hinst, dwVersion, IID_IDirectInput8A, (LPVOID*)lplpDirectInput, punkOuter);

	if (SUCCEEDED(hr))
	{
		*lplpDirectInput = ProxyAddressLookupTable.FindAddress<m_IDirectInputA>(*lplpDirectInput);
	}

	return hr;
}

HRESULT WINAPI DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riid, LPVOID * lplpDD, LPUNKNOWN punkOuter)
{
	if (!m_pDirectInput8Create || !lplpDD)
	{
		return E_FAIL;
	}

	Logging::Log() << __FUNCTION__ << " Redirecting to --> 'DirectInput8Create' using " << riid;

	HRESULT hr = m_pDirectInput8Create(hinst, dwVersion, (GetStringType(riid) == UNICODE) ? IID_IDirectInput8W : IID_IDirectInput8A, lplpDD, punkOuter);

	if (SUCCEEDED(hr))
	{
		genericQueryInterface(riid, lplpDD);
	}

	return hr;
}

HRESULT WINAPI DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW* lplpDirectInput, LPUNKNOWN punkOuter)
{
	if (!m_pDirectInput8Create || !lplpDirectInput)
	{
		return E_FAIL;
	}

	Logging::Log() << __FUNCTION__ << " Redirecting to --> 'DirectInput8Create'";

	HRESULT hr = m_pDirectInput8Create(hinst, dwVersion, IID_IDirectInput8W, (LPVOID*)lplpDirectInput, punkOuter);

	if (SUCCEEDED(hr))
	{
		*lplpDirectInput = ProxyAddressLookupTable.FindAddress<m_IDirectInputW>(*lplpDirectInput);
	}

	return hr;
}

HRESULT WINAPI DllCanUnloadNow()
{
	if (!m_pDllCanUnloadNow)
	{
		return E_FAIL;
	}

	return m_pDllCanUnloadNow();
}

HRESULT WINAPI DllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv)
{
	if (!m_pDllGetClassObject || !ppv)
	{
		return E_FAIL;
	}

	DWORD StringType = GetStringType(riid);

	HRESULT hr = m_pDllGetClassObject(rclsid, (StringType == ANSI_CHARSET) ? IID_IDirectInput8A : (StringType == UNICODE) ? IID_IDirectInput8W : riid, ppv);

	if (SUCCEEDED(hr))
	{
		genericQueryInterface(riid, ppv);
	}

	return hr;
}

HRESULT WINAPI DllRegisterServer()
{
	if (!m_pDllRegisterServer)
	{
		return E_FAIL;
	}

	return m_pDllRegisterServer();
}

HRESULT WINAPI DllUnregisterServer()
{
	if (!m_pDllUnregisterServer)
	{
		return E_FAIL;
	}

	return m_pDllUnregisterServer();
}
