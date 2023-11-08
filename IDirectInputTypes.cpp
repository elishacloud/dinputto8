/**
* Copyright (C) 2023 Elisha Riedlinger
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

DWORD ConvertDevTypeTo7(DWORD dwDevType)
{
	switch (dwDevType)
	{
	case DIDEVTYPE_DEVICE:
	case DI8DEVTYPE_DEVICE:
	case DI8DEVTYPE_DEVICECTRL:
	default:
		return DIDEVTYPE_DEVICE;
	case DIDEVTYPE_MOUSE:
	case DI8DEVTYPE_MOUSE:
		return DIDEVTYPE_MOUSE;
	case DIDEVTYPE_KEYBOARD:
	case DI8DEVTYPE_KEYBOARD:
		return DIDEVTYPE_KEYBOARD;
	case DIDEVTYPE_JOYSTICK:
	case DI8DEVTYPE_JOYSTICK:
	case DI8DEVTYPE_GAMEPAD:
	case DI8DEVTYPE_DRIVING:
	case DI8DEVTYPE_FLIGHT:
	case DI8DEVTYPE_SUPPLEMENTAL:
	case DI8DEVTYPE_1STPERSON:
	case DI8DEVTYPE_SCREENPOINTER:
	case DI8DEVTYPE_REMOTE:
		return DIDEVTYPE_JOYSTICK;
	}
}

DWORD ConvertDevSubTypeTo7(DWORD dwDevType, DWORD dwDevSubType)
{
	switch (dwDevType)
	{
	case DIDEVTYPE_DEVICE:
	case DI8DEVTYPE_DEVICE:
	case DI8DEVTYPE_DEVICECTRL:
	default:
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
