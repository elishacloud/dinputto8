#pragma once

#include <unordered_map>
#include <algorithm>

template <typename D>
class AddressLookupTableDinput
{
public:
	explicit AddressLookupTableDinput() {}
	~AddressLookupTableDinput()
	{
		ConstructorFlag = true;
		for (const auto& entry : g_map)
		{
			entry.second->DeleteMe();
		}
	}

	template <typename T>
	struct AddressCacheIndex { };
	template <>
	struct AddressCacheIndex<m_IDirectInputW> {
		using Type1A = m_IDirectInputA;
		using Type2A = m_IDirectInput2A;
		using Type7A = m_IDirectInput7A;
		using Type1W = m_IDirectInputW;
		using Type2W = m_IDirectInput2W;
		using Type7W = m_IDirectInput7W;
	};
	template <>
	struct AddressCacheIndex<m_IDirectInputDeviceW> {
		using Type1A = m_IDirectInputDeviceA;
		using Type2A = m_IDirectInputDevice2A;
		using Type7A = m_IDirectInputDevice7A;
		using Type1W = m_IDirectInputDeviceW;
		using Type2W = m_IDirectInputDevice2W;
		using Type7W = m_IDirectInputDevice7W;
	};

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
		case UNICODE:
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
		if (Proxy == nullptr)
		{
			return nullptr;
		}

		auto it = g_map.find(Proxy);

		if (it != std::end(g_map))
		{
			return static_cast<T *>(it->second);
		}

		return new T(static_cast<T *>(Proxy));
	}

	template <typename T>
	void SaveAddress(T *Wrapper, void *Proxy)
	{
		if (Wrapper != nullptr && Proxy != nullptr)
		{
			g_map[Proxy] = Wrapper;
		}
	}

	template <typename T>
	void DeleteAddress(T *Wrapper)
	{
		if (Wrapper != nullptr && !ConstructorFlag)
		{
			auto it = std::find_if(g_map.begin(), g_map.end(),
				[Wrapper](std::pair<void*, class AddressLookupTableObject*> Map) -> bool { return Map.second == Wrapper; });

			if (it != std::end(g_map))
			{
				it = g_map.erase(it);
			}
		}
	}

private:
	bool ConstructorFlag = false;
	std::unordered_map<void*, class AddressLookupTableObject*> g_map;
};

class AddressLookupTableObject
{
public:
	virtual ~AddressLookupTableObject() { }

	void DeleteMe()
	{
		delete this;
	}
};
