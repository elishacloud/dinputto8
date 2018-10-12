#pragma once

class m_IDirectInput7W : public IDirectInput7W, public AddressLookupTableDinputObject
{
private:
	std::unique_ptr<m_IDirectInputX> ProxyInterface;
	IDirectInput7W *RealInterface;
	REFIID WrapperID = IID_IDirectInput7W;

public:
	m_IDirectInput7W(IDirectInput7W *aOriginal) : RealInterface(aOriginal)
	{
		ProxyInterface = std::make_unique<m_IDirectInputX>((IDirectInput8W*)RealInterface, 7, WrapperID, (m_IDirectInput7W*)this);
		ProxyAddressLookupTable.SaveAddress(this, RealInterface);
	}
	~m_IDirectInput7W()
	{
		ProxyAddressLookupTable.DeleteAddress(this);
	}

	DWORD GetDirectXVersion() { return 7; }
	REFIID GetWrapperType() { return WrapperID; }
	IDirectInput7W *GetProxyInterface() { return RealInterface; }
	m_IDirectInputX *GetWrapperInterface() { return ProxyInterface.get(); }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInput2W methods ***/
	STDMETHOD(CreateDevice)(THIS_ REFGUID, LPDIRECTINPUTDEVICEW *, LPUNKNOWN);
	STDMETHOD(EnumDevices)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);
	STDMETHOD(FindDevice)(THIS_ REFGUID, LPCWSTR, LPGUID);

	/*** IDirectInput7W methods ***/
	STDMETHOD(CreateDeviceEx)(THIS_ REFGUID, REFIID, LPVOID *, LPUNKNOWN);
};
