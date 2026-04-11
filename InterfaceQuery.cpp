/**
* Copyright (C) 2026 Elisha Riedlinger
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

HRESULT dinputto8::hresValidInstanceAndVersion(HINSTANCE& hinst, DWORD dwVersion)
{
	bool bValidInstance;
	if (hinst != nullptr)
	{
		wchar_t path[4];
		bValidInstance = GetModuleFileNameW(hinst, path, (DWORD)std::size(path) - 1) != 0;
	}
	else
	{
		// DInput version 0x300 permits no instance...
		bValidInstance = dwVersion == 0x300;
	}

	if (!bValidInstance)
	{
		return DIERR_INVALIDPARAM;
	}

	// ...but DInput8 does not, so if the instance is empty, give it one or else it'll fail.
	if (hinst == nullptr && dwVersion == 0x300)
	{
		hinst = ::GetModuleHandle(nullptr);
	}

	if (dwVersion == 0x300 || dwVersion == 0x500 || dwVersion == 0x50A || dwVersion == 0x5B2 || dwVersion == 0x602 || dwVersion == 0x61A || dwVersion == 0x700)
	{
		return DI_OK;
	}

	if (dwVersion == 0)
	{
		return DIERR_NOTINITIALIZED;
	}
	return dwVersion < 0x700 ? DIERR_BETADIRECTINPUTVERSION : DIERR_OLDDIRECTINPUTVERSION;
}
