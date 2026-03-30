#pragma once

#define INITGUID

#define DIRECTINPUT_VERSION 0x0800
#define UNICODE_CHARSET DEFAULT_CHARSET
#include <dinput.h>
#include "ditypes.h"

class m_IDirectInputEffect;
class m_IDirectInputX;
class m_IDirectInputDeviceX;

#include <memory>
#include "AddressLookupTable.h"
#include "Logging\Logging.h"

typedef HRESULT(WINAPI *DirectInput8CreateProc)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
typedef HRESULT(WINAPI *DllCanUnloadNowProc)();
typedef	HRESULT(WINAPI *DllGetClassObjectProc)(REFCLSID, REFIID, LPVOID *);
typedef HRESULT(WINAPI *DllRegisterServerProc)();
typedef HRESULT(WINAPI *DllUnregisterServerProc)();

namespace dinputto8
{
	void SetCriticalSection();
	void ReleaseCriticalSection();

	DWORD GetStringType(REFIID riid);
	DWORD GetGUIDVersion(REFIID riid);
	REFIID ReplaceIIDUnknown(REFIID riid, REFIID guid);
	REFCLSID ConvertREFCLSID(REFCLSID rclsid);
	REFIID ConvertREFIID(REFIID riid);
	HRESULT hresValidInstanceAndVersion(HINSTANCE& hinst, DWORD dwVersion);
}

extern AddressLookupTableDinput<void> ProxyAddressLookupTable;

using namespace dinputto8;

#include "IDirectInputEffect.h"
#include "IDirectInputTypes.h"
#include "IDirectInputX.h"
#include "IDirectInputDeviceX.h"
