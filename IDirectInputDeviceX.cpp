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

// Cached wrapper interface
m_IDirectInputDeviceA* DirectInputDeviceWrapperBackupA = nullptr;
m_IDirectInputDeviceW* DirectInputDeviceWrapperBackupW = nullptr;
m_IDirectInputDevice2A* DirectInputDeviceWrapperBackup2A = nullptr;
m_IDirectInputDevice2W* DirectInputDeviceWrapperBackup2W = nullptr;
m_IDirectInputDevice7A* DirectInputDeviceWrapperBackup7A = nullptr;
m_IDirectInputDevice7W* DirectInputDeviceWrapperBackup7W = nullptr;

HWND GetMainWindow()
{
	struct ENUMEDATA
	{
		DWORD processId = GetCurrentProcessId();
		HWND mainWindow = nullptr;
	} WindowData;

	EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			DWORD wndProcessId;
			GetWindowThreadProcessId(hwnd, &wndProcessId);
			ENUMEDATA* WindowData = reinterpret_cast<ENUMEDATA*>(lParam);

			if (wndProcessId == WindowData->processId && GetWindow(hwnd, GW_OWNER) == nullptr && IsWindowVisible(hwnd))
			{
				WindowData->mainWindow = hwnd;
				return FALSE;  // Stop enumerating once found
			}

			return TRUE;
		}, reinterpret_cast<LPARAM>(&WindowData));

	// Fallback to GetForegroundWindow if no suitable window is found
	if (!WindowData.mainWindow)
	{
		WindowData.mainWindow = GetForegroundWindow();
	}

	return WindowData.mainWindow;
}

// Our EnumObjectDataLUT contains only abstract types AXIS, BUTTON and POV, so consider it a match if
// any bit of the type matches (e.g. DIDFT_AXIS is 3 while DIDFT_ABSAXIS is 1).
template<typename T>
static auto FindByDITypeAndInstance(T& collection, DWORD dwType)
{
	auto it = collection.lower_bound(dwType & 0xFFFFFF);
	if (it != collection.end())
	{
		if (!(DIDFT_GETINSTANCE(it->first) == DIDFT_GETINSTANCE(dwType) &&
			(DIDFT_GETTYPE(dwType) == 0 || (DIDFT_GETTYPE(dwType) & DIDFT_GETTYPE(it->first)) != 0)))
		{
			it = collection.end();
		}
	}
	return it;
}

void m_IDirectInputDeviceX::InitializeEnumObjectData()
{
	DIDEVICEINSTANCEW didi { sizeof(didi) };
	if (SUCCEEDED(ProxyInterface->GetDeviceInfo(&didi)))
	{
		BOOL IsGamepad = FALSE;
		DevType7 = ConvertDevTypeTo7(GET_DIDEVICE_TYPE(didi.dwDevType), didi.wUsagePage, didi.wUsage, didi.dwDevType & DIDEVTYPE_HID, IsGamepad);

		// We only need to do this trickery for game controllers - keyboard/mice should be sorted fine
		// If this is ever proven to be false, just add code here for other DIDEVTYPE_*
		if (DevType7 == DIDEVTYPE_JOYSTICK)
		{
			// List based off the one in Wine, and restructured to match our use case better:
			// https://gitlab.winehq.org/besentv/wine/-/blob/wine-1.5.29/dlls/dinput/data_formats.c#L82
			// The important part is to keep instances of the same object type (axis, POV, button etc.) in order.
			auto AddObject = [this](DWORD key, DWORD dwOfs)
				{
					EnumObjectDataLUT.try_emplace(key, ObjectOrderValue{ dwOfs, dwOfs });
				};

			// Axes
			{
				DWORD dwAxisInstanceNo = 0;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), DIJOFS_X); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), DIJOFS_Y); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), DIJOFS_Z); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), DIJOFS_RX); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), DIJOFS_RY); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), DIJOFS_RZ); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), DIJOFS_SLIDER(0)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), DIJOFS_SLIDER(1)); dwAxisInstanceNo++;

				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lVX)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lVY)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lVZ)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lVRx)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lVRy)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lVRz)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,rglVSlider[0])); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,rglVSlider[1])); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lAX)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lAY)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lAZ)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lARx)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lARy)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lARz)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,rglASlider[0])); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,rglASlider[1])); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lFX)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lFY)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lFZ)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lFRx)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lFRy)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,lFRz)); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,rglFSlider[0])); dwAxisInstanceNo++;
				AddObject(DIDFT_AXIS | DIDFT_MAKEINSTANCE(dwAxisInstanceNo), FIELD_OFFSET(DIJOYSTATE2,rglFSlider[1])); dwAxisInstanceNo++;
			}

			// POV
			for (DWORD dwPovNo = 0; dwPovNo < 4; ++dwPovNo)
			{
				AddObject(DIDFT_POV | DIDFT_MAKEINSTANCE(dwPovNo), DIJOFS_POV(dwPovNo));
			}
			
			// Buttons
			for (DWORD dwButtonNo = 0; dwButtonNo < 128; ++dwButtonNo)
			{
				AddObject(DIDFT_BUTTON | DIDFT_MAKEINSTANCE(dwButtonNo), DIJOFS_BUTTON(dwButtonNo));
			}
		}
	}
}

void m_IDirectInputDeviceX::SetEnumObjectDataFromFormat(LPCDIDATAFORMAT lpdf)
{
	OffsetForMissingObjects = static_cast<DWORD>(-1);

	// First set all offsets to -1, then update them for any objects that are defined
	for (auto& element : EnumObjectDataLUT)
	{
		element.second.dwOfs = static_cast<DWORD>(-1);
	}

	// lpdf is most likely going to contain mostly DIDFT_ANYINSTANCE as instance numbers,
	// so we need a map to track those by type.
	// Mixing specific instances and DIDFT_ANYINSTANCE for the same type is not permitted,
	// so we don't need to worry about aliasing.
	std::map<DWORD, DWORD> InstanceNumbersByType {
		{ DIDFT_AXIS, 0 }, { DIDFT_BUTTON, 0 }, { DIDFT_POV, 0 },
	};

	// To support proper relocation of axes with specific GUIDs, we need to "remember" their respective instance numbers.
	// Annoying, but at least it can be contained into just this function.
	std::list<std::pair<const GUID*, DWORD>> InstanceNumbersByGUID;
	if (DevType7 == DIDEVTYPE_MOUSE)
	{
		DWORD instanceNo = 0;
		InstanceNumbersByGUID.emplace_back(&GUID_XAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_YAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_ZAxis, instanceNo++);
	}
	else if (DevType7 == DIDEVTYPE_JOYSTICK)
	{
		DWORD instanceNo = 0;
		InstanceNumbersByGUID.emplace_back(&GUID_XAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_YAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_ZAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RxAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RyAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RzAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_Slider, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_Slider, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_XAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_YAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_ZAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RxAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RyAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RzAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_Slider, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_Slider, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_XAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_YAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_ZAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RxAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RyAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RzAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_Slider, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_Slider, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_XAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_YAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_ZAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RxAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RyAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_RzAxis, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_Slider, instanceNo++);
		InstanceNumbersByGUID.emplace_back(&GUID_Slider, instanceNo++);
	}

	const auto begin = lpdf->rgodf;
	const auto end = begin + lpdf->dwNumObjs;
	for (auto it = begin; it != end; ++it)
	{
		auto instanceNumIt = FindByDITypeAndInstance(InstanceNumbersByType, DIDFT_GETTYPE(it->dwType));
		if (instanceNumIt != InstanceNumbersByType.end())
		{
			DWORD instanceNum = DIDFT_GETINSTANCE(it->dwType);
			if (instanceNum == 0xFFFF) // Is ANYINSTANCE in use?
			{
				// Does this GUID have a predefined instance?
				auto instanceGuidIt = std::find_if(InstanceNumbersByGUID.begin(), InstanceNumbersByGUID.end(),
					[pguid = it->pguid] (const auto& e)
					{
						const GUID& a = pguid ? *pguid : GUID{};
						const GUID& b = e.first ? *e.first : GUID{};
						return memcmp(&a, &b, sizeof(GUID)) == 0;
					});
				if (instanceGuidIt != InstanceNumbersByGUID.end())
				{
					instanceNum = instanceGuidIt->second;
					// Once a GUID has been used, "pop" it so the next object with the same GUID is used next time
					InstanceNumbersByGUID.erase(instanceGuidIt);
				}
				else
				{
					instanceNum = instanceNumIt->second++;
				}
			}

			auto enumObjectIt = FindByDITypeAndInstance(EnumObjectDataLUT, DIDFT_MAKEINSTANCE(instanceNum) | DIDFT_GETTYPE(it->dwType));
			if (enumObjectIt != EnumObjectDataLUT.end())
			{
				enumObjectIt->second.dwOfs = it->dwOfs;
			}
		}	
	}
}

HRESULT m_IDirectInputDeviceX::QueryInterface(REFIID riid, LPVOID FAR * ppvObj, DWORD DirectXVersion)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	DWORD DxVersion = (CheckWrapperType(riid)) ? GetGUIDVersion(riid) : DirectXVersion;

	return ProxyQueryInterface(ProxyInterface, riid, ppvObj, GetWrapperType(DxVersion), GetWrapperInterfaceX(DxVersion));
}

LPVOID m_IDirectInputDeviceX::GetWrapperInterfaceX(DWORD DirectXVersion)
{
	switch (DirectXVersion)
	{
	case 0:
		if (WrapperInterface7) return WrapperInterface7;
		if (WrapperInterface2) return WrapperInterface2;
		if (WrapperInterface) return WrapperInterface;
		break;
	case 1:
		if (StringType == ANSI_CHARSET)
		{
			return GetInterfaceAddress((m_IDirectInputDeviceA*&)WrapperInterface, DirectInputDeviceWrapperBackupA, (LPDIRECTINPUTDEVICEA)ProxyInterface, this);
		}
		else
		{
			return GetInterfaceAddress((m_IDirectInputDeviceW*&)WrapperInterface, DirectInputDeviceWrapperBackupW, (LPDIRECTINPUTDEVICEW)ProxyInterface, this);
		}
	case 2:
		if (StringType == ANSI_CHARSET)
		{
			return GetInterfaceAddress((m_IDirectInputDevice2A*&)WrapperInterface2, DirectInputDeviceWrapperBackup2A, (LPDIRECTINPUTDEVICE2A)ProxyInterface, this);
		}
		else
		{
			return GetInterfaceAddress((m_IDirectInputDevice2W*&)WrapperInterface2, DirectInputDeviceWrapperBackup2W, (LPDIRECTINPUTDEVICE2W)ProxyInterface, this);
		}
	case 7:
		if (StringType == ANSI_CHARSET)
		{
			return GetInterfaceAddress((m_IDirectInputDevice7A*&)WrapperInterface7, DirectInputDeviceWrapperBackup7A, (LPDIRECTINPUTDEVICE7A)ProxyInterface, this);
		}
		else
		{
			return GetInterfaceAddress((m_IDirectInputDevice7W*&)WrapperInterface7, DirectInputDeviceWrapperBackup7W, (LPDIRECTINPUTDEVICE7W)ProxyInterface, this);
		}
	}
	LOG_LIMIT(100, __FUNCTION__ << " Error: wrapper interface version not found: " << DirectXVersion);
	return nullptr;
}

ULONG m_IDirectInputDeviceX::AddRef()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->AddRef();
}

ULONG m_IDirectInputDeviceX::Release()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	ULONG ref = ProxyInterface->Release();

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

template HRESULT m_IDirectInputDeviceX::GetCapabilities<LPDIDEVCAPS, DIDEVICEINSTANCEA>(LPDIDEVCAPS);
template HRESULT m_IDirectInputDeviceX::GetCapabilities<LPDIDEVCAPS, DIDEVICEINSTANCEW>(LPDIDEVCAPS);
template <class T, class D>
HRESULT m_IDirectInputDeviceX::GetCapabilities(T lpDIDevCaps)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!lpDIDevCaps || !lpDIDevCaps->dwSize)
	{
		return DIERR_INVALIDPARAM;
	}

	HRESULT hr = ProxyInterface->GetCapabilities(lpDIDevCaps);

	if (SUCCEEDED(hr))
	{
		D didi = {};
		didi.dwSize = sizeof(D);

		hr = GetDeviceInfo(&didi);
		if (SUCCEEDED(hr))
		{
			lpDIDevCaps->dwDevType = didi.dwDevType;
		}
	}

	return hr;
}

template HRESULT m_IDirectInputDeviceX::EnumObjectsX<IDirectInputDevice8A, LPDIENUMDEVICEOBJECTSCALLBACKA, DIDEVICEOBJECTINSTANCEA, DIDEVICEOBJECTINSTANCE_DX3A>(LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD);
template HRESULT m_IDirectInputDeviceX::EnumObjectsX<IDirectInputDevice8W, LPDIENUMDEVICEOBJECTSCALLBACKW, DIDEVICEOBJECTINSTANCEW, DIDEVICEOBJECTINSTANCE_DX3W>(LPDIENUMDEVICEOBJECTSCALLBACKW, LPVOID, DWORD);
template <class T, class V, class D, class D_Old>
HRESULT m_IDirectInputDeviceX::EnumObjectsX(V lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!lpCallback)
	{
		return DIERR_INVALIDPARAM;
	}

	// EnumObjects of old DirectInput differs from DirectInput8 in three ways:
	// 1. When the data format is set, objects not specified in that format get an offset of -1 (all DInput versions).
	// 2. Objects are enumerated in order and with offsets that match a predefined data format, while DInput8 enumerates them in "raw data" order
	//    (all DInput versions except for 0x700 and 0x5B2).
	// 3. Collections are not enumerated, except for versions 0x5B2 and 0x700
	// Second element of the pair is the sorting order (used only if needed).
	using DeviceObjectList = std::list<std::pair<D, DWORD>>;

	// Callback structure
	struct ObjectEnumerator
	{
		DeviceObjectList objects;
		const EnumObjectDataMap* pObjectDataMap = nullptr;
		DWORD dwDefaultOffset = 0;
		V lpCallback = nullptr;
		LPVOID pvRef = nullptr;
		DWORD dwStructSize = sizeof(D);
		bool bOldDInputEnumerationBehaviour = false;

		static BOOL CALLBACK StoreObjectsCallback(const D* lpddoi, LPVOID pvRef)
		{
			ObjectEnumerator* self = static_cast<ObjectEnumerator*>(pvRef);

			// Old DInput does not enumerate collections without data
			if (self->bOldDInputEnumerationBehaviour && DIDFT_GETTYPE(lpddoi->dwType) == (DIDFT_COLLECTION|DIDFT_NODATA))
			{
				return DIENUM_CONTINUE;
			}

			auto& element = self->objects.emplace_back(*lpddoi, MAXDWORD);
			if (!self->pObjectDataMap->empty())
			{
				// For DInput versions that reorder elements, overwrite the offset unconditionally.
				// Otherwise, only overwrite it if it's -1 or not in the format at all.	
				auto it = FindByDITypeAndInstance(*self->pObjectDataMap, element.first.dwType);
				if (self->bOldDInputEnumerationBehaviour)
				{
					const ObjectOrderValue& objOrderValue = it != self->pObjectDataMap->end() ? it->second : ObjectOrderValue{self->dwDefaultOffset};
					element.first.dwOfs = objOrderValue.dwOfs;
					element.second = objOrderValue.dwSortOrder;
				}
				else
				{
					if (it != self->pObjectDataMap->end())
					{
						// If default offset is not 0, then it means we have set the data format
						if (self->dwDefaultOffset != 0)
						{
							element.first.dwOfs = it->second.dwOfs;
						}
					}
					else
					{
						element.first.dwOfs = self->dwDefaultOffset;
					}
				}
			}

			return DIENUM_CONTINUE;
		}

		static BOOL CALLBACK EnumObjectsCallback(const D* lpddoi, LPVOID pvRef)
		{
			const ObjectEnumerator* self = static_cast<ObjectEnumerator*>(pvRef);

			D DOI = {};
			CopyMemory(&DOI, lpddoi, min(lpddoi->dwSize, sizeof(D)));
			// Prevent DInput3 games from encountering a structure bigger than they might expect.
			DOI.dwSize = self->dwStructSize;

			return self->lpCallback(&DOI, self->pvRef);
		}

	} CallbackContext;
	CallbackContext.pvRef = pvRef;
	CallbackContext.lpCallback = lpCallback;
	CallbackContext.bOldDInputEnumerationBehaviour = diVersion < 0x700 && diVersion != 0x5B2;
	CallbackContext.pObjectDataMap = &EnumObjectDataLUT;
	CallbackContext.dwDefaultOffset = OffsetForMissingObjects;
	CallbackContext.dwStructSize = diVersion >= 0x500 ? sizeof(D) : sizeof(D_Old);

	HRESULT hr = GetProxyInterface<T>()->EnumObjects(ObjectEnumerator::StoreObjectsCallback, &CallbackContext, dwFlags);
	if (FAILED(hr))
	{
		return hr;
	}

	// Versions that aren't 0x5B2 and 0x700 need (stable) sorting by the sort order we prepared earlier
	if (diVersion < 0x700 && diVersion != 0x5B2)
	{
		CallbackContext.objects.sort([](const auto& left, const auto& right)
			{
				return left.second < right.second;
			});
	}

	for (const auto& object : CallbackContext.objects)
	{
		if (ObjectEnumerator::EnumObjectsCallback(&object.first, &CallbackContext) == DIENUM_STOP)
		{
			break;
		}
	}
	return DI_OK;
}

HRESULT m_IDirectInputDeviceX::GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDeviceX::SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	// Get rguidProp as a raw integer value, assuming it's not an actual GUID pointer
	const int rguidBytes = (const int)reinterpret_cast<const BYTE*>(&rguidProp);
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ") Property Identifier: " << rguidBytes;

	return ProxyInterface->SetProperty(rguidProp, pdiph);
}

HRESULT m_IDirectInputDeviceX::Acquire()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->Acquire();
}

HRESULT m_IDirectInputDeviceX::Unacquire()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->Unacquire();
}

HRESULT m_IDirectInputDeviceX::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	// Handle data format offset
	if (Offset)
	{
		UCHAR tmp[MAX_KEYBAORD] = {};

		HRESULT hr = ProxyInterface->GetDeviceState(MAX_KEYBAORD, tmp);

		if (SUCCEEDED(hr))
		{
			ZeroMemory(lpvData, cbData);

			for (DWORD x = Offset; x < min(cbData, Offset + MAX_KEYBAORD); x++)
			{
				((UCHAR*)lpvData)[x] = tmp[x - Offset];
			}
		}

		return hr;
	}

	return ProxyInterface->GetDeviceState(cbData, lpvData);
}

HRESULT m_IDirectInputDeviceX::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	//  If just peeking or clearing data
	if (dwFlags == DIGDD_PEEK || !rgdod || (pdwInOut && *pdwInOut == INFINITE))
	{
		return ProxyInterface->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, pdwInOut, dwFlags);
	}

	// Check for valid parameters
	if (!pdwInOut || !*pdwInOut || (cbObjectData != sizeof(DIDEVICEOBJECTDATA_DX3) && cbObjectData != sizeof(DIDEVICEOBJECTDATA)))
	{
		return DIERR_INVALIDPARAM;
	}

	EnterCriticalSection(&dics);

	// Check the size of the array
	if (*pdwInOut > pdod.size())
	{
		pdod.resize((*pdwInOut / 100 + 1) * 100);	// Increase buffer by factors of 100
	}

	HRESULT hr = ProxyInterface->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &pdod[0], pdwInOut, dwFlags);

	// Copy array
	if (SUCCEEDED(hr))
	{
		if (cbObjectData == sizeof(DIDEVICEOBJECTDATA_DX3))
		{
			CopyDeviceData((DIDEVICEOBJECTDATA_DX3*)rgdod, pdod.data(), *pdwInOut);
		}
		else
		{
			CopyDeviceData((DIDEVICEOBJECTDATA*)rgdod, pdod.data(), *pdwInOut);
		}
	}

	LeaveCriticalSection(&dics);

	return hr;
}

HRESULT m_IDirectInputDeviceX::SetDataFormat(LPCDIDATAFORMAT lpdf)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	// Fix unsupported flags
	if (lpdf && lpdf->dwNumObjs && lpdf->dwObjSize == sizeof(DIOBJECTDATAFORMAT))
	{
		// Handle data format offset
		if (lpdf->dwNumObjs < MAX_KEYBAORD && (lpdf->rgodf[0].dwType & DIDFT_BUTTON) && lpdf->rgodf[0].dwOfs != 0 && *lpdf->rgodf[0].pguid == GUID_Key)
		{
			Offset = lpdf->rgodf[0].dwOfs - 1;

			HRESULT hr = ProxyInterface->SetDataFormat(&c_dfDIKeyboard);
			if (SUCCEEDED(hr))
			{
				SetEnumObjectDataFromFormat(&c_dfDIKeyboard);
				return hr;
			}
		}

		std::vector<DIOBJECTDATAFORMAT> rgodf(lpdf->dwNumObjs);

		const DIDATAFORMAT df {
			sizeof(DIDATAFORMAT),
			sizeof(DIOBJECTDATAFORMAT),
			lpdf->dwFlags,
			lpdf->dwDataSize,
			lpdf->dwNumObjs,
			rgodf.data() };

		Logging::LogDebug() << __FUNCTION__ << " (" << this << ") "
			<< " Size: " << lpdf->dwSize
			<< " ObjSize: " << lpdf->dwObjSize
			<< " Flags: " << Logging::hex(lpdf->dwFlags)
			<< " DataSize: " << lpdf->dwDataSize
			<< " NumObjs: " << lpdf->dwNumObjs;

		for (DWORD x = 0; x < df.dwNumObjs; x++)
		{
			rgodf[x].pguid = lpdf->rgodf[x].pguid;
			rgodf[x].dwOfs = lpdf->rgodf[x].dwOfs;
			rgodf[x].dwType = ((lpdf->rgodf[x].dwType & DIDFT_ANYINSTANCE) == 0xFF00) ? lpdf->rgodf[x].dwType | DIDFT_ANYINSTANCE : lpdf->rgodf[x].dwType;
			rgodf[x].dwFlags = lpdf->rgodf[x].dwFlags;

			Logging::LogDebug() << __FUNCTION__ << " (" << this << ") -" << x << "-"
				<< " GUID: " << rgodf[x].pguid
				<< " Ofs: " << rgodf[x].dwOfs
				<< " Type: " << Logging::hex(rgodf[x].dwType)
				<< " Flags: " << Logging::hex(rgodf[x].dwFlags);
		}

		Offset = 0;

		HRESULT hr = ProxyInterface->SetDataFormat(&df);
		if (SUCCEEDED(hr))
		{
			SetEnumObjectDataFromFormat(&df);
		}
		return hr;
	}

	HRESULT hr = ProxyInterface->SetDataFormat(lpdf);
	if (SUCCEEDED(hr))
	{
		SetEnumObjectDataFromFormat(lpdf);
	}
	return hr;
}

HRESULT m_IDirectInputDeviceX::SetEventNotification(HANDLE hEvent)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->SetEventNotification(hEvent);
}

HRESULT m_IDirectInputDeviceX::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ") hwnd: " << hwnd << " dwFlags: " << Logging::hex(dwFlags);

	// If hwnd is null, assign the main application window as a fallback
	if (!hwnd && IsMouse)
	{
		hwnd = GetMainWindow();
		Logging::LogDebug() << __FUNCTION__ << " Warning: null hwnd, using: " << hwnd << (dwFlags & DISCL_FOREGROUND ? " Removing: DISCL_FOREGROUND" : "");
		dwFlags = (dwFlags | DISCL_BACKGROUND) & ~DISCL_FOREGROUND;
	}

	return ProxyInterface->SetCooperativeLevel(hwnd, dwFlags);
}

template HRESULT m_IDirectInputDeviceX::GetObjectInfoX<IDirectInputDevice8A, LPDIDEVICEOBJECTINSTANCEA>(LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD);
template HRESULT m_IDirectInputDeviceX::GetObjectInfoX<IDirectInputDevice8W, LPDIDEVICEOBJECTINSTANCEW>(LPDIDEVICEOBJECTINSTANCEW, DWORD, DWORD);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::GetObjectInfoX(V pdidoi, DWORD dwObj, DWORD dwHow)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	HRESULT hr = GetProxyInterface<T>()->GetObjectInfo(pdidoi, dwObj, dwHow);

	if (SUCCEEDED(hr) && pdidoi && pdidoi->dwSize)
	{
		if (!EnumObjectDataLUT.empty())
		{
			const bool bOldDInputEnumerationBehaviour = diVersion < 0x700 && diVersion != 0x5B2;

			// For DInput versions that reorder elements, overwrite the offset unconditionally.
			// Otherwise, only overwrite it if it's -1 or not in the format at all.	
			auto it = FindByDITypeAndInstance(EnumObjectDataLUT, pdidoi->dwType);
			if (bOldDInputEnumerationBehaviour)
			{
				pdidoi->dwOfs = it != EnumObjectDataLUT.end() ? it->second.dwOfs : OffsetForMissingObjects;
			}
			else
			{
				if (it != EnumObjectDataLUT.end())
				{
					// If default offset is not 0, then it means we have set the data format
					if (OffsetForMissingObjects != 0)
					{
						pdidoi->dwOfs = it->second.dwOfs;
					}
				}
				else
				{
					pdidoi->dwOfs = OffsetForMissingObjects;
				}
			}
		}
	}

	return hr;
}

template HRESULT m_IDirectInputDeviceX::GetDeviceInfoX<IDirectInputDevice8A, LPDIDEVICEINSTANCEA>(LPDIDEVICEINSTANCEA);
template HRESULT m_IDirectInputDeviceX::GetDeviceInfoX<IDirectInputDevice8W, LPDIDEVICEINSTANCEW>(LPDIDEVICEINSTANCEW);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::GetDeviceInfoX(V pdidi)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!pdidi || !pdidi->dwSize)
	{
		return DIERR_INVALIDPARAM;
	}

	HRESULT hr = GetProxyInterface<T>()->GetDeviceInfo(pdidi);

	if (SUCCEEDED(hr))
	{
		DWORD devType = GET_DIDEVICE_TYPE(pdidi->dwDevType);
		DWORD devSubType = GET_DIDEVICE_SUBTYPE(pdidi->dwDevType);
		DWORD hidDevice = pdidi->dwDevType & DIDEVTYPE_HID;
		BOOL IsGamepad = FALSE;
		DWORD devType7 = ConvertDevTypeTo7(devType, pdidi->wUsagePage, pdidi->wUsage, hidDevice, IsGamepad);
		DWORD devSubType7 = ConvertDevSubTypeTo7(devType, devType7, devSubType, IsGamepad);
		pdidi->dwDevType = devType7 | (devSubType7 << 8) | hidDevice;
	}

	return hr;
}

HRESULT m_IDirectInputDeviceX::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT m_IDirectInputDeviceX::Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

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

HRESULT m_IDirectInputDeviceX::CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT * ppdeff, LPUNKNOWN punkOuter)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ") Trying! " << rguid << " " << punkOuter;

	if (!ppdeff)
	{
		return DIERR_INVALIDPARAM;
	}
	*ppdeff = nullptr;

	DIEFFECT eff = {};
	if (lpeff && lpeff->dwSize == sizeof(DIEFFECT_DX5))
	{
		*(DIEFFECT_DX5*)&eff = *(DIEFFECT_DX5*)lpeff;
		eff.dwSize = sizeof(DIEFFECT);
		lpeff = &eff;
	}

	HRESULT hr = ProxyInterface->CreateEffect(rguid, lpeff, ppdeff, punkOuter);

	if (SUCCEEDED(hr))
	{
		m_IDirectInputEffect* pEffect = CreateEffectWrapper((IDirectInputEffect*)*ppdeff);
		pEffect->SetVersion(diVersion);

		*ppdeff = pEffect;
	}
	else
	{
		Logging::LogDebug() << __FUNCTION__ << " (" << this << ") Failed! hr: " << (DIERR)hr;

		m_IDirectInputEffect* pEffect = CreateEffectWrapper(nullptr);

		// Return an effect class even on failure becasue some games don't check for failure
		*ppdeff = pEffect;
	}

	return hr;
}

template HRESULT m_IDirectInputDeviceX::EnumEffectsX<IDirectInputDevice8A, LPDIENUMEFFECTSCALLBACKA>(LPDIENUMEFFECTSCALLBACKA, LPVOID, DWORD);
template HRESULT m_IDirectInputDeviceX::EnumEffectsX<IDirectInputDevice8W, LPDIENUMEFFECTSCALLBACKW>(LPDIENUMEFFECTSCALLBACKW, LPVOID, DWORD);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::EnumEffectsX(V lpCallback, LPVOID pvRef, DWORD dwEffType)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return GetProxyInterface<T>()->EnumEffects(lpCallback, pvRef, dwEffType);
}

template HRESULT m_IDirectInputDeviceX::GetEffectInfoX<IDirectInputDevice8A, LPDIEFFECTINFOA>(LPDIEFFECTINFOA, REFGUID);
template HRESULT m_IDirectInputDeviceX::GetEffectInfoX<IDirectInputDevice8W, LPDIEFFECTINFOW>(LPDIEFFECTINFOW, REFGUID);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::GetEffectInfoX(V pdei, REFGUID rguid)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return GetProxyInterface<T>()->GetEffectInfo(pdei, rguid);
}

HRESULT m_IDirectInputDeviceX::GetForceFeedbackState(LPDWORD pdwOut)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetForceFeedbackState(pdwOut);
}

HRESULT m_IDirectInputDeviceX::SendForceFeedbackCommand(DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->SendForceFeedbackCommand(dwFlags);
}

HRESULT m_IDirectInputDeviceX::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!lpCallback)
	{
		return DIERR_INVALIDPARAM;
	}

	// Callback structure
	struct EffectEnumerator
	{
		LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback = nullptr;
		LPVOID pvRef = nullptr;

		static BOOL CALLBACK EnumEffectCallback(LPDIRECTINPUTEFFECT pdeff, LPVOID pvRef)
		{
			EffectEnumerator* self = static_cast<EffectEnumerator*>(pvRef);

			if (pdeff)
			{
				pdeff = ProxyAddressLookupTable.FindAddress<m_IDirectInputEffect>(pdeff);
			}

			return self->lpCallback(pdeff, self->pvRef);
		}
	} CallbackContext;
	CallbackContext.pvRef = pvRef;
	CallbackContext.lpCallback = lpCallback;

	return ProxyInterface->EnumCreatedEffectObjects(EffectEnumerator::EnumEffectCallback, &CallbackContext, fl);
}

HRESULT m_IDirectInputDeviceX::Escape(LPDIEFFESCAPE pesc)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->Escape(pesc);
}

HRESULT m_IDirectInputDeviceX::Poll()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	HRESULT hr = ProxyInterface->Poll();

	return (hr == DI_NOEFFECT) ? DI_OK : hr;
}

HRESULT m_IDirectInputDeviceX::SendDeviceData(DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!pdwInOut || !rgdod || (cbObjectData != sizeof(DIDEVICEOBJECTDATA_DX3) && cbObjectData != sizeof(DIDEVICEOBJECTDATA)))
	{
		return DIERR_INVALIDPARAM;
	}

	EnterCriticalSection(&dics);

	// Check the size of the array
	if (*pdwInOut > pdod.size())
	{
		pdod.resize((*pdwInOut / 100 + 1) * 100);	// Increase buffer by factors of 100
	}

	// Copy array
	if (cbObjectData == sizeof(DIDEVICEOBJECTDATA_DX3))
	{
		CopyDeviceData(pdod.data(), (DIDEVICEOBJECTDATA_DX3*)rgdod, *pdwInOut);
	}
	else
	{
		CopyDeviceData(pdod.data(), (DIDEVICEOBJECTDATA*)rgdod, *pdwInOut);
	}

	HRESULT hr = ProxyInterface->SendDeviceData(sizeof(DIDEVICEOBJECTDATA), &pdod[0], pdwInOut, fl);

	LeaveCriticalSection(&dics);

	return hr;
}

template HRESULT m_IDirectInputDeviceX::EnumEffectsInFileX<IDirectInputDevice8A, LPCSTR>(LPCSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
template HRESULT m_IDirectInputDeviceX::EnumEffectsInFileX<IDirectInputDevice8W, LPCWSTR>(LPCWSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::EnumEffectsInFileX(V lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return GetProxyInterface<T>()->EnumEffectsInFile(lpszFileName, pec, pvRef, dwFlags);
}

template HRESULT m_IDirectInputDeviceX::WriteEffectToFileX<IDirectInputDevice8A, LPCSTR>(LPCSTR, DWORD, LPDIFILEEFFECT, DWORD);
template HRESULT m_IDirectInputDeviceX::WriteEffectToFileX<IDirectInputDevice8W, LPCWSTR>(LPCWSTR, DWORD, LPDIFILEEFFECT, DWORD);
template <class T, class V>
HRESULT m_IDirectInputDeviceX::WriteEffectToFileX(V lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return GetProxyInterface<T>()->WriteEffectToFile(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
}

void m_IDirectInputDeviceX::ReleaseDirectInput()
{
	// Don't delete wrapper interface
	if (StringType == ANSI_CHARSET)
	{
		SaveInterfaceAddress((m_IDirectInputDeviceA*&)WrapperInterface, DirectInputDeviceWrapperBackupA);
		SaveInterfaceAddress((m_IDirectInputDevice2A*&)WrapperInterface2, DirectInputDeviceWrapperBackup2A);
		SaveInterfaceAddress((m_IDirectInputDevice7A*&)WrapperInterface7, DirectInputDeviceWrapperBackup7A);
	}
	else
	{
		SaveInterfaceAddress((m_IDirectInputDeviceW*&)WrapperInterface, DirectInputDeviceWrapperBackupW);
		SaveInterfaceAddress((m_IDirectInputDevice2W*&)WrapperInterface2, DirectInputDeviceWrapperBackup2W);
		SaveInterfaceAddress((m_IDirectInputDevice7W*&)WrapperInterface7, DirectInputDeviceWrapperBackup7W);
	}
}
