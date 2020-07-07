#pragma once

class m_IDirectInputA : public IDirectInputA, public AddressLookupTableDinputObject
{
private:
	m_IDirectInputX *ProxyInterface;
	IDirectInputA *RealInterface;
	REFIID WrapperID = IID_IDirectInputA;
	const DWORD DirectXVersion = 1;

public:
	m_IDirectInputA(IDirectInputA *aOriginal, m_IDirectInputX *Interface) : RealInterface(aOriginal), ProxyInterface(Interface)
	{
		ProxyAddressLookupTable.SaveAddress(this, RealInterface);
	}
	~m_IDirectInputA()
	{
		ProxyAddressLookupTable.DeleteAddress(this);
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInput methods ***/
	STDMETHOD(CreateDevice)(THIS_ REFGUID, LPDIRECTINPUTDEVICEA *, LPUNKNOWN);
	STDMETHOD(EnumDevices)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);
};
