#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fstream>

#ifndef DINPUTTO8NOLOG
#include "External\Logging\Logging.h"
#endif

typedef enum _DIERR { } DIERR;

#ifdef DINPUTTO8NOLOG

#define LOG_ONCE(msg)
#define LOG_LIMIT(num, msg)

namespace Logging
{
	extern bool EnableLogging;

	template <typename T> void Open(T) {}
	template <typename Num> struct Hex { explicit Hex(Num) {} };
	template <typename Num> Hex<Num> hex(Num val) { return Hex<Num>(val); }
	template <typename Num> std::ostream& operator<<(std::ostream& os, Hex<Num>) { return os; }

	class Log
	{
	public:
		Log::Log(bool LogEvent = true) { UNREFERENCED_PARAMETER(LogEvent); }
		Log::~Log() {}

		template <typename T>
		Log& operator<<(const T&) { return *this; }
	};

	class LogStruct
	{
	public:
		LogStruct(std::ostream& os) : m_os(os) {}
		~LogStruct() {}
		template <typename T> LogStruct& operator<<(const T&) { return *this; }

		operator std::ostream&() { return m_os; }

	private:
		std::ostream& m_os;
	};

	typedef Log LogDebug;

	void LogFormat(char *, ...);
	void LogFormat(wchar_t *, ...);
	void LogProcessNameAndPID();
	void LogOSVersion();
	void LogComputerManufacturer();
	void LogVideoCard();
	void LogGameType();
}

extern std::ofstream LOG;

#endif

void LogDataFormat(LPCDIDATAFORMAT lpdf);
std::ostream& operator<<(std::ostream& os, DIEFFECT df);
std::ostream& operator<<(std::ostream& os, LPCDIEFFECT lpdf);
std::ostream& operator<<(std::ostream& os, DIENVELOPE df);
std::ostream& operator<<(std::ostream& os, LPDIENVELOPE lpdf);
std::ostream& operator<<(std::ostream& os, DIDATAFORMAT df);
std::ostream& operator<<(std::ostream& os, LPCDIDATAFORMAT lpdf);
std::ostream& operator<<(std::ostream& os, DIOBJECTDATAFORMAT odf);
std::ostream& operator<<(std::ostream& os, LPCDIOBJECTDATAFORMAT rgodf);
std::ostream& operator<<(std::ostream& os, const DIERR& ErrCode);
std::ostream& operator<<(std::ostream& os, REFIID riid);
