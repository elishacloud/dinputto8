#pragma once

class m_IDirectInputX : public AddressLookupTableDinputObject
{
private:
	IDirectInput8W *ProxyInterface;
	REFIID WrapperID;
	DWORD StringType;

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
	inline IDirectInput8A *GetProxyInterfaceA() { return (IDirectInput8A*)ProxyInterface; }
	inline IDirectInput8W *GetProxyInterfaceW() { return ProxyInterface; }

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

		ProxyAddressLookupTable.SaveAddress(this, ProxyInterface);
	}
	~m_IDirectInputX()
	{
		LOG_LIMIT(3, __FUNCTION__ << "(" << this << ")" << " deleting interface!");
		
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

		ProxyAddressLookupTable.DeleteAddress(this);
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj, DWORD DirectXVersion);
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) { return QueryInterface(riid, ppvObj, GetGUIDVersion(riid)); }
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInput methods ***/
	STDMETHOD(EnumDevicesA)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
	STDMETHOD(EnumDevicesW)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);

	/*** IDirectInput2 methods ***/
	STDMETHOD(FindDeviceA)(THIS_ REFGUID, LPCSTR, LPGUID);
	STDMETHOD(FindDeviceW)(THIS_ REFGUID, LPCWSTR, LPGUID);

	/*** IDirectInput7 methods ***/
	STDMETHOD(CreateDeviceExA)(THIS_ REFGUID, REFIID, LPDIRECTINPUTDEVICE8A *, LPUNKNOWN);
	STDMETHOD(CreateDeviceExW)(THIS_ REFGUID, REFIID, LPDIRECTINPUTDEVICE8W *, LPUNKNOWN);

	// Helper functions
	LPVOID GetWrapperInterfaceX(DWORD DXVersion);
};
