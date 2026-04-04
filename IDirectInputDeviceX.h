#pragma once

#include <map>
#include <vector>

class m_IDirectInputDeviceX final : public IDirectInputDevice7A, public IDirectInputDevice7W,
		AddressLookupTableDinputObject<m_IDirectInputDeviceX>, ModuleObjectCount::CountedObject
{
public:
	// Factory traits
	static inline const CLSID wrapper_clsid = CLSID_DirectInputDevice;

	static inline const CLSID proxy_clsid = CLSID_DirectInputDevice8;
	static inline const IID proxy_iid = IID_IDirectInputDevice8W;
	using proxy_type = IDirectInputDevice8W;

private:
	proxy_type *ProxyInterface;
	IDirectInputDevice8A *ProxyInterfaceA; // Non-owning alias

	// Requested DirectInput version - used to alter behaviour by requested version
	DWORD diVersion = 0;

	// For SetCooperativeLevel
	bool IsMouse = false;

	// Format memory
	DWORD Offset = 0;

	// For data format fixups
	DWORD DevType7 = 0;

	// Critical section for shared memory
	CRITICAL_SECTION dics = {};

	// For DeviceData
	std::vector<DIDEVICEOBJECTDATA> pdod;

	// For SetDataFormat
	std::vector<DIOBJECTDATAFORMAT> rgodf;

	// For EnumObjects
	struct ObjectOrderValue
	{
		DWORD dwOfs = 0;
		DWORD dwSortOrder = MAXDWORD;
	};
	using EnumObjectDataMap = std::map<DWORD, ObjectOrderValue>;
	EnumObjectDataMap EnumObjectDataLUT;
	DWORD OffsetForMissingObjects = 0; // -1 after the data format is set

	void InitializeEnumObjectData();
	void SetEnumObjectDataFromFormat(LPCDIDATAFORMAT lpdf);

	template <class T>
	inline void CopyDeviceData(T* DestDod, DIDEVICEOBJECTDATA* SrcDod, DWORD dwNumRecords)
	{
		for (UINT x = 0; x < dwNumRecords; x++)
		{
			*DestDod = *(T*)SrcDod;
			SrcDod = (DIDEVICEOBJECTDATA*)((DWORD_PTR)SrcDod + sizeof(DIDEVICEOBJECTDATA));
			DestDod = (T*)((DWORD_PTR)DestDod + sizeof(T));
		}
	}
	inline void CopyDeviceData(DIDEVICEOBJECTDATA* DestDod, DIDEVICEOBJECTDATA_DX3* SrcDod, DWORD dwNumRecords)
	{
		for (UINT x = 0; x < dwNumRecords; x++)
		{
			*(DIDEVICEOBJECTDATA_DX3*)DestDod = *SrcDod;
			DestDod->uAppData = NULL;
			SrcDod = (DIDEVICEOBJECTDATA_DX3*)((DWORD_PTR)SrcDod + sizeof(DIDEVICEOBJECTDATA_DX3));
			DestDod = (DIDEVICEOBJECTDATA*)((DWORD_PTR)DestDod + sizeof(DIDEVICEOBJECTDATA));
		}
	}

	template <bool bUnicode, class V, class D, class D_Old>
	inline HRESULT EnumObjectsX(V lpCallback, LPVOID pvRef, DWORD dwFlags);

	template <bool bUnicode, class V>
	inline HRESULT GetObjectInfoX(V pdidoi, DWORD dwObj, DWORD dwHow);

	template <bool bUnicode, class V>
	inline HRESULT GetDeviceInfoX(V pdidi);

	template <bool bUnicode, class V>
	inline HRESULT EnumEffectsX(V lpCallback, LPVOID pvRef, DWORD dwEffType);

	template <bool bUnicode, class V>
	inline HRESULT GetEffectInfoX(V pdei, REFGUID rguid);

	template <bool bUnicode, class V>
	inline HRESULT EnumEffectsInFileX(V lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags);

	template <bool bUnicode, class V>
	inline HRESULT WriteEffectToFileX(V lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags);;

public:
	m_IDirectInputDeviceX(proxy_type *aOriginal)
		: AddressLookupTableDinputObject(aOriginal)
		, ProxyInterface(aOriginal)
	{
		LOG_LIMIT(3, "Creating interface " << __FUNCTION__ << "(" << this << ")");

		ProxyInterface->QueryInterface(IID_IDirectInputDevice8A, reinterpret_cast<LPVOID*>(&ProxyInterfaceA));

		// Initialize Critical Section
		InitializeCriticalSection(&dics);

		InitializeEnumObjectData();
	}
	~m_IDirectInputDeviceX()
	{
		LOG_LIMIT(3, __FUNCTION__ << " (" << this << ")" << " deleting interface!");

		// Delete Critical Section
		DeleteCriticalSection(&dics);
	}

	/*** IUnknown methods ***/
	IFACEMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) override;
	IFACEMETHOD_(ULONG, AddRef)(THIS) override;
	IFACEMETHOD_(ULONG, Release)(THIS) override;

	/*** IDirectInputDevice methods ***/
	IFACEMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) override;
	IFACEMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags) override
	{
		return EnumObjectsX<false, LPDIENUMDEVICEOBJECTSCALLBACKA, DIDEVICEOBJECTINSTANCEA, DIDEVICEOBJECTINSTANCE_DX3A>(lpCallback, pvRef, dwFlags);
	}
	IFACEMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) override
	{
		return EnumObjectsX<true, LPDIENUMDEVICEOBJECTSCALLBACKW, DIDEVICEOBJECTINSTANCEW, DIDEVICEOBJECTINSTANCE_DX3W>(lpCallback, pvRef, dwFlags);
	}
	IFACEMETHOD(GetProperty)(THIS_ REFGUID, LPDIPROPHEADER) override;
	IFACEMETHOD(SetProperty)(THIS_ REFGUID, LPCDIPROPHEADER) override;
	IFACEMETHOD(Acquire)(THIS) override;
	IFACEMETHOD(Unacquire)(THIS) override;
	IFACEMETHOD(GetDeviceState)(THIS_ DWORD, LPVOID) override;
	IFACEMETHOD(GetDeviceData)(THIS_ DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD) override;
	IFACEMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) override;
	IFACEMETHOD(SetEventNotification)(THIS_ HANDLE) override;
	IFACEMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD) override;
	IFACEMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow) override
	{
		return GetObjectInfoX<false, LPDIDEVICEOBJECTINSTANCEA>(pdidoi, dwObj, dwHow);
	}
	IFACEMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW pdidoi, DWORD dwObj, DWORD dwHow) override
	{
		return GetObjectInfoX<true, LPDIDEVICEOBJECTINSTANCEW>(pdidoi, dwObj, dwHow);
	}
	IFACEMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA pdidi) override
	{
		return GetDeviceInfoX<false, LPDIDEVICEINSTANCEA>(pdidi);
	}
	IFACEMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW pdidi) override
	{
		return GetDeviceInfoX<true, LPDIDEVICEINSTANCEW>(pdidi);
	}
	IFACEMETHOD(RunControlPanel)(THIS_ HWND, DWORD) override;
	IFACEMETHOD(Initialize)(THIS_ HINSTANCE, DWORD, REFGUID) override;

	/*** IDirectInputDevice2 methods ***/
	IFACEMETHOD(CreateEffect)(THIS_ REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN) override;
	IFACEMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType) override
	{
		return EnumEffectsX<false, LPDIENUMEFFECTSCALLBACKA>(lpCallback, pvRef, dwEffType);
	}
	IFACEMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwEffType) override
	{
		return EnumEffectsX<true, LPDIENUMEFFECTSCALLBACKW>(lpCallback, pvRef, dwEffType);
	}
	IFACEMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA pdei, REFGUID rguid) override
	{
		return GetEffectInfoX<false, LPDIEFFECTINFOA>(pdei, rguid);
	}
	IFACEMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOW pdei, REFGUID rguid) override
	{
		return GetEffectInfoX<true, LPDIEFFECTINFOW>(pdei, rguid);
	}
	IFACEMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) override;
	IFACEMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) override;
	IFACEMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD) override;
	IFACEMETHOD(Escape)(THIS_ LPDIEFFESCAPE) override;
	IFACEMETHOD(Poll)(THIS) override;
	IFACEMETHOD(SendDeviceData)(THIS_ DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD) override;

	/*** IDirectInputDevice7 methods ***/
	IFACEMETHOD(EnumEffectsInFile)(THIS_ LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags) override
	{
		return EnumEffectsInFileX<false, LPCSTR>(lpszFileName, pec, pvRef, dwFlags);
	}
	IFACEMETHOD(EnumEffectsInFile)(THIS_ LPCWSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags) override
	{
		return EnumEffectsInFileX<true, LPCWSTR>(lpszFileName, pec, pvRef, dwFlags);
	}
	IFACEMETHOD(WriteEffectToFile)(THIS_ LPCSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags) override
	{
		return WriteEffectToFileX<false, LPCSTR>(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
	}
	IFACEMETHOD(WriteEffectToFile)(THIS_ LPCWSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags) override
	{
		return WriteEffectToFileX<true, LPCWSTR>(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
	}

	// Helper functions
	void SetVersion(DWORD dwVersion) { diVersion = dwVersion; }
	void SetAsMouse() { IsMouse = true; }
};
