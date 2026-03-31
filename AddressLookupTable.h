#pragma once

#include <unordered_map>
#include <algorithm>
#include "dinputto8.h"

class AddressLookupTableDinputObject
{
public:
	virtual ~AddressLookupTableDinputObject() = default;
};

class AddressLookupTableDinput
{
private:
	static constexpr size_t MaxCacheIndex = 3;

	bool ConstructorFlag = false;
	std::unordered_map<void*, AddressLookupTableDinputObject*> g_map[MaxCacheIndex];

	template <typename T>
	struct AddressCacheIndex {};
	template <>
	struct AddressCacheIndex<m_IDirectInputEffect> { static constexpr size_t CacheIndex = 0; };
	template <>
	struct AddressCacheIndex<m_IDirectInputX> { static constexpr size_t CacheIndex = 1; };
	template <>
	struct AddressCacheIndex<m_IDirectInputDeviceX> { static constexpr size_t CacheIndex = 2; };

	void DeleteAll()
	{
		for (const auto& map : g_map)
		{
			for (const auto& entry : map)
			{
				delete entry.second;
			}
		}
	}

	template <typename T>
	T *FindAddressAllInterfaces(void *Proxy)
	{
		constexpr size_t CacheIndex = AddressCacheIndex<T>::CacheIndex;

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
	T *FindAddress(void *Proxy)
	{
		if (!Proxy)
		{
			return nullptr;
		}

		return FindAddressAllInterfaces<T>(Proxy);
	}

	template <typename T>
	void SaveAddress(T *Wrapper, void *Proxy)
	{
		constexpr size_t CacheIndex = AddressCacheIndex<T>::CacheIndex;
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

		constexpr size_t CacheIndex = AddressCacheIndex<T>::CacheIndex;

		auto it = std::find_if(g_map[CacheIndex].begin(), g_map[CacheIndex].end(),
			[=](auto& Map) -> bool { return Map.second == Wrapper; });
		if (it != std::end(g_map[CacheIndex]))
		{
			g_map[CacheIndex].erase(it);
		}
	}
};
