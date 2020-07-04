#pragma once

class m_IDirectInput7W : public IDirectInput7W, public AddressLookupTableDinputObject
{
private:
	m_IDirectInputX *ProxyInterface;
	IDirectInput7W *RealInterface;
	REFIID WrapperID = IID_IDirectInput7W;
	const DWORD DirectXVersion = 7;

public:
	m_IDirectInput7W(IDirectInput7W *aOriginal, m_IDirectInputX *Interface) : RealInterface(aOriginal), ProxyInterface(Interface)
	{
		ProxyAddressLookupTable.SaveAddress(this, RealInterface);
	}
	~m_IDirectInput7W()
	{
		ProxyAddressLookupTable.DeleteAddress(this);
	}

	DWORD GetDirectXVersion() { return 7; }
	REFIID GetWrapperType() { return WrapperID; }
	IDirectInput7W *GetProxyInterface() { return RealInterface; }
	m_IDirectInputX *GetWrapperInterface() { return ProxyInterface; }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInputW methods ***/
	STDMETHOD(CreateDevice)(THIS_ REFGUID, LPDIRECTINPUTDEVICEW *, LPUNKNOWN);
	STDMETHOD(EnumDevices)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);
	/*** IDirectInput2W methods ***/
	STDMETHOD(FindDevice)(THIS_ REFGUID, LPCWSTR, LPGUID);
	/*** IDirectInput7W methods ***/
	STDMETHOD(CreateDeviceEx)(THIS_ REFGUID, REFIID, LPVOID *, LPUNKNOWN);
};
