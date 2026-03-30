#pragma once

m_IDirectInputEffect* CreateEffectWrapper(IDirectInputEffect* aOriginal);

class m_IDirectInputEffect final : public IDirectInputEffect, public AddressLookupTableDinputObject
{
private:
	IDirectInputEffect *ProxyInterface;
	m_IDirectInputEffect *WrapperInterface = this;
	const IID WrapperID = IID_IDirectInputEffect;

	// Requested DirectInput version - used to alter behaviour by requested version
	DWORD diVersion = 0;

public:
	m_IDirectInputEffect(IDirectInputEffect *aOriginal) : ProxyInterface(aOriginal)
	{
		LOG_LIMIT(3, "Creating interface " << __FUNCTION__ << " (" << this << ")");

		ProxyAddressLookupTable.SaveAddress(this, ProxyInterface);
	}
	~m_IDirectInputEffect()
	{
		LOG_LIMIT(3, __FUNCTION__ << " (" << this << ")" << " deleting interface!");

		ProxyAddressLookupTable.DeleteAddress(this);
	}

	void SetProxy(IDirectInputEffect* NewProxyInterface)
	{
		ProxyInterface = NewProxyInterface;
		if (NewProxyInterface)
		{
			ProxyAddressLookupTable.SaveAddress(this, ProxyInterface);
		}
		else
		{
			ProxyAddressLookupTable.DeleteAddress(this);
		}
	}

	/*** IUnknown methods ***/
	IFACEMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR * ppvObj) override;
	IFACEMETHOD_(ULONG, AddRef)(THIS) override;
	IFACEMETHOD_(ULONG, Release)(THIS) override;

	/*** IDirectInputEffect methods ***/
	IFACEMETHOD(Initialize)(THIS_ HINSTANCE, DWORD, REFGUID) override;
	IFACEMETHOD(GetEffectGuid)(THIS_ LPGUID) override;
	IFACEMETHOD(GetParameters)(THIS_ LPDIEFFECT, DWORD) override;
	IFACEMETHOD(SetParameters)(THIS_ LPCDIEFFECT, DWORD) override;
	IFACEMETHOD(Start)(THIS_ DWORD, DWORD) override;
	IFACEMETHOD(Stop)(THIS) override;
	IFACEMETHOD(GetEffectStatus)(THIS_ LPDWORD) override;
	IFACEMETHOD(Download)(THIS) override;
	IFACEMETHOD(Unload)(THIS) override;
	IFACEMETHOD(Escape)(THIS_ LPDIEFFESCAPE) override;

	// Helper functions
	void SetVersion(DWORD dwVersion)
	{
		diVersion = dwVersion;
	}
};
