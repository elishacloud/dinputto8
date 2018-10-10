#pragma once

#include <fstream>
#include <iostream>
#include <stdarg.h> 

#ifndef DINPUTTO8NOLOG
extern std::ofstream LOG;
#endif

class Log
{
public:
	Log() {}
	~Log()
	{
#ifndef DINPUTTO8NOLOG
		if (LOG.is_open())
		{
			LOG << std::endl;
		}
#endif
	}

	template <typename T>
	Log& operator<<(const T& t)
	{
#ifndef DINPUTTO8NOLOG
		if (LOG.is_open())
		{
			LOG << t;
		}
#else
		std::cout << t;
#endif
		return *this;
	}
};

class LogDebug
{
public:
	LogDebug() {}
	~LogDebug()
	{
#ifdef _DEBUG
#ifndef DINPUTTO8NOLOG
		if (LOG.is_open())
		{
			LOG << std::endl;
		}
#endif
#endif // _DEBUG
	}

	template <typename T>
	LogDebug& operator<<(const T& t)
	{
#ifdef _DEBUG
#ifndef DINPUTTO8NOLOG
		if (LOG.is_open())
		{
			LOG << t;
		}
#else
		std::cout << t;
#endif
#else
		std::cout << t;
#endif // _DEBUG
		return *this;
	}
};

typedef struct { DWORD num; } hexDWORD;

static hexDWORD hex(DWORD val) { return *(hexDWORD*)&val; }

template <typename T>
typename std::enable_if<std::is_class<T>::value && !std::is_same<T, std::string>::value, std::ostream&>::type operator<<(std::ostream& os, const T& t)
{
	return os << static_cast<const void*>(&t);
}

static std::ostream& operator<<(std::ostream& os, hexDWORD num)
{
	return os << std::hex << num.num << std::dec;
}

static std::ostream& operator<<(std::ostream& os, const wchar_t* wchr)
{
	std::wstring ws(wchr);
	return os << std::string(ws.begin(), ws.end()).c_str();
}

#pragma warning(suppress: 4505)
static std::ostream& operator<<(std::ostream& os, REFIID riid)
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

	return os << "{"
		<< hex(riid.Data1) << ","
		<< hex(riid.Data2) << ","
		<< hex(riid.Data3) << ","
		<< hex((UINT)riid.Data4[0]) << ","
		<< hex((UINT)riid.Data4[1]) << ","
		<< hex((UINT)riid.Data4[2]) << ","
		<< hex((UINT)riid.Data4[3]) << ","
		<< hex((UINT)riid.Data4[4]) << ","
		<< hex((UINT)riid.Data4[5]) << ","
		<< hex((UINT)riid.Data4[6]) << ","
		<< hex((UINT)riid.Data4[7]) << ","
		<< "}";
}

#pragma warning(suppress: 4505)
static void logf(char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	auto size = vsnprintf(nullptr, 0, fmt, ap);
	std::string output(size + 1, '\0');
	vsprintf_s(&output[0], size + 1, fmt, ap);
	Log() << output.c_str();
	va_end(ap);
}

#pragma warning(suppress: 4505)
static void logf(wchar_t * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
#pragma warning(suppress: 4996)
	auto size = _vsnwprintf(nullptr, 0, fmt, ap);
	std::wstring output(size + 1, '\0');
	vswprintf_s(&output[0], size + 1, fmt, ap);
	Log() << output.c_str();
	va_end(ap);
}
