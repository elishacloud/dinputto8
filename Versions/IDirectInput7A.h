#pragma once

class m_IDirectInput7A : public IDirectInput7A, public AddressLookupTableDinputObject
{
private:
	m_IDirectInputX *ProxyInterface;
	IDirectInput7A *RealInterface;
	REFIID WrapperID = IID_IDirectInput7A;
	const DWORD DirectXVersion = 7;

public:
	m_IDirectInput7A(IDirectInput7A *aOriginal, m_IDirectInputX *Interface) : RealInterface(aOriginal), ProxyInterface(Interface)
	{
		ProxyAddressLookupTable.SaveAddress(this, RealInterface);
	}
	~m_IDirectInput7A()
	{
		ProxyAddressLookupTable.DeleteAddress(this);
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR * ppvObj);
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
	/*** IDirectInput7A methods ***/
	STDMETHOD(CreateDeviceEx)(THIS_ REFGUID, REFIID, LPVOID *, LPUNKNOWN);
};
