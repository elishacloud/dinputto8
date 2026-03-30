#pragma once

#include <unordered_map>
#include <algorithm>
#include "dinputto8.h"

constexpr UINT MaxIndex = 4;

template <typename T>
inline void SaveInterfaceAddress(T*& Interface, T*& InterfaceBackup)
{
	if (Interface)
	{
		SetCriticalSection();
		Interface->SetProxy(nullptr);
		if (InterfaceBackup)
		{
			InterfaceBackup->DeleteMe();
			InterfaceBackup = nullptr;
		}
		InterfaceBackup = Interface;
		ReleaseCriticalSection();
	}
}

template <typename T, typename S, typename X>
inline T* GetInterfaceAddress(T*& Interface, T*& InterfaceBackup, S* ProxyInterface, X* InterfaceX)
{
	if (!Interface)
	{
		SetCriticalSection();
		if (InterfaceBackup)
		{
			Interface = InterfaceBackup;
			InterfaceBackup = nullptr;
			Interface->SetProxy(InterfaceX);
		}
		else
		{
			Interface = new T(ProxyInterface, InterfaceX);
		}
		ReleaseCriticalSection();
	}
	return Interface;
}

template <typename D>
class AddressLookupTableDinput
{
private:
	bool ConstructorFlag = false;
	std::unordered_map<void*, class AddressLookupTableDinputObject*> g_map[MaxIndex];

	template <typename T>
	struct AddressCacheIndex { static constexpr UINT CacheIndex = 0; };
	template <>
	struct AddressCacheIndex<m_IDirectInputEffect> { static constexpr UINT CacheIndex = 1; };
	template <>
	struct AddressCacheIndex<m_IDirectInputX> { static constexpr UINT CacheIndex = 2; };
	template <>
	struct AddressCacheIndex<m_IDirectInputDeviceX> { static constexpr UINT CacheIndex = 3; };

	void DeleteAll()
	{
		for (const auto x : { 1, 2, 3 })
		{
			for (const auto& entry : g_map[x])
			{
				entry.second->DeleteMe();
			}
		}
	}

	template <typename T, typename X, typename I>
	T *FindAddressVersion(void *Proxy, REFIID riid)
	{
		T *Interface = FindAddressAllInterfaces<T>(Proxy);

		if (!Interface)
		{
			X *InterfaceX = new X((I*)Proxy, riid);

			Interface = (T*)InterfaceX->GetWrapperInterfaceX(GetGUIDVersion(riid));
		}

		return Interface;
	}

	template <typename T>
	T *FindAddressPrivate(void *Proxy)
	{
		T *Interface = FindAddressAllInterfaces<T>(Proxy);

		if (!Interface)
		{
			Interface = new T(static_cast<T *>(Proxy));
		}

		return Interface;
	}

	template <typename T>
	T *FindAddressAllInterfaces(void *Proxy)
	{
		constexpr UINT CacheIndex = AddressCacheIndex<T>::CacheIndex;

		auto it = g_map[CacheIndex].find(Proxy);
		if (it != std::end(g_map[CacheIndex]))
		{
			Logging::LogDebug() << __FUNCTION__ << " Found device address!";
			return static_cast<T *>(it->second);
		}

		return nullptr;
	}

public:
	explicit AddressLookupTableDinput() {}
	~AddressLookupTableDinput()
	{
		ConstructorFlag = true;
		DeleteAll();
	}

	template <typename T>
	T *FindAddress(void *Proxy, DWORD Version, DWORD Type)
	{
		switch (Type)
		{
		case ANSI_CHARSET:
		{
			switch (Version)
			{
			case 1:
				return (T*)FindAddress<AddressCacheIndex<T>::Type1A>(Proxy);
			case 2:
				return (T*)FindAddress<AddressCacheIndex<T>::Type2A>(Proxy);
			case 7:
				return (T*)FindAddress<AddressCacheIndex<T>::Type7A>(Proxy);
			default:
				return nullptr;
			}
		}
		case UNICODE_CHARSET:
		{
			switch (Version)
			{
			case 1:
				return (T*)FindAddress<AddressCacheIndex<T>::Type1W>(Proxy);
			case 2:
				return (T*)FindAddress<AddressCacheIndex<T>::Type2W>(Proxy);
			case 7:
				return (T*)FindAddress<AddressCacheIndex<T>::Type7W>(Proxy);
			default:
				return nullptr;
			}
		}
		default:
			return nullptr;
		}
	}

	template <typename T>
	T *FindAddress(void *Proxy)
	{
		if (!Proxy)
		{
			return nullptr;
		}

		return FindAddressPrivate<T>(Proxy);
	}

	template <typename T>
	void SaveAddress(T *Wrapper, void *Proxy)
	{
		constexpr UINT CacheIndex = AddressCacheIndex<T>::CacheIndex;
		if (Wrapper && Proxy)
		{
			g_map[CacheIndex][Proxy] = Wrapper;
		}
	}

	template <typename T>
	void DeleteAddress(T *Wrapper)
	{
		if (!Wrapper || ConstructorFlag)
		{
			return;
		}

		constexpr UINT CacheIndex = AddressCacheIndex<T>::CacheIndex;

		auto it = std::find_if(g_map[CacheIndex].begin(), g_map[CacheIndex].end(),
			[=](auto& Map) -> bool { return Map.second == Wrapper; });
		if (it != std::end(g_map[CacheIndex]))
		{
			g_map[CacheIndex].erase(it);
		}
	}
};

class AddressLookupTableDinputObject
{
public:
	virtual ~AddressLookupTableDinputObject() { }

	void DeleteMe()
	{
		delete this;
	}
};
