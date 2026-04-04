#pragma once

class m_IDirectInputX final : public IDirectInput7A, public IDirectInput7W, AddressLookupTableDinputObject<m_IDirectInputX>, ModuleObjectCount::CountedObject
{
public:
	// Factory traits
	static inline const CLSID wrapper_clsid = CLSID_DirectInput;

	static inline const CLSID proxy_clsid = CLSID_DirectInput8;
	static inline const IID proxy_iid = IID_IDirectInput8W;
	using proxy_type = IDirectInput8W;

private:
	proxy_type *ProxyInterface;
	IDirectInput8A *ProxyInterfaceA; // Non-owning alias

	// Requested DirectInput version - used to alter behaviour by requested version
	DWORD diVersion = 0;

	template <bool bUnicode, class V, class D, class D_Old>
	inline HRESULT EnumDevicesX(DWORD, V, LPVOID, DWORD);

	template <bool bUnicode, class V>
	inline HRESULT FindDeviceX(REFGUID rguidClass, V ptszName, LPGUID pguidInstance);

public:
	m_IDirectInputX(proxy_type *aOriginal)
		: AddressLookupTableDinputObject(aOriginal)
		, ProxyInterface(aOriginal)
	{
		LOG_LIMIT(3, "Creating interface " << __FUNCTION__ << "(" << this << ")");

		ProxyInterface->QueryInterface(IID_IDirectInput8A, reinterpret_cast<LPVOID*>(&ProxyInterfaceA));
	}
	~m_IDirectInputX()
	{
		LOG_LIMIT(3, __FUNCTION__ << " (" << this << ")" << " deleting interface!");
	}

	/*** IUnknown methods ***/
	IFACEMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) override;
	IFACEMETHOD_(ULONG, AddRef)(THIS) override;
	IFACEMETHOD_(ULONG, Release)(THIS) override;

	/*** IDirectInput methods ***/
	IFACEMETHOD(CreateDevice)(THIS_ REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpDirectInputDevice, LPUNKNOWN pUnkOuter) override
	{
		return CreateDeviceEx(rguid, IID_IDirectInputDeviceA, reinterpret_cast<LPVOID*>(lplpDirectInputDevice), pUnkOuter);
	}
	IFACEMETHOD(CreateDevice)(THIS_ REFGUID rguid, LPDIRECTINPUTDEVICEW *lplpDirectInputDevice, LPUNKNOWN pUnkOuter) override
	{
		return CreateDeviceEx(rguid, IID_IDirectInputDeviceW, reinterpret_cast<LPVOID*>(lplpDirectInputDevice), pUnkOuter);
	}

	IFACEMETHOD(EnumDevices)(THIS_ DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags) override
	{
		return EnumDevicesX<false, LPDIENUMDEVICESCALLBACKA, DIDEVICEINSTANCEA, DIDEVICEINSTANCE_DX3A>(dwDevType, lpCallback, pvRef, dwFlags);
	}
	IFACEMETHOD(EnumDevices)(THIS_ DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) override
	{
		return EnumDevicesX<true, LPDIENUMDEVICESCALLBACKW, DIDEVICEINSTANCEW, DIDEVICEINSTANCE_DX3W>(dwDevType, lpCallback, pvRef, dwFlags);
	}
	IFACEMETHOD(GetDeviceStatus)(THIS_ REFGUID) override;
	IFACEMETHOD(RunControlPanel)(THIS_ HWND, DWORD) override;
	IFACEMETHOD(Initialize)(THIS_ HINSTANCE, DWORD) override;

	/*** IDirectInput2 methods ***/
	IFACEMETHOD(FindDevice)(THIS_ REFGUID rguidClass, LPCSTR ptszName, LPGUID pguidInstance) override
	{
		return FindDeviceX<false, LPCSTR>(rguidClass, ptszName, pguidInstance);
	}
	IFACEMETHOD(FindDevice)(THIS_ REFGUID rguidClass, LPCWSTR ptszName, LPGUID pguidInstance) override
	{
		return FindDeviceX<true, LPCWSTR>(rguidClass, ptszName, pguidInstance);
	}

	/*** IDirectInput7 methods ***/
	IFACEMETHOD(CreateDeviceEx)(THIS_ REFGUID rguid, REFIID riid, LPVOID *ppvObj, LPUNKNOWN pUnkOuter) override;

	// Helper functions
	void SetVersion(DWORD dwVersion)
	{
		diVersion = dwVersion;
	}
};
