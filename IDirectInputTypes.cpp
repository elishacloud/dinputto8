/**
* Copyright (C) 2025 Elisha Riedlinger
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/

#include "dinputto8.h"
#include <hidusage.h>

namespace dinputto8
{
	CRITICAL_SECTION dics;
	bool IsInitialized = false;
}

const DIDATAFORMAT c_dfDIKeyboard = {
	sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),
	DIDF_RELAXIS,
	MAX_KEYBAORD,
	sizeof(dfDIKeyboard) / sizeof(*dfDIKeyboard),
	(LPDIOBJECTDATAFORMAT)dfDIKeyboard
};

const DIDATAFORMAT c_dfDIMouse = {
	sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),
	DIDF_RELAXIS,
	sizeof(DIMOUSESTATE),
	sizeof(dfDIMouse) / sizeof(*dfDIMouse),
	(LPDIOBJECTDATAFORMAT)dfDIMouse
};

const DIDATAFORMAT c_dfDIMouse2 = {
	sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),
	DIDF_RELAXIS,
	sizeof(DIMOUSESTATE2),
	sizeof(dfDIMouse2) / sizeof(*dfDIMouse2),
	(LPDIOBJECTDATAFORMAT)dfDIMouse2
};

const DIDATAFORMAT c_dfDIJoystick = {
	sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),
	DIDF_ABSAXIS,
	sizeof(DIJOYSTATE),
	sizeof(dfDIJoystick) / sizeof(*dfDIJoystick),
	(LPDIOBJECTDATAFORMAT)dfDIJoystick
};

const DIDATAFORMAT c_dfDIJoystick2 = {
	sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),
	DIDF_ABSAXIS,
	sizeof(DIJOYSTATE2),
	sizeof(dfDIJoystick2) / sizeof(*dfDIJoystick2),
	(LPDIOBJECTDATAFORMAT)dfDIJoystick2
};

DWORD ConvertDevTypeTo7(DWORD dwDevType, WORD wUsagePage, WORD wUsage, BOOL isHID, BOOL& IsGamepad)
{
	IsGamepad = FALSE;
	switch (dwDevType)
	{
	case DIDEVTYPE_DEVICE:
	case DI8DEVTYPE_DEVICE:
	case DI8DEVTYPE_DEVICECTRL:
	default:
		if (isHID && wUsagePage == HID_USAGE_PAGE_GENERIC)
		{
			if (wUsage == HID_USAGE_GENERIC_MOUSE)
			{
				return DIDEVTYPE_MOUSE;
			}
			else if (wUsage == HID_USAGE_GENERIC_JOYSTICK)
			{
				return DIDEVTYPE_JOYSTICK;
			}
			else if (wUsage == HID_USAGE_GENERIC_GAMEPAD)
			{
				IsGamepad = TRUE;
				return DIDEVTYPE_JOYSTICK;
			}
			else if (wUsage == HID_USAGE_GENERIC_KEYBOARD)
			{
				return DIDEVTYPE_KEYBOARD;
			}
		}
		if (isHID && wUsagePage == HID_USAGE_PAGE_KEYBOARD)
		{
			return DIDEVTYPE_KEYBOARD;
		}
		return DIDEVTYPE_DEVICE;
	case DIDEVTYPE_MOUSE:
	case DI8DEVTYPE_MOUSE:
		return DIDEVTYPE_MOUSE;
	case DIDEVTYPE_KEYBOARD:
	case DI8DEVTYPE_KEYBOARD:
		return DIDEVTYPE_KEYBOARD;
	case DI8DEVTYPE_GAMEPAD:
		IsGamepad = TRUE;
		[[fallthrough]];
	case DIDEVTYPE_JOYSTICK:
	case DI8DEVTYPE_JOYSTICK:
	case DI8DEVTYPE_DRIVING:
	case DI8DEVTYPE_FLIGHT:
	case DI8DEVTYPE_SUPPLEMENTAL:
	case DI8DEVTYPE_1STPERSON:
	case DI8DEVTYPE_SCREENPOINTER:
	case DI8DEVTYPE_REMOTE:
		return DIDEVTYPE_JOYSTICK;
	}
}

DWORD ConvertDevSubTypeTo7(DWORD dwDevType, DWORD dwDevType7, DWORD dwDevSubType, BOOL IsGamepad)
{
	switch (dwDevType)
	{
	case DIDEVTYPE_DEVICE:
	case DI8DEVTYPE_DEVICE:
	case DI8DEVTYPE_DEVICECTRL:
	default:
		switch (dwDevType7)
		{
		case DIDEVTYPE_MOUSE:
			return DIDEVTYPEMOUSE_UNKNOWN;
		case DIDEVTYPE_KEYBOARD:
			return DIDEVTYPEKEYBOARD_UNKNOWN;
		case DIDEVTYPE_JOYSTICK:
			if (IsGamepad)
				return DIDEVTYPEJOYSTICK_GAMEPAD;
			else
				return DIDEVTYPEJOYSTICK_TRADITIONAL;
		}
		return 0;
	case DIDEVTYPE_MOUSE:
	case DI8DEVTYPE_MOUSE:
		switch (dwDevSubType)
		{
		case DI8DEVTYPEMOUSE_ABSOLUTE:
			return DIDEVTYPEMOUSE_TOUCHPAD;
		default:
			return dwDevSubType;
		}
	case DIDEVTYPE_KEYBOARD:
	case DI8DEVTYPE_KEYBOARD:
		return dwDevSubType;
	case DIDEVTYPE_JOYSTICK:
	case DI8DEVTYPE_JOYSTICK:
		return DIDEVTYPEJOYSTICK_TRADITIONAL;
	case DI8DEVTYPE_GAMEPAD:
		return DIDEVTYPEJOYSTICK_GAMEPAD;
	case DI8DEVTYPE_DRIVING:
		return DIDEVTYPEJOYSTICK_WHEEL;
	case DI8DEVTYPE_FLIGHT:
		return DIDEVTYPEJOYSTICK_FLIGHTSTICK;
	case DI8DEVTYPE_SUPPLEMENTAL:
		switch (dwDevSubType)
		{
		case DI8DEVTYPESUPPLEMENTAL_HEADTRACKER:
			return DIDEVTYPEJOYSTICK_HEADTRACKER;
		case DI8DEVTYPESUPPLEMENTAL_RUDDERPEDALS:
			return DIDEVTYPEJOYSTICK_RUDDER;
		default:
			return DIDEVTYPEJOYSTICK_UNKNOWN;
		}
	case DI8DEVTYPE_1STPERSON:
	case DI8DEVTYPE_SCREENPOINTER:
	case DI8DEVTYPE_REMOTE:
		return DIDEVTYPEJOYSTICK_UNKNOWN;
	}
}

void dinputto8::SetCriticalSection()
{
	if (!IsInitialized)
	{
		InitializeCriticalSection(&dics);
		IsInitialized = true;
	}
	if (IsInitialized)
	{
		EnterCriticalSection(&dics);
		return;
	}
}

void dinputto8::ReleaseCriticalSection()
{
	if (IsInitialized)
	{
		LeaveCriticalSection(&dics);
		return;
	}
}
