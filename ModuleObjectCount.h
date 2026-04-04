#pragma once

class ModuleObjectCount
{
private:
	static inline LONG s_ObjectCount;

public:
	static bool AnyObjectsInUse()
	{
		return _InterlockedCompareExchange(&s_ObjectCount, 0, 0) > 0;
	}

	static void IncrementObjectCount()
	{
		_InterlockedIncrement(&s_ObjectCount);
	}

	static void DecrementObjectCount()
	{
		_InterlockedDecrement(&s_ObjectCount);
	}

	class CountedObject
	{
	public:
		CountedObject()
		{
			IncrementObjectCount();
		}

		~CountedObject()
		{
			DecrementObjectCount();
		}
	};
};
