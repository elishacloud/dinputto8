#pragma once

class m_IDirectInputDeviceX
{
private:
	IDirectInputDevice8W *ProxyInterface;
	m_IDirectInputDevice7W *WrapperInterface;
	DWORD DirectXVersion;
	REFIID WrapperID;
	DWORD StringType;

public:
	m_IDirectInputDeviceX(IDirectInputDevice8W *aOriginal, DWORD Version, REFIID riid, m_IDirectInputDevice7W *Interface) : ProxyInterface(aOriginal), DirectXVersion(Version), WrapperID(riid), WrapperInterface(Interface)
	{
		StringType = GetStringType(WrapperID);
	}
	~m_IDirectInputDeviceX() { }

	IDirectInputDevice8W *GetProxyInterface() { return ProxyInterface; }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInputDevice2 methods ***/
	STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS);
	template <typename T>
	HRESULT EnumObjects(T, LPVOID, DWORD);
	STDMETHOD(GetProperty)(THIS_ REFGUID, LPDIPROPHEADER);
	STDMETHOD(SetProperty)(THIS_ REFGUID, LPCDIPROPHEADER);
	STDMETHOD(Acquire)(THIS);
	STDMETHOD(Unacquire)(THIS);
	STDMETHOD(GetDeviceState)(THIS_ DWORD, LPVOID);
	STDMETHOD(GetDeviceData)(THIS_ DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
	STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT);
	STDMETHOD(SetEventNotification)(THIS_ HANDLE);
	STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD);
	template <typename T>
	HRESULT GetObjectInfo(T, DWORD, DWORD);
	template <typename T>
	HRESULT GetDeviceInfo(T);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD, REFGUID);
	STDMETHOD(CreateEffect)(THIS_ REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN);
	template <typename T>
	HRESULT EnumEffects(T, LPVOID, DWORD);
	template <typename T>
	HRESULT GetEffectInfo(T, REFGUID);
	STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD);
	STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD);
	STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD);
	STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE);
	STDMETHOD(Poll)(THIS);
	STDMETHOD(SendDeviceData)(THIS_ DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD);

	/*** IDirectInputDevice7 methods ***/
	template <typename T>
	HRESULT EnumEffectsInFile(T, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
	template <typename T>
	HRESULT WriteEffectToFile(T, DWORD, LPDIFILEEFFECT, DWORD);
};
