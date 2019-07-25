#pragma once

#define INITGUID

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class m_IDirectInputA;
class m_IDirectInputW;
class m_IDirectInput2A;
class m_IDirectInput2W;
class m_IDirectInput7A;
class m_IDirectInput7W;
class m_IDirectInputDeviceA;
class m_IDirectInputDeviceW;
class m_IDirectInputDevice2A;
class m_IDirectInputDevice2W;
class m_IDirectInputDevice7A;
class m_IDirectInputDevice7W;
class m_IDirectInputEffect;
class m_IDirectInputEnumEffect;
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
	DWORD GetStringType(REFIID riid);
	REFCLSID ConvertREFCLSID(REFCLSID rclsid);
	REFIID ConvertREFIID(REFIID riid);
	HRESULT ProxyQueryInterface(LPVOID ProxyInterface, REFIID riid, LPVOID * ppvObj, REFIID WrapperID, LPVOID WrapperInterface);
	void WINAPI genericQueryInterface(REFIID riid, LPVOID * ppvObj);
	void SetCriticalSection(bool &ThreadSyncFlag);
	void ReleaseCriticalSection(bool &ThreadSyncFlag);
}

extern AddressLookupTableDinput<void> ProxyAddressLookupTable;
extern DWORD diVersion;

using namespace dinputto8;

#include "Versions\IDirectInputA.h"
#include "Versions\IDirectInputW.h"
#include "Versions\IDirectInput2A.h"
#include "Versions\IDirectInput2W.h"
#include "Versions\IDirectInput7A.h"
#include "Versions\IDirectInput7W.h"
#include "Versions\IDirectInputDeviceA.h"
#include "Versions\IDirectInputDeviceW.h"
#include "Versions\IDirectInputDevice2A.h"
#include "Versions\IDirectInputDevice2W.h"
#include "Versions\IDirectInputDevice7A.h"
#include "Versions\IDirectInputDevice7W.h"
#include "IDirectInputEffect.h"
#include "IDirectInputEnum.h"
#include "IDirectInputTypes.h"
#include "IDirectInputX.h"
#include "IDirectInputDeviceX.h"
