#pragma once

class m_IDirectInputX
{
private:
	IDirectInput8W *ProxyInterface;
	m_IDirectInput7W *WrapperInterface;
	DWORD DirectXVersion;
	REFIID WrapperID;
	DWORD StringType;
	ULONG RefCount = 1;

public:
	m_IDirectInputX(IDirectInput8W *aOriginal, DWORD Version, REFIID riid, m_IDirectInput7W *Interface) : ProxyInterface(aOriginal), DirectXVersion(Version), WrapperID(riid), WrapperInterface(Interface)
	{
		StringType = GetStringType(WrapperID);

		LOG_LIMIT(3, "Creating device " << __FUNCTION__ << "(" << this << ")" << " converting device from v" << Version << " to v8 using " << ((StringType == DEFAULT_CHARSET) ? "UNICODE" : "ANSI"));
	}
	~m_IDirectInputX()
	{
		LOG_LIMIT(3, __FUNCTION__ << "(" << this << ")" << " deleting device!");
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInput methods ***/
	STDMETHOD(EnumDevicesA)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
	STDMETHOD(EnumDevicesW)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);

	/*** IDirectInput2 methods ***/
	STDMETHOD(FindDeviceA)(THIS_ REFGUID, LPCSTR, LPGUID);
	STDMETHOD(FindDeviceW)(THIS_ REFGUID, LPCWSTR, LPGUID);

	/*** IDirectInput7 methods ***/
	STDMETHOD(CreateDeviceExA)(THIS_ REFGUID, REFIID, LPDIRECTINPUTDEVICE8A *, LPUNKNOWN);
	STDMETHOD(CreateDeviceExW)(THIS_ REFGUID, REFIID, LPDIRECTINPUTDEVICE8W *, LPUNKNOWN);

	// Helper functions
	void IncRef() { InterlockedIncrement(&RefCount); }
	IDirectInput8A *GetProxyInterfaceA() { return (IDirectInput8A*)ProxyInterface; }
	IDirectInput8W *GetProxyInterfaceW() { return ProxyInterface; }
};
