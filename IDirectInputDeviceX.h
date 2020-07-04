#pragma once

class m_IDirectInputDeviceX : public AddressLookupTableDinputObject
{
private:
	IDirectInputDevice8W *ProxyInterface;
	REFIID WrapperID;
	DWORD StringType;

	// Version Interfaces
	void *WrapperInterface;
	void *WrapperInterface2;
	void *WrapperInterface7;

	// For CooperativeLevel
	bool CanAquireDevice = false;

	// Format memory
	DWORD Offset = 0;

	// Critical section for shared memory
	CRITICAL_SECTION dics;

	// For DeviceData
	std::vector<DIDEVICEOBJECTDATA> pdod;

	// For DataFormat
	DIDATAFORMAT df;
	std::vector<DIOBJECTDATAFORMAT> rgodf;

public:
	m_IDirectInputDeviceX(IDirectInputDevice8W *aOriginal, REFIID riid) : ProxyInterface(aOriginal), WrapperID(riid), StringType(GetStringType(riid))
	{
		LOG_LIMIT(3, "Creating device " << __FUNCTION__ << "(" << this << ")" << " converting device from v" << dinputto8::GetGUIDVersion(riid) << " to v8 using " << ((StringType == ANSI_CHARSET) ? "ANSI" : "UNICODE"));

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

		ProxyAddressLookupTable.SaveAddress(this, ProxyInterface);
	}
	~m_IDirectInputDeviceX()
	{
		LOG_LIMIT(3, __FUNCTION__ << "(" << this << ")" << " deleting device!");

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

		ProxyAddressLookupTable.DeleteAddress(this);
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInputDevice methods ***/
	STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS);
	STDMETHOD(EnumObjectsA)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD);
	STDMETHOD(EnumObjectsW)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW, LPVOID, DWORD);
	STDMETHOD(GetProperty)(THIS_ REFGUID, LPDIPROPHEADER);
	STDMETHOD(SetProperty)(THIS_ REFGUID, LPCDIPROPHEADER);
	STDMETHOD(Acquire)(THIS);
	STDMETHOD(Unacquire)(THIS);
	STDMETHOD(GetDeviceState)(THIS_ DWORD, LPVOID);
	STDMETHOD(GetDeviceData)(THIS_ DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
	STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT);
	STDMETHOD(SetEventNotification)(THIS_ HANDLE);
	STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD);
	STDMETHOD(GetObjectInfoA)(THIS_ LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD);
	STDMETHOD(GetObjectInfoW)(THIS_ LPDIDEVICEOBJECTINSTANCEW, DWORD, DWORD);
	STDMETHOD(GetDeviceInfoA)(THIS_ LPDIDEVICEINSTANCEA);
	STDMETHOD(GetDeviceInfoW)(THIS_ LPDIDEVICEINSTANCEW);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD, REFGUID);

	/*** IDirectInputDevice2 methods ***/
	STDMETHOD(CreateEffect)(THIS_ REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN);
	STDMETHOD(EnumEffectsA)(THIS_ LPDIENUMEFFECTSCALLBACKA, LPVOID, DWORD);
	STDMETHOD(EnumEffectsW)(THIS_ LPDIENUMEFFECTSCALLBACKW, LPVOID, DWORD);
	STDMETHOD(GetEffectInfoA)(THIS_ LPDIEFFECTINFOA, REFGUID);
	STDMETHOD(GetEffectInfoW)(THIS_ LPDIEFFECTINFOW, REFGUID);
	STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD);
	STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD);
	STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD);
	STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE);
	STDMETHOD(Poll)(THIS);
	STDMETHOD(SendDeviceData)(THIS_ DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD);

	/*** IDirectInputDevice7 methods ***/
	STDMETHOD(EnumEffectsInFileA)(THIS_ LPCSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
	STDMETHOD(EnumEffectsInFileW)(THIS_ LPCWSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
	STDMETHOD(WriteEffectToFileA)(THIS_ LPCSTR, DWORD, LPDIFILEEFFECT, DWORD);
	STDMETHOD(WriteEffectToFileW)(THIS_ LPCWSTR, DWORD, LPDIFILEEFFECT, DWORD);

	// Helper functions
	LPVOID GetWrapperInterface(DWORD DXVersion);
	IDirectInputDevice8A *GetProxyInterfaceA() { return (IDirectInputDevice8A*)ProxyInterface; }
	IDirectInputDevice8W *GetProxyInterfaceW() { return ProxyInterface; }
};
