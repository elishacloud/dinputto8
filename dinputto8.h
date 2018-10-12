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
#include "BasicLogging.h"

typedef HRESULT(WINAPI *DirectInput8CreateProc)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
typedef HRESULT(WINAPI *DllCanUnloadNowProc)();
typedef	HRESULT(WINAPI *DllGetClassObjectProc)(REFCLSID, REFIID, LPVOID *);
typedef HRESULT(WINAPI *DllRegisterServerProc)();
typedef HRESULT(WINAPI *DllUnregisterServerProc)();

DWORD GetStringType(REFIID riid);
HRESULT genericQueryInterface(REFIID CalledID, LPVOID * ppvObj);
HRESULT ProxyQueryInterface(LPVOID ProxyInterface, REFIID riid, LPVOID * ppvObj, REFIID WrapperID, LPVOID WrapperInterface);
extern AddressLookupTableDinput<void> ProxyAddressLookupTable;
extern DWORD diVersion;

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
#include "IDirectInputEnumEffect.h"
#include "IDirectInputX.h"
#include "IDirectInputDeviceX.h"
