#pragma once

class m_IDirectInputDeviceW : public IDirectInputDeviceW, public AddressLookupTableDinputObject
{
private:
	m_IDirectInputDeviceX *ProxyInterface;
	IDirectInputDeviceW *RealInterface;
	REFIID WrapperID = IID_IDirectInputDeviceW;
	const DWORD DirectXVersion = 1;

public:
	m_IDirectInputDeviceW(IDirectInputDeviceW *aOriginal, m_IDirectInputDeviceX *Interface) : RealInterface(aOriginal), ProxyInterface(Interface)
	{
		ProxyAddressLookupTable.SaveAddress(this, RealInterface);
	}
	~m_IDirectInputDeviceW()
	{
		ProxyAddressLookupTable.DeleteAddress(this);
	}

	DWORD GetDirectXVersion() { return 1; }
	REFIID GetWrapperType() { return WrapperID; }
	IDirectInputDeviceW *GetProxyInterface() { return RealInterface; }
	m_IDirectInputDeviceX *GetWrapperInterface() { return ProxyInterface; }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInputDevice methods ***/
	STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS);
	STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW, LPVOID, DWORD);
	STDMETHOD(GetProperty)(THIS_ REFGUID, LPDIPROPHEADER);
	STDMETHOD(SetProperty)(THIS_ REFGUID, LPCDIPROPHEADER);
	STDMETHOD(Acquire)(THIS);
	STDMETHOD(Unacquire)(THIS);
	STDMETHOD(GetDeviceState)(THIS_ DWORD, LPVOID);
	STDMETHOD(GetDeviceData)(THIS_ DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
	STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT);
	STDMETHOD(SetEventNotification)(THIS_ HANDLE);
	STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD);
	STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW, DWORD, DWORD);
	STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD, REFGUID);
};
