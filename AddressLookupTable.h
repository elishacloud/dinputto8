#pragma once

#include <unordered_map>
#include <algorithm>
#include "dinputto8.h"

constexpr UINT MaxIndex = 16;

template <typename D>
class AddressLookupTableDinput
{
private:
	bool ConstructorFlag = false;
	std::unordered_map<void*, class AddressLookupTableDinputObject*> g_map[MaxIndex];

	template <typename T>
	struct AddressCacheIndex { static constexpr UINT CacheIndex = 0; };
	template <>
	struct AddressCacheIndex<m_IDirectInputA> { static constexpr UINT CacheIndex = 1; };
	template <>
	struct AddressCacheIndex<m_IDirectInputW> { static constexpr UINT CacheIndex = 2;
		using Type1A = m_IDirectInputA;
		using Type2A = m_IDirectInput2A;
		using Type7A = m_IDirectInput7A;
		using Type1W = m_IDirectInputW;
		using Type2W = m_IDirectInput2W;
		using Type7W = m_IDirectInput7W;
	};
	template <>
	struct AddressCacheIndex<m_IDirectInput2A> { static constexpr UINT CacheIndex = 3; };
	template <>
	struct AddressCacheIndex<m_IDirectInput2W> { static constexpr UINT CacheIndex = 4; };
	template <>
	struct AddressCacheIndex<m_IDirectInput7A> { static constexpr UINT CacheIndex = 5; };
	template <>
	struct AddressCacheIndex<m_IDirectInput7W> { static constexpr UINT CacheIndex = 6; };
	template <>
	struct AddressCacheIndex<m_IDirectInputDeviceA> { static constexpr UINT CacheIndex = 7; };
	template <>
	struct AddressCacheIndex<m_IDirectInputDeviceW> { static constexpr UINT CacheIndex = 8;
		using Type1A = m_IDirectInputDeviceA;
		using Type2A = m_IDirectInputDevice2A;
		using Type7A = m_IDirectInputDevice7A;
		using Type1W = m_IDirectInputDeviceW;
		using Type2W = m_IDirectInputDevice2W;
		using Type7W = m_IDirectInputDevice7W;
	};
	template <>
	struct AddressCacheIndex<m_IDirectInputDevice2A> { static constexpr UINT CacheIndex = 9; };
	template <>
	struct AddressCacheIndex<m_IDirectInputDevice2W> { static constexpr UINT CacheIndex = 10; };
	template <>
	struct AddressCacheIndex<m_IDirectInputDevice7A> { static constexpr UINT CacheIndex = 11; };
	template <>
	struct AddressCacheIndex<m_IDirectInputDevice7W> { static constexpr UINT CacheIndex = 12; };
	template <>
	struct AddressCacheIndex<m_IDirectInputEffect> { static constexpr UINT CacheIndex = 13; };
	template <>
	struct AddressCacheIndex<m_IDirectInputX> { static constexpr UINT CacheIndex = 14; };
	template <>
	struct AddressCacheIndex<m_IDirectInputDeviceX> { static constexpr UINT CacheIndex = 15; };

	void DeleteAll()
	{
		for (const auto& x : { 13, 14, 15 })
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
	template <>
	m_IDirectInputA *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInputA, m_IDirectInputX, IDirectInput8W>(Proxy, IID_IDirectInputA); }
	template <>
	m_IDirectInputW *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInputW, m_IDirectInputX, IDirectInput8W>(Proxy, IID_IDirectInputW); }
	template <>
	m_IDirectInput2A *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInput2A, m_IDirectInputX, IDirectInput8W>(Proxy, IID_IDirectInput2A); }
	template <>
	m_IDirectInput2W *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInput2W, m_IDirectInputX, IDirectInput8W>(Proxy, IID_IDirectInput2W); }
	template <>
	m_IDirectInput7A *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInput7A, m_IDirectInputX, IDirectInput8W>(Proxy, IID_IDirectInput7A); }
	template <>
	m_IDirectInput7W *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInput7W, m_IDirectInputX, IDirectInput8W>(Proxy, IID_IDirectInput7W); }
	template <>
	m_IDirectInputDeviceA *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInputDeviceA, m_IDirectInputDeviceX, IDirectInputDevice8W>(Proxy, IID_IDirectInputDeviceA); }
	template <>
	m_IDirectInputDeviceW *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInputDeviceW, m_IDirectInputDeviceX, IDirectInputDevice8W>(Proxy, IID_IDirectInputDeviceW); }
	template <>
	m_IDirectInputDevice2A *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInputDevice2A, m_IDirectInputDeviceX, IDirectInputDevice8W>(Proxy, IID_IDirectInputDevice2A); }
	template <>
	m_IDirectInputDevice2W *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInputDevice2W, m_IDirectInputDeviceX, IDirectInputDevice8W>(Proxy, IID_IDirectInputDevice2W); }
	template <>
	m_IDirectInputDevice7A *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInputDevice7A, m_IDirectInputDeviceX, IDirectInputDevice8W>(Proxy, IID_IDirectInputDevice7A); }
	template <>
	m_IDirectInputDevice7W *FindAddressPrivate(void *Proxy) { return FindAddressVersion<m_IDirectInputDevice7W, m_IDirectInputDeviceX, IDirectInputDevice8W>(Proxy, IID_IDirectInputDevice7W); }

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
		void DeleteAll();
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
			[=](auto Map) -> bool { return Map.second == Wrapper; });

		if (it != std::end(g_map[CacheIndex]))
		{
			it = g_map[CacheIndex].erase(it);
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
