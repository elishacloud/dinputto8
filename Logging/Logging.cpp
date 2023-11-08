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

#include "..\dinputto8.h"
#include "Logging.h"

void LogDataFormat(const DIDATAFORMAT &df)
{
	Logging::Log() << "DIDATAFORMAT " << df;

	for (DWORD x = 0; x < df.dwNumObjs; x++)
	{
		Logging::Log() << "DIOBJECTDATAFORMAT " << x << " " << df.rgodf[x];
	}
}

void LogDataFormat(LPCDIDATAFORMAT lpdf)
{
	if (lpdf)
	{
		LogDataFormat(*lpdf);
	}
}

void LogEffectFormat(const DIEFFECT &eff, REFGUID rguid)
{
	Logging::Log() << "DIEFFECT " << eff;

	if (eff.cAxes && eff.rgdwAxes)
	{
		DWORD *pAxes = eff.rgdwAxes;

		for (int x = 0; x < (int)eff.cAxes; x++)
		{
			Logging::Log() << "DIEFFECT rgdwAxes " << x << ": " << pAxes[x];
		}
	}

	if (eff.cAxes && eff.rglDirection)
	{
		LONG *pDirection = (LONG*)eff.rglDirection;

		for (int x = 0; x < (int)eff.cAxes; x++)
		{
			Logging::Log() << "DIEFFECT rglDirection " << x << ": " << pDirection[x];
		}
	}

	if (eff.cbTypeSpecificParams && eff.lpvTypeSpecificParams)
	{
		// DIEFT_CONSTANTFORCE
		if (rguid == GUID_ConstantForce && eff.cbTypeSpecificParams == sizeof(DICONSTANTFORCE))
		{
			DICONSTANTFORCE *lSpecificParams = (DICONSTANTFORCE*)eff.lpvTypeSpecificParams;

			Logging::Log() << "DIEFFECT lpvTypeSpecificParams { lMagnitude: " << lSpecificParams->lMagnitude << " }";
		}
		// DIEFT_RAMPFORCE
		else if (rguid == GUID_RampForce && eff.cbTypeSpecificParams == sizeof(DIRAMPFORCE))
		{
			DIRAMPFORCE *lSpecificParams = (DIRAMPFORCE*)eff.lpvTypeSpecificParams;

			Logging::Log() << "DIEFFECT lpvTypeSpecificParams {" <<
				" lStart: " << lSpecificParams->lStart <<
				" lEnd: " << lSpecificParams->lEnd <<
				" }";
		}
		// DIEFT_PERIODIC
		else if ((rguid == GUID_Square || rguid == GUID_Sine || rguid == GUID_Triangle || rguid == GUID_SawtoothUp || rguid == GUID_SawtoothDown) && eff.cbTypeSpecificParams == sizeof(DIPERIODIC))
		{
			DIPERIODIC *lSpecificParams = (DIPERIODIC*)eff.lpvTypeSpecificParams;

			Logging::Log() << "DIEFFECT lpvTypeSpecificParams {" <<
				" dwMagnitude: " << lSpecificParams->dwMagnitude <<
				" lOffset: " << lSpecificParams->lOffset <<
				" dwPhase: " << lSpecificParams->dwPhase <<
				" dwPeriod: " << lSpecificParams->dwPeriod <<
				" }";
		}
		// DIEFT_CONDITION
		else if ((rguid == GUID_Spring || rguid == GUID_Damper || rguid == GUID_Inertia || rguid == GUID_Friction) && eff.cbTypeSpecificParams == sizeof(DICONDITION))
		{
			DICONDITION  *lSpecificParams = (DICONDITION *)eff.lpvTypeSpecificParams;

			Logging::Log() << "DIEFFECT lpvTypeSpecificParams {" <<
				" lOffset: " << lSpecificParams->lOffset <<
				" lPositiveCoefficient: " << lSpecificParams->lPositiveCoefficient <<
				" lNegativeCoefficient: " << lSpecificParams->lNegativeCoefficient <<
				" dwPositiveSaturation: " << lSpecificParams->dwPositiveSaturation <<
				" dwNegativeSaturation: " << lSpecificParams->dwNegativeSaturation <<
				" lDeadBand: " << lSpecificParams->lDeadBand <<
				" }";
		}
		// DIEFT_CUSTOMFORCE
		else if (rguid == GUID_CustomForce && eff.cbTypeSpecificParams == sizeof(DICUSTOMFORCE))
		{
			DICUSTOMFORCE *lSpecificParams = (DICUSTOMFORCE*)eff.lpvTypeSpecificParams;

			Logging::Log() << "DIEFFECT lpvTypeSpecificParams {" <<
				" cChannels: " << lSpecificParams->cChannels <<
				" dwSamplePeriod: " << lSpecificParams->dwSamplePeriod <<
				" cSamples: " << lSpecificParams->cSamples <<
				" rglForceData: " << lSpecificParams->rglForceData <<
				" }";

			if (lSpecificParams->rglForceData)
			{
				for (int x = 0; x < (int)lSpecificParams->cSamples; x++)
				{
					Logging::Log() << "DIEFFECT lpvTypeSpecificParams rglForceData " << x << ": " << lSpecificParams->rglForceData[x];
				}
			}
		}
		// Unknown device
		else
		{
			if (eff.lpvTypeSpecificParams)
			{
				DWORD *lSpecificParams = (DWORD*)eff.lpvTypeSpecificParams;

				for (int x = 0; x < (int)(eff.cbTypeSpecificParams / 4); x++)
				{
					Logging::Log() << "DIEFFECT lpvTypeSpecificParams " << x << ": " << lSpecificParams[x];
				}
			}
		}
	}
}

void LogEffectFormat(LPCDIEFFECT lpeff, REFGUID rguid)
{
	if (lpeff)
	{
		LogEffectFormat(*lpeff, rguid);
	}
}

std::ostream& operator<<(std::ostream& os, DIEFFECT deff)
{
	return Logging::LogStruct(os) <<
		" dwSize:" << deff.dwSize <<
		" dwFlags:" << Logging::hex(deff.dwFlags) <<
		" dwDuration:" << deff.dwDuration <<
		" dwSamplePeriod:" << deff.dwSamplePeriod <<
		" dwGain:" << deff.dwGain <<
		" dwTriggerButton:" << deff.dwTriggerButton <<
		" dwTriggerRepeatInterval:" << deff.dwTriggerRepeatInterval <<
		" cAxes:" << deff.cAxes <<
		" rgdwAxes:" << deff.rgdwAxes <<
		" rglDirection:" << deff.rglDirection <<
		" lpEnvelope:" << deff.lpEnvelope <<
		" cbTypeSpecificParams:" << deff.cbTypeSpecificParams <<
		" lpvTypeSpecificParams:" << deff.lpvTypeSpecificParams <<
		((deff.dwSize == sizeof(DIEFFECT)) ? " dwStartDelay:" : " ") << ((deff.dwSize == sizeof(DIEFFECT)) ? deff.dwStartDelay : 0) << " ";
}

std::ostream& operator<<(std::ostream& os, LPCDIEFFECT lpdeff)
{
	if (!lpdeff)
	{
		return os << (void*)lpdeff;
	}

	return os << *lpdeff;
}

std::ostream& operator<<(std::ostream& os, DIENVELOPE de)
{
	return Logging::LogStruct(os) <<
		" dwSize:" << de.dwSize <<
		" dwAttackLevel:" << de.dwAttackLevel <<
		" dwAttackTime:" << de.dwAttackTime <<
		" dwFadeLevel:" << de.dwFadeLevel <<
		" dwFadeTime:" << de.dwFadeTime << " ";
}

std::ostream& operator<<(std::ostream& os, LPDIENVELOPE lpde)
{
	if (!lpde)
	{
		return os << (void*)lpde;
	}

	return os << *lpde;
}

std::ostream& operator<<(std::ostream& os, DIDATAFORMAT df)
{
	return Logging::LogStruct(os) <<
		" ObjSize:" << df.dwObjSize <<
		" Flags:" << Logging::hex(df.dwFlags) <<
		" DataSize:" << df.dwDataSize <<
		" NumObj:" << df.dwNumObjs <<
		" Addr:" << Logging::hex((void*)df.rgodf) << " ";
}

std::ostream& operator<<(std::ostream& os, LPCDIDATAFORMAT lpdf)
{
	if (!lpdf)
	{
		return os << (void*)lpdf;
	}

	return os << *lpdf;
}

std::ostream& operator<<(std::ostream& os, DIOBJECTDATAFORMAT odf)
{
	return Logging::LogStruct(os) <<
		" GUID:" << odf.pguid <<
		" Offset:" << odf.dwOfs <<
		" Type:" << Logging::hex(odf.dwType) <<
		" Flags:" << Logging::hex(odf.dwFlags) << " ";
}

std::ostream& operator<<(std::ostream& os, LPCDIOBJECTDATAFORMAT rgodf)
{
	if (!rgodf)
	{
		return os << (void*)rgodf;
	}

	return os << *rgodf;
}

#ifndef DISABLE_COMMON_LOGGING
std::ostream& operator<<(std::ostream& os, REFIID riid)
{
#define CHECK_REFIID(riidPrefix, riidName) \
	if (riid == riidPrefix ## _ ## riidName) \
	{ \
		return os << #riidPrefix << "_" << #riidName; \
	}

	CHECK_REFIID(IID, IUnknown);
	CHECK_REFIID(IID, IClassFactory);
	// dinput
	CHECK_REFIID(CLSID, DirectInput);
	CHECK_REFIID(CLSID, DirectInputDevice);
	CHECK_REFIID(CLSID, DirectInput8);
	CHECK_REFIID(CLSID, DirectInputDevice8);
	CHECK_REFIID(IID, IDirectInputA);
	CHECK_REFIID(IID, IDirectInputW);
	CHECK_REFIID(IID, IDirectInput2A);
	CHECK_REFIID(IID, IDirectInput2W);
	CHECK_REFIID(IID, IDirectInput7A);
	CHECK_REFIID(IID, IDirectInput7W);
	CHECK_REFIID(IID, IDirectInput8A);
	CHECK_REFIID(IID, IDirectInput8W);
	CHECK_REFIID(IID, IDirectInputDeviceA);
	CHECK_REFIID(IID, IDirectInputDeviceW);
	CHECK_REFIID(IID, IDirectInputDevice2A);
	CHECK_REFIID(IID, IDirectInputDevice2W);
	CHECK_REFIID(IID, IDirectInputDevice7A);
	CHECK_REFIID(IID, IDirectInputDevice7W);
	CHECK_REFIID(IID, IDirectInputDevice8A);
	CHECK_REFIID(IID, IDirectInputDevice8W);
	CHECK_REFIID(IID, IDirectInputEffect);
	// Predefined object types
	CHECK_REFIID(GUID, XAxis);
	CHECK_REFIID(GUID, YAxis);
	CHECK_REFIID(GUID, ZAxis);
	CHECK_REFIID(GUID, RxAxis);
	CHECK_REFIID(GUID, RyAxis);
	CHECK_REFIID(GUID, RzAxis);
	CHECK_REFIID(GUID, Slider);
	CHECK_REFIID(GUID, Button);
	CHECK_REFIID(GUID, Key);
	CHECK_REFIID(GUID, POV);
	CHECK_REFIID(GUID, Unknown);
	// Predefined product GUIDs
	CHECK_REFIID(GUID, SysMouse);
	CHECK_REFIID(GUID, SysKeyboard);
	CHECK_REFIID(GUID, Joystick);
	CHECK_REFIID(GUID, SysMouseEm);
	CHECK_REFIID(GUID, SysMouseEm2);
	CHECK_REFIID(GUID, SysKeyboardEm);
	CHECK_REFIID(GUID, SysKeyboardEm2);
	// Predefined force feedback effects
	CHECK_REFIID(GUID, ConstantForce);
	CHECK_REFIID(GUID, RampForce);
	CHECK_REFIID(GUID, Square);
	CHECK_REFIID(GUID, Sine);
	CHECK_REFIID(GUID, Triangle);
	CHECK_REFIID(GUID, SawtoothUp);
	CHECK_REFIID(GUID, SawtoothDown);
	CHECK_REFIID(GUID, Spring);
	CHECK_REFIID(GUID, Damper);
	CHECK_REFIID(GUID, Inertia);
	CHECK_REFIID(GUID, Friction);
	CHECK_REFIID(GUID, CustomForce);

	return Logging::LogStruct(os)
		<< Logging::hex(riid.Data1)
		<< Logging::hex(riid.Data2)
		<< Logging::hex(riid.Data3)
		<< Logging::hex((UINT)riid.Data4[0])
		<< Logging::hex((UINT)riid.Data4[1])
		<< Logging::hex((UINT)riid.Data4[2])
		<< Logging::hex((UINT)riid.Data4[3])
		<< Logging::hex((UINT)riid.Data4[4])
		<< Logging::hex((UINT)riid.Data4[5])
		<< Logging::hex((UINT)riid.Data4[6])
		<< Logging::hex((UINT)riid.Data4[7]);
}

std::ostream& operator<<(std::ostream& os, const DIERR& ErrCode)
{
#define VISIT_DIERR_CODES(visit) \
	visit(DI_OK) \
	visit(DI_POLLEDDEVICE) \
	visit(DI_DOWNLOADSKIPPED) \
	visit(DI_EFFECTRESTARTED) \
	visit(DI_TRUNCATED) \
	visit(DI_SETTINGSNOTSAVED) \
	visit(DI_TRUNCATEDANDRESTARTED) \
	visit(DI_WRITEPROTECT) \
	visit(DIERR_OLDDIRECTINPUTVERSION) \
	visit(DIERR_BETADIRECTINPUTVERSION) \
	visit(DIERR_BADDRIVERVER) \
	visit(DIERR_DEVICENOTREG) \
	visit(DIERR_NOTFOUND) \
	visit(DIERR_OBJECTNOTFOUND) \
	visit(DIERR_INVALIDPARAM) \
	visit(DIERR_NOINTERFACE) \
	visit(DIERR_GENERIC) \
	visit(DIERR_OUTOFMEMORY) \
	visit(DIERR_UNSUPPORTED) \
	visit(DIERR_NOTINITIALIZED) \
	visit(DIERR_ALREADYINITIALIZED) \
	visit(DIERR_NOAGGREGATION) \
	visit(DIERR_OTHERAPPHASPRIO) \
	visit(DIERR_INPUTLOST) \
	visit(DIERR_ACQUIRED) \
	visit(DIERR_NOTACQUIRED) \
	visit(DIERR_READONLY) \
	visit(DIERR_HANDLEEXISTS) \
	visit(DIERR_INSUFFICIENTPRIVS) \
	visit(DIERR_DEVICEFULL) \
	visit(DIERR_MOREDATA) \
	visit(DIERR_NOTDOWNLOADED) \
	visit(DIERR_HASEFFECTS) \
	visit(DIERR_NOTEXCLUSIVEACQUIRED) \
	visit(DIERR_INCOMPLETEEFFECT) \
	visit(DIERR_NOTBUFFERED) \
	visit(DIERR_EFFECTPLAYING) \
	visit(DIERR_UNPLUGGED) \
	visit(DIERR_REPORTFULL) \
	visit(DIERR_MAPFILEFAIL) \
	visit(E_PENDING) \
	visit(E_NOINTERFACE) \
	visit(E_POINTER) \
	visit(E_HANDLE)

#define VISIT_DIERR_RETURN(x) \
	if (ErrCode == x) \
	{ \
		return os << #x; \
	}
	
	if (ErrCode == S_FALSE)
	{
		return os << "'DI_NOTATTACHED' or 'DI_BUFFEROVERFLOW' or 'DI_PROPNOEFFECT' or 'DI_NOEFFECT'";
	}

	VISIT_DIERR_CODES(VISIT_DIERR_RETURN);

	return os << Logging::hex((DWORD)ErrCode);
}
#endif // !DISABLE_COMMON_LOGGING
