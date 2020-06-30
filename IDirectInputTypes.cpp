/**
* Copyright (C) 2020 Elisha Riedlinger
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

#define DIDEVTYPE_DEVICE        1
#define DIDEVTYPE_MOUSE         2
#define DIDEVTYPE_KEYBOARD      3
#define DIDEVTYPE_JOYSTICK      4

DWORD ConvertDevTypeTo7(DWORD dwDevType)
{
	switch (dwDevType)
	{
	case DI8DEVTYPE_DEVICE:
		return DIDEVTYPE_DEVICE;
	case DI8DEVTYPE_MOUSE:
		return DIDEVTYPE_MOUSE;
	case DI8DEVTYPE_KEYBOARD:
		return DIDEVTYPE_KEYBOARD;
	case DI8DEVTYPE_JOYSTICK:
	case DI8DEVTYPE_GAMEPAD:
		return DIDEVTYPE_JOYSTICK;
	default:
		return dwDevType;
	}
}

DWORD ConvertDevTypeTo8(DWORD dwDevType)
{
	switch (dwDevType)
	{
	case DIDEVTYPE_DEVICE:
		return DI8DEVTYPE_DEVICE;
	case DIDEVTYPE_MOUSE:
		return DI8DEVTYPE_MOUSE;
	case DIDEVTYPE_KEYBOARD:
		return DI8DEVTYPE_KEYBOARD;
	case DIDEVTYPE_JOYSTICK:
		return DI8DEVTYPE_GAMEPAD;
	default:
		return dwDevType;
	}
}
