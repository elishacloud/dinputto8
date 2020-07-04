#pragma once

class m_IDirectInputDevice7W : public IDirectInputDevice7W, public AddressLookupTableDinputObject
{
private:
	m_IDirectInputDeviceX *ProxyInterface;
	IDirectInputDevice7W *RealInterface;
	REFIID WrapperID = IID_IDirectInputDevice7W;
	const DWORD DirectXVersion = 7;

public:
	m_IDirectInputDevice7W(IDirectInputDevice7W *aOriginal, m_IDirectInputDeviceX *Interface) : RealInterface(aOriginal), ProxyInterface(Interface)
	{
		ProxyAddressLookupTable.SaveAddress(this, RealInterface);
	}
	~m_IDirectInputDevice7W()
	{
		ProxyAddressLookupTable.DeleteAddress(this);
	}

	DWORD GetDirectXVersion() { return 7; }
	REFIID GetWrapperType() { return WrapperID; }
	IDirectInputDevice7W *GetProxyInterface() { return RealInterface; }
	m_IDirectInputDeviceX *GetWrapperInterface() { return ProxyInterface; }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInputDeviceW methods ***/
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
	/*** IDirectInputDevice2W methods ***/
	STDMETHOD(CreateEffect)(THIS_ REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN);
	STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW, LPVOID, DWORD);
	STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOW, REFGUID);
	STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD);
	STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD);
	STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD);
	STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE);
	STDMETHOD(Poll)(THIS);
	STDMETHOD(SendDeviceData)(THIS_ DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD);
	/*** IDirectInputDevice7W methods ***/
	STDMETHOD(EnumEffectsInFile)(THIS_ LPCWSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
	STDMETHOD(WriteEffectToFile)(THIS_ LPCWSTR, DWORD, LPDIFILEEFFECT, DWORD);
};
