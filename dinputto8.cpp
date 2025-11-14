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

#include "resource.h"
#include "dinputto8.h"

std::ofstream LOG;

bool InitFlag = false;

AddressLookupTableDinput<void> ProxyAddressLookupTable = AddressLookupTableDinput<void>();

DirectInput8CreateProc m_pDirectInput8Create = nullptr;
DllCanUnloadNowProc m_pDllCanUnloadNow = nullptr;
DllGetClassObjectProc m_pDllGetClassObject = nullptr;
DllRegisterServerProc m_pDllRegisterServer = nullptr;
DllUnregisterServerProc m_pDllUnregisterServer = nullptr;

HRESULT WINAPI DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riid, LPVOID * lplpDD, LPUNKNOWN pUnkOuter);

void InitDinput8()
{
	// Check if already initialized
	if (InitFlag)
	{
		return;
	}
	InitFlag = true;

	// Init logs
#ifdef NOLOGGING
	Logging::EnableLogging = false;
#else
	Logging::Open("dinput.log");
	Logging::Log() << "Starting dinputto8 v" << APP_VERSION;
	Logging::LogComputerManufacturer();
	Logging::LogOSVersion();
	Logging::LogProcessNameAndPID();
	Logging::LogGameType();
#endif

	// Load dll
	HMODULE dinput8dll = LoadLibraryA("dinput8.dll");

	// Get function addresses
	m_pDirectInput8Create = (DirectInput8CreateProc)GetProcAddress(dinput8dll, "DirectInput8Create");
	m_pDllCanUnloadNow = (DllCanUnloadNowProc)GetProcAddress(dinput8dll, "DllCanUnloadNow");
	m_pDllGetClassObject = (DllGetClassObjectProc)GetProcAddress(dinput8dll, "DllGetClassObject");
	m_pDllRegisterServer = (DllRegisterServerProc)GetProcAddress(dinput8dll, "DllRegisterServer");
	m_pDllUnregisterServer = (DllUnregisterServerProc)GetProcAddress(dinput8dll, "DllUnregisterServer");
}

HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* lplpDirectInput, LPUNKNOWN pUnkOuter)
{
	InitDinput8();

	LOG_LIMIT(1, __FUNCTION__);

	return DirectInputCreateEx(hinst, dwVersion, IID_IDirectInputA, (LPVOID*)lplpDirectInput, pUnkOuter);
}

HRESULT WINAPI DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW* lplpDirectInput, LPUNKNOWN pUnkOuter)
{
	InitDinput8();

	LOG_LIMIT(1, __FUNCTION__);

	return DirectInputCreateEx(hinst, dwVersion, IID_IDirectInputW, (LPVOID*)lplpDirectInput, pUnkOuter);
}

HRESULT WINAPI DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riid, LPVOID * lplpDD, LPUNKNOWN pUnkOuter)
{
	InitDinput8();

	LOG_LIMIT(1, __FUNCTION__);

	if (!m_pDirectInput8Create)
	{
		return DIERR_GENERIC;
	}

	LOG_LIMIT(3, "Redirecting 'DirectInputCreate' " << riid << " version " << Logging::hex(dwVersion) << " to --> 'DirectInput8Create'");

	if (pUnkOuter)
	{
		LOG_LIMIT(3, __FUNCTION__ << " Warning: 'pUnkOuter' is not null: " << pUnkOuter);
	}

	HRESULT hr = hresValidInstanceAndVersion(hinst, dwVersion);
	if (SUCCEEDED(hr))
	{
		hr = m_pDirectInput8Create(hinst, 0x0800, ConvertREFIID(riid), lplpDD, nullptr);

		if (SUCCEEDED(hr) && lplpDD)
		{
			m_IDirectInputX *Interface = new m_IDirectInputX((IDirectInput8W*)*lplpDD, riid);
			Interface->SetVersion(dwVersion);

			*lplpDD = Interface->GetWrapperInterfaceX(GetGUIDVersion(riid));
		}
	}

	return hr;
}

HRESULT WINAPI DllCanUnloadNow()
{
	InitDinput8();

	LOG_LIMIT(1, __FUNCTION__);

	if (!m_pDllCanUnloadNow)
	{
		return DIERR_GENERIC;
	}

	return m_pDllCanUnloadNow();
}

HRESULT WINAPI DllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv)
{
	InitDinput8();

	LOG_LIMIT(1, __FUNCTION__);

	if (!m_pDllGetClassObject)
	{
		return DIERR_GENERIC;
	}

	HRESULT hr = m_pDllGetClassObject(ConvertREFCLSID(rclsid), ConvertREFIID(riid), ppv);

	if (SUCCEEDED(hr) && ppv)
	{
		genericQueryInterface(riid, ppv);
	}

	return hr;
}

HRESULT WINAPI DllRegisterServer()
{
	InitDinput8();

	LOG_LIMIT(1, __FUNCTION__);

	if (!m_pDllRegisterServer)
	{
		return DIERR_GENERIC;
	}

	return m_pDllRegisterServer();
}

HRESULT WINAPI DllUnregisterServer()
{
	InitDinput8();

	LOG_LIMIT(1, __FUNCTION__);

	if (!m_pDllUnregisterServer)
	{
		return DIERR_GENERIC;
	}

	return m_pDllUnregisterServer();
}
