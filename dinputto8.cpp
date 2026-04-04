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

#include <new>

std::ofstream LOG;

bool InitFlag = false;

AddressLookupTableDinput ProxyAddressLookupTable;

DirectInput8CreateProc m_pDirectInput8Create = nullptr;
DllCanUnloadNowProc m_pDllCanUnloadNow = nullptr;
DllGetClassObjectProc m_pDllGetClassObject = nullptr;

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
	HMODULE dinput8dll = LoadLibrary(TEXT("dinput8"));

	// Get function addresses
	m_pDirectInput8Create = (DirectInput8CreateProc)GetProcAddress(dinput8dll, "DirectInput8Create");
	m_pDllCanUnloadNow = (DllCanUnloadNowProc)GetProcAddress(dinput8dll, "DllCanUnloadNow");
	m_pDllGetClassObject = (DllGetClassObjectProc)GetProcAddress(dinput8dll, "DllGetClassObject");
}

HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* lplpDirectInput, LPUNKNOWN pUnkOuter)
{
	LOG_LIMIT(1, __FUNCTION__);

	return DirectInputCreateEx(hinst, dwVersion, IID_IDirectInputA, (LPVOID*)lplpDirectInput, pUnkOuter);
}

HRESULT WINAPI DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW* lplpDirectInput, LPUNKNOWN pUnkOuter)
{
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

	// DirectInputCreateEx can only be called with IDirectInput interfaces, not with IUnknown!
	if (riid != IID_IDirectInputA && riid != IID_IDirectInput2A && riid != IID_IDirectInput7A &&
		riid != IID_IDirectInputW && riid != IID_IDirectInput2W && riid != IID_IDirectInput7W)
	{
		return DIERR_NOINTERFACE;
	}

	LOG_LIMIT(3, "Redirecting 'DirectInputCreate' " << riid << " version " << Logging::hex(dwVersion) << " to --> 'DirectInput8Create'");

	if (pUnkOuter)
	{
		LOG_LIMIT(3, __FUNCTION__ << " Warning: 'pUnkOuter' is not null: " << pUnkOuter);
	}

	HRESULT hr = hresValidInstanceAndVersion(hinst, dwVersion);
	if (SUCCEEDED(hr))
	{
		typename m_IDirectInputX::proxy_type* Proxy;
		hr = m_pDirectInput8Create(hinst, 0x0800, m_IDirectInputX::proxy_iid, reinterpret_cast<LPVOID*>(&Proxy), nullptr);

		if (SUCCEEDED(hr))
		{
			m_IDirectInputX *Interface = new m_IDirectInputX(Proxy);
			Interface->SetVersion(dwVersion);

			hr = Interface->QueryInterface(riid, lplpDD);
			Interface->Release();
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
		return S_FALSE;
	}

	if (ModuleObjectCount::AnyObjectsInUse())
	{
		return S_FALSE;
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

	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	HRESULT hr = E_OUTOFMEMORY;
	*ppv = nullptr;

	ClassFactoryBase* wrapperFactory = nullptr;
	if (rclsid == m_IDirectInputX::wrapper_clsid)
	{
		IClassFactory* proxyFactory;
		HRESULT proxyHr = m_pDllGetClassObject(m_IDirectInputX::proxy_clsid, IID_PPV_ARGS(&proxyFactory));
		if (FAILED(proxyHr))
		{
			return proxyHr;
		}

		wrapperFactory = new(std::nothrow) ClassFactory<m_IDirectInputX>(proxyFactory);
	}
	else if (rclsid == m_IDirectInputDeviceX::wrapper_clsid)
	{
		IClassFactory* proxyFactory;
		HRESULT proxyHr = m_pDllGetClassObject(m_IDirectInputDeviceX::proxy_clsid, IID_PPV_ARGS(&proxyFactory));
		if (FAILED(proxyHr))
		{
			return proxyHr;
		}

		wrapperFactory = new(std::nothrow) ClassFactory<m_IDirectInputDeviceX>(proxyFactory);
	}
	else
	{
		return CLASS_E_CLASSNOTAVAILABLE;
	}

	if (wrapperFactory != nullptr)
	{
		hr = wrapperFactory->QueryInterface(riid, ppv);
		wrapperFactory->Release();
	}

	return hr;
}

HRESULT WINAPI DllRegisterServer()
{
	// Registering this wrapper does nothing
	return S_OK;
}

HRESULT WINAPI DllUnregisterServer()
{
	// Unregistering this wrapper does nothing
	return S_OK;
}
