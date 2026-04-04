#pragma once

#include <new>

class ClassFactoryBase : public IClassFactory, ModuleObjectCount::CountedObject
{
public:
	/*** IUnknown methods ***/
	IFACEMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) override;
	IFACEMETHOD_(ULONG, AddRef)(THIS) override;
	IFACEMETHOD_(ULONG, Release)(THIS) override;

	/*** IClassFactory methods ***/
	IFACEMETHOD(LockServer)(THIS_ BOOL fLock) override;

protected:
	virtual ~ClassFactoryBase() = default;
	explicit ClassFactoryBase(IClassFactory* dinput8Factory)
		: m_dinput8Factory(dinput8Factory)
	{
	}

protected:
	IClassFactory* m_dinput8Factory = nullptr;

private:
	LONG m_refCount = 1;
};

template<typename WrapperClass>
class ClassFactory final : public ClassFactoryBase
{
public:
	explicit ClassFactory(IClassFactory* dinput8Factory)
		: ClassFactoryBase(dinput8Factory)
	{
	}

	/*** IClassFactory methods ***/
    IFACEMETHOD(CreateInstance)(THIS_ IUnknown *pUnkOuter, REFIID riid, void **ppvObject) override
	{
		if (ppvObject == nullptr)
		{
			return E_POINTER;
		}

		HRESULT hr = E_OUTOFMEMORY;
		*ppvObject = nullptr;

		typename WrapperClass::proxy_type* proxyObject; 
		HRESULT proxyHr = m_dinput8Factory->CreateInstance(pUnkOuter, WrapperClass::proxy_iid, reinterpret_cast<void**>(&proxyObject));
		if (FAILED(proxyHr))
		{
			return proxyHr;
		}

		WrapperClass* wrapper = new (std::nothrow) WrapperClass(proxyObject);
		if (wrapper != nullptr)
		{
			hr = wrapper->QueryInterface(riid, ppvObject);
			wrapper->Release();
		}
		return hr;
	}
};
