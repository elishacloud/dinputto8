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

	static BOOL CALLBACK EnumDeviceCallbackA(LPCDIDEVICEINSTANCEA, LPVOID);
	static BOOL CALLBACK EnumDeviceCallbackW(LPCDIDEVICEINSTANCEW, LPVOID);
};
