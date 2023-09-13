#pragma once

class m_IDirectInputX : public AddressLookupTableDinputObject
{
private:
	IDirectInput8W *ProxyInterface;
	REFIID WrapperID;
	DWORD StringType;

	// Requested DirectInput version - used to alter behaviour by requested version
	DWORD diVersion = 0;

	// Version Interfaces
	void *WrapperInterface;
	void *WrapperInterface2;
	void *WrapperInterface7;

	// Wrapper interface functions
	inline REFIID GetWrapperType(DWORD DirectXVersion)
	{
		return (StringType == ANSI_CHARSET) ?
			((DirectXVersion == 1) ? IID_IDirectInputA :
			(DirectXVersion == 2) ? IID_IDirectInput2A :
			(DirectXVersion == 7) ? IID_IDirectInput7A : IID_IUnknown) :
			((DirectXVersion == 1) ? IID_IDirectInputW :
			(DirectXVersion == 2) ? IID_IDirectInput2W :
			(DirectXVersion == 7) ? IID_IDirectInput7W : IID_IUnknown);
	}
	inline bool CheckWrapperType(REFIID IID)
	{
		return (StringType == ANSI_CHARSET) ?
			((IID == IID_IDirectInputA ||
			IID == IID_IDirectInput2A ||
			IID == IID_IDirectInput7A) ? true : false) :
			((IID == IID_IDirectInputW ||
			IID == IID_IDirectInput2W ||
			IID == IID_IDirectInput7W) ? true : false);
	}
	template <class T>
	inline T *GetProxyInterface() { return (T*)ProxyInterface; }

	template <class T, class V, class D, class D_Old>
	inline HRESULT EnumDevicesX(DWORD, V, LPVOID, DWORD);

	template <class T, class V>
	inline HRESULT FindDeviceX(REFGUID rguidClass, V ptszName, LPGUID pguidInstance);

	template <class T, class V>
	inline HRESULT CreateDeviceExX(REFGUID rguid, REFIID riid, V *ppvObj, LPUNKNOWN pUnkOuter);

public:
	m_IDirectInputX(IDirectInput8W *aOriginal, REFIID riid) : ProxyInterface(aOriginal), WrapperID(riid), StringType(GetStringType(riid))
	{
		LOG_LIMIT(3, "Creating interface " << __FUNCTION__ << "(" << this << ")" << " converting interface from v" << GetGUIDVersion(riid) << " to v8 using " << ((StringType == ANSI_CHARSET) ? "ANSI" : "UNICODE"));

		if (StringType == ANSI_CHARSET)
		{
			WrapperInterface = new m_IDirectInputA((LPDIRECTINPUTA)ProxyInterface, this);
			WrapperInterface2 = new m_IDirectInput2A((LPDIRECTINPUT2A)ProxyInterface, this);
			WrapperInterface7 = new m_IDirectInput7A((LPDIRECTINPUT7A)ProxyInterface, this);
		}
		else
		{
			WrapperInterface = new m_IDirectInputW((LPDIRECTINPUTW)ProxyInterface, this);
			WrapperInterface2 = new m_IDirectInput2W((LPDIRECTINPUT2W)ProxyInterface, this);
			WrapperInterface7 = new m_IDirectInput7W((LPDIRECTINPUT7W)ProxyInterface, this);
		}
	}
	~m_IDirectInputX()
	{
		LOG_LIMIT(3, __FUNCTION__ << " (" << this << ")" << " deleting interface!");
		
		if (StringType == ANSI_CHARSET)
		{
			((m_IDirectInputA*)WrapperInterface)->DeleteMe();
			((m_IDirectInput2A*)WrapperInterface2)->DeleteMe();
			((m_IDirectInput7A*)WrapperInterface7)->DeleteMe();
		}
		else
		{
			((m_IDirectInputW*)WrapperInterface)->DeleteMe();
			((m_IDirectInput2W*)WrapperInterface2)->DeleteMe();
			((m_IDirectInput7W*)WrapperInterface7)->DeleteMe();
		}
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj, DWORD DirectXVersion);
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) { return QueryInterface(riid, ppvObj, GetGUIDVersion(riid)); }
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInput methods ***/
	STDMETHOD(EnumDevices)(THIS_ DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		return EnumDevicesX<IDirectInput8A, LPDIENUMDEVICESCALLBACKA, DIDEVICEINSTANCEA, DIDEVICEINSTANCE_DX3A>(dwDevType, lpCallback, pvRef, dwFlags);
	}
	STDMETHOD(EnumDevices)(THIS_ DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		return EnumDevicesX<IDirectInput8W, LPDIENUMDEVICESCALLBACKW, DIDEVICEINSTANCEW, DIDEVICEINSTANCE_DX3W>(dwDevType, lpCallback, pvRef, dwFlags);
	}
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);

	/*** IDirectInput2 methods ***/
	STDMETHOD(FindDevice)(THIS_ REFGUID rguidClass, LPCSTR ptszName, LPGUID pguidInstance)
	{
		return FindDeviceX<IDirectInput8A, LPCSTR>(rguidClass, ptszName, pguidInstance);
	}
	STDMETHOD(FindDevice)(THIS_ REFGUID rguidClass, LPCWSTR ptszName, LPGUID pguidInstance)
	{
		return FindDeviceX<IDirectInput8W, LPCWSTR>(rguidClass, ptszName, pguidInstance);
	}

	/*** IDirectInput7 methods ***/
	STDMETHOD(CreateDeviceEx)(THIS_ REFGUID rguid, REFIID riid, LPDIRECTINPUTDEVICE8A *ppvObj, LPUNKNOWN pUnkOuter)
	{
		return CreateDeviceExX<IDirectInput8A, LPDIRECTINPUTDEVICE8A>(rguid, riid, ppvObj, pUnkOuter);
	}
	STDMETHOD(CreateDeviceEx)(THIS_ REFGUID rguid, REFIID riid, LPDIRECTINPUTDEVICE8W *ppvObj, LPUNKNOWN pUnkOuter)
	{
		return CreateDeviceExX<IDirectInput8W, LPDIRECTINPUTDEVICE8W>(rguid, riid, ppvObj, pUnkOuter);
	}

	// Helper functions
	LPVOID GetWrapperInterfaceX(DWORD DXVersion);

	void SetVersion(DWORD dwVersion)
	{
		diVersion = dwVersion;
	}
};
