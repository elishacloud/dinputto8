#pragma once

class m_IDirectInputX
{
private:
	IDirectInput8W *ProxyInterface;
	m_IDirectInput7W *WrapperInterface;
	DWORD DirectXVersion;
	REFIID WrapperID;
	DWORD StringType;

public:
	m_IDirectInputX(IDirectInput8W *aOriginal, DWORD Version, REFIID riid, m_IDirectInput7W *Interface) : ProxyInterface(aOriginal), DirectXVersion(Version), WrapperID(riid), WrapperInterface(Interface)
	{
		StringType = GetStringType(WrapperID);
	}
	~m_IDirectInputX() { }

	IDirectInput8W *GetProxyInterface() { return ProxyInterface; }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInput2 methods ***/
	template <typename T>
	HRESULT CreateDevice(REFGUID, T *, LPUNKNOWN);
	template <typename T>
	HRESULT EnumDevices(DWORD, T, LPVOID, DWORD);
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);
	template <typename T>
	HRESULT FindDevice(THIS_ REFGUID, T, LPGUID);

	/*** IDirectInput7 methods ***/
	STDMETHOD(CreateDeviceEx)(THIS_ REFGUID, REFIID, LPVOID *, LPUNKNOWN);
};
