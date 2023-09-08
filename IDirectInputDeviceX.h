#pragma once

class m_IDirectInputDeviceX : public AddressLookupTableDinputObject
{
private:
	IDirectInputDevice8W *ProxyInterface;
	REFIID WrapperID;
	DWORD StringType;

	// Requested DirectInput version - used to alter behaviour by requested version
	DWORD diVersion = 0;

	// Version Interfaces
	void *WrapperInterface;
	void *WrapperInterface2;
	void *WrapperInterface7;

	// For CooperativeLevel
	bool CanAquireDevice = false;

	// Format memory
	DWORD Offset = 0;

	// Critical section for shared memory
	CRITICAL_SECTION dics = {};

	// For DeviceData
	std::vector<DIDEVICEOBJECTDATA> pdod;

	// Wrapper interface functions
	inline REFIID GetWrapperType(DWORD DirectXVersion)
	{
		return (StringType == ANSI_CHARSET) ?
			((DirectXVersion == 1) ? IID_IDirectInputDeviceA :
			(DirectXVersion == 2) ? IID_IDirectInputDevice2A :
			(DirectXVersion == 7) ? IID_IDirectInputDevice7A : IID_IUnknown) :
			((DirectXVersion == 1) ? IID_IDirectInputDeviceW :
			(DirectXVersion == 2) ? IID_IDirectInputDevice2W :
			(DirectXVersion == 7) ? IID_IDirectInputDevice7W : IID_IUnknown);
	}
	inline bool CheckWrapperType(REFIID IID)
	{
		return (StringType == ANSI_CHARSET) ?
			((IID == IID_IDirectInputDeviceA ||
			IID == IID_IDirectInputDevice2A ||
			IID == IID_IDirectInputDevice7A) ? true : false) :
			((IID == IID_IDirectInputDeviceW ||
			IID == IID_IDirectInputDevice2W ||
			IID == IID_IDirectInputDevice7W) ? true : false);
	}
	template <class T>
	inline T *GetProxyInterface() { return (T*)ProxyInterface; }

	template <class T, class V>
	inline HRESULT EnumObjectsX(V lpCallback, LPVOID pvRef, DWORD dwFlags);

	template <class T, class V>
	inline HRESULT GetObjectInfoX(V pdidoi, DWORD dwObj, DWORD dwHow);

	template <class T, class V>
	inline HRESULT GetDeviceInfoX(V pdidi);

	template <class T, class V>
	inline HRESULT EnumEffectsX(V lpCallback, LPVOID pvRef, DWORD dwEffType);

	template <class T, class V>
	inline HRESULT GetEffectInfoX(V pdei, REFGUID rguid);

	template <class T, class V>
	inline HRESULT EnumEffectsInFileX(V lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags);

	template <class T, class V>
	inline HRESULT WriteEffectToFileX(V lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags);

public:
	m_IDirectInputDeviceX(IDirectInputDevice8W *aOriginal, REFIID riid) : ProxyInterface(aOriginal), WrapperID(riid), StringType(GetStringType(riid))
	{
		LOG_LIMIT(3, "Creating interface " << __FUNCTION__ << "(" << this << ")" << " converting interface from v" << GetGUIDVersion(riid) << " to v8 using " << ((StringType == ANSI_CHARSET) ? "ANSI" : "UNICODE"));

		if (StringType == ANSI_CHARSET)
		{
			WrapperInterface = new m_IDirectInputDeviceA((LPDIRECTINPUTDEVICEA)ProxyInterface, this);
			WrapperInterface2 = new m_IDirectInputDevice2A((LPDIRECTINPUTDEVICE2A)ProxyInterface, this);
			WrapperInterface7 = new m_IDirectInputDevice7A((LPDIRECTINPUTDEVICE7A)ProxyInterface, this);
		}
		else
		{
			WrapperInterface = new m_IDirectInputDeviceW((LPDIRECTINPUTDEVICEW)ProxyInterface, this);
			WrapperInterface2 = new m_IDirectInputDevice2W((LPDIRECTINPUTDEVICE2W)ProxyInterface, this);
			WrapperInterface7 = new m_IDirectInputDevice7W((LPDIRECTINPUTDEVICE7W)ProxyInterface, this);
		}

		// Initialize Critical Section
		InitializeCriticalSection(&dics);
	}
	~m_IDirectInputDeviceX()
	{
		LOG_LIMIT(3, __FUNCTION__ << " (" << this << ")" << " deleting interface!");

		if (StringType == ANSI_CHARSET)
		{
			((m_IDirectInputA*)WrapperInterface)->DeleteMe();
			((m_IDirectInput2A*)WrapperInterface2)->DeleteMe();
			((m_IDirectInput7A*)WrapperInterface7)->DeleteMe();
		}
		else
		{
			((m_IDirectInputW*)WrapperInterface)->DeleteMe();
			((m_IDirectInput2W*)WrapperInterface2)->DeleteMe();
			((m_IDirectInput7W*)WrapperInterface7)->DeleteMe();
		}

		// Delete Critical Section
		DeleteCriticalSection(&dics);
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj, DWORD DirectXVersion);
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) { return QueryInterface(riid, ppvObj, GetGUIDVersion(riid)); }
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInputDevice methods ***/
	STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS);
	STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		return EnumObjectsX<IDirectInputDevice8A, LPDIENUMDEVICEOBJECTSCALLBACKA>(lpCallback, pvRef, dwFlags);
	}
	STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		return EnumObjectsX<IDirectInputDevice8W, LPDIENUMDEVICEOBJECTSCALLBACKW>(lpCallback, pvRef, dwFlags);
	}
	STDMETHOD(GetProperty)(THIS_ REFGUID, LPDIPROPHEADER);
	STDMETHOD(SetProperty)(THIS_ REFGUID, LPCDIPROPHEADER);
	STDMETHOD(Acquire)(THIS);
	STDMETHOD(Unacquire)(THIS);
	STDMETHOD(GetDeviceState)(THIS_ DWORD, LPVOID);
	STDMETHOD(GetDeviceData)(THIS_ DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
	STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT);
	STDMETHOD(SetEventNotification)(THIS_ HANDLE);
	STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD);
	STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow)
	{
		return GetObjectInfoX<IDirectInputDevice8A, LPDIDEVICEOBJECTINSTANCEA>(pdidoi, dwObj, dwHow);
	}
	STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW pdidoi, DWORD dwObj, DWORD dwHow)
	{
		return GetObjectInfoX<IDirectInputDevice8W, LPDIDEVICEOBJECTINSTANCEW>(pdidoi, dwObj, dwHow);
	}
	STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA pdidi)
	{
		return GetDeviceInfoX<IDirectInputDevice8A, LPDIDEVICEINSTANCEA>(pdidi);
	}
	STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW pdidi)
	{
		return GetDeviceInfoX<IDirectInputDevice8W, LPDIDEVICEINSTANCEW>(pdidi);
	}
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD, REFGUID);

	/*** IDirectInputDevice2 methods ***/
	STDMETHOD(CreateEffect)(THIS_ REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN);
	STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType)
	{
		return EnumEffectsX<IDirectInputDevice8A, LPDIENUMEFFECTSCALLBACKA>(lpCallback, pvRef, dwEffType);
	}
	STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwEffType)
	{
		return EnumEffectsX<IDirectInputDevice8W, LPDIENUMEFFECTSCALLBACKW>(lpCallback, pvRef, dwEffType);
	}
	STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA pdei, REFGUID rguid)
	{
		return GetEffectInfoX<IDirectInputDevice8A, LPDIEFFECTINFOA>(pdei, rguid);
	}
	STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOW pdei, REFGUID rguid)
	{
		return GetEffectInfoX<IDirectInputDevice8W, LPDIEFFECTINFOW>(pdei, rguid);
	}
	STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD);
	STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD);
	STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD);
	STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE);
	STDMETHOD(Poll)(THIS);
	STDMETHOD(SendDeviceData)(THIS_ DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD);

	/*** IDirectInputDevice7 methods ***/
	STDMETHOD(EnumEffectsInFile)(THIS_ LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
	{
		return EnumEffectsInFileX<IDirectInputDevice8A, LPCSTR>(lpszFileName, pec, pvRef, dwFlags);
	}
	STDMETHOD(EnumEffectsInFile)(THIS_ LPCWSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
	{
		return EnumEffectsInFileX<IDirectInputDevice8W, LPCWSTR>(lpszFileName, pec, pvRef, dwFlags);
	}
	STDMETHOD(WriteEffectToFile)(THIS_ LPCSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
	{
		return WriteEffectToFileX<IDirectInputDevice8A, LPCSTR>(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
	}
	STDMETHOD(WriteEffectToFile)(THIS_ LPCWSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
	{
		return WriteEffectToFileX<IDirectInputDevice8W, LPCWSTR>(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
	}

	// Helper functions
	LPVOID GetWrapperInterfaceX(DWORD DXVersion);

	void SetVersion(DWORD dwVersion)
	{
		diVersion = dwVersion;
	}
};
