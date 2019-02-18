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

		Logging::LogDebug() << "Creating device " << __FUNCTION__ << "(" << this << ")" << " converting device from v" << Version << " to v8 using " << ((StringType == DEFAULT_CHARSET) ? "UNICODE" : "ANSI");
	}
	~m_IDirectInputX()
	{
		Logging::LogDebug() << __FUNCTION__ << "(" << this << ")" << " deleting device!";
	}

	// Helper functions
	IDirectInput8W *GetProxyInterface() { return ProxyInterface; }
	IDirectInput8A *GetProxyInterface(LPDIENUMDEVICESCALLBACKA) { return (IDirectInput8A*)ProxyInterface; }
	IDirectInput8W *GetProxyInterface(LPDIENUMDEVICESCALLBACKW) { return ProxyInterface; }
	IDirectInput8A *GetProxyInterface(LPDIRECTINPUTDEVICE8A) { return (IDirectInput8A*)ProxyInterface; }
	IDirectInput8W *GetProxyInterface(LPDIRECTINPUTDEVICE8W) { return ProxyInterface; }
	IDirectInput8A *GetProxyInterface(LPCSTR) { return (IDirectInput8A*)ProxyInterface; }
	IDirectInput8W *GetProxyInterface(LPCWSTR) { return ProxyInterface; }
	m_IDirectInputX *GetWrapperInterface() { return this; }
	void IncRef() { InterlockedIncrement(&RefCount); }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInput2 methods ***/
	template <typename T>
	HRESULT EnumDevices(DWORD, T, LPVOID, DWORD);
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);
	template <typename T>
	HRESULT FindDevice(THIS_ REFGUID, T, LPGUID);

	/*** IDirectInput7 methods ***/
	template <typename T>
	HRESULT CreateDeviceEx(REFGUID rguid, REFIID riid, T *ppvObj, LPUNKNOWN pUnkOuter);
};
