#pragma once

struct ENUMEFFECT
{
	LPVOID pvRef;
	LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback;
};

class m_IDirectInputEnumEffect
{
public:
	m_IDirectInputEnumEffect() {}
	~m_IDirectInputEnumEffect() {}

	static BOOL CALLBACK EnumEffectCallback(LPDIRECTINPUTEFFECT, LPVOID);
};

struct ENUMDEVICE
{
	LPVOID pvRef;
	LPVOID lpCallback;
};

class m_IDirectInputEnumDevice
{
public:
	m_IDirectInputEnumDevice() {}
	~m_IDirectInputEnumDevice() {}

	static BOOL CALLBACK EnumDeviceCallback(LPCDIDEVICEINSTANCEA lpddi, LPVOID pvRef)
	{
		return EnumDeviceCallbackX<IDirectInput8A, DIDEVICEINSTANCEA, LPDIENUMDEVICESCALLBACKA>(lpddi, pvRef);
	}
	static BOOL CALLBACK EnumDeviceCallback(LPCDIDEVICEINSTANCEW lpddi, LPVOID pvRef)
	{
		return EnumDeviceCallbackX<IDirectInput8W, DIDEVICEINSTANCEW, LPDIENUMDEVICESCALLBACKW>(lpddi, pvRef);
	}
	template <class T, class V, class D>
	static BOOL CALLBACK EnumDeviceCallbackX(const V *lpddi, LPVOID pvRef);
};
