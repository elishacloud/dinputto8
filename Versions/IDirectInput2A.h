#pragma once

class m_IDirectInput2A : public IDirectInput2A, public AddressLookupTableDinputObject
{
private:
	m_IDirectInputX *ProxyInterface;
	IDirectInput2A *RealInterface;
	REFIID WrapperID = IID_IDirectInput2A;
	const DWORD DirectXVersion = 2;

public:
	m_IDirectInput2A(IDirectInput2A *aOriginal, m_IDirectInputX *Interface) : RealInterface(aOriginal), ProxyInterface(Interface)
	{
		ProxyAddressLookupTable.SaveAddress(this, RealInterface);
	}
	~m_IDirectInput2A()
	{
		ProxyAddressLookupTable.DeleteAddress(this);
	}

	DWORD GetDirectXVersion() { return 2; }
	REFIID GetWrapperType() { return WrapperID; }
	IDirectInput2A *GetProxyInterface() { return RealInterface; }
	m_IDirectInputX *GetWrapperInterface() { return ProxyInterface; }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInputA methods ***/
	STDMETHOD(CreateDevice)(THIS_ REFGUID, LPDIRECTINPUTDEVICEA *, LPUNKNOWN);
	STDMETHOD(EnumDevices)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);
	/*** IDirectInput2A methods ***/
	STDMETHOD(FindDevice)(THIS_ REFGUID, LPCSTR, LPGUID);
};
