//---------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#if defined(_ICU_) && !defined(_ICUSWORD_)
#define _USTDIO_
#include <unicode/ustdio.h>
#include <unicode/ustream.h>
#endif
#include "swlog.h"
//---------------------------------------------------------------------------

SWORD_NAMESPACE_START

SWLog *SWLog::systemlog = 0;


class __staticsystemlog {
public:
	__staticsystemlog() {
		SWLog::systemlog = new SWLog();
	}
	~__staticsystemlog() {
		delete SWLog::systemlog;
	}
} _staticsystemlog;


void SWLog::LogWarning(char *fmt, ...)
{
	//#ifdef _USTDIO_
	//UChar msg[2048];
	//#else
	char msg[2048];
	//#endif
	va_list argptr;

	if (logLevel >= 2) {
		va_start(argptr, fmt);
		//#ifdef _USTDIO_
		//u_vsprintf(msg, NULL, fmt, argptr);
		//#else
		vsprintf(msg, fmt, argptr);
		//#endif
		va_end(argptr);

		std::cerr << msg;
		std::cerr << std::endl;
	}
}


void SWLog::LogError(char *fmt, ...)
{
	//#ifdef _USTDIO_
	//UChar msg[2048];
	//#else
	char msg[2048];
	//#endif
	va_list argptr;

	if (logLevel) {
		va_start(argptr, fmt);
		//#ifdef _USTDIO_
		//u_vsprintf(msg, NULL, fmt, argptr);
		//#else
		vsprintf(msg, fmt, argptr);
		//#endif
		va_end(argptr);

		std::cerr << msg;
		std::cerr << std::endl;
	}
}


void SWLog::LogTimedInformation(char *fmt, ...)
{
	//#ifdef _USTDIO_
	//UChar msg[2048];
	//#else
	char msg[2048];
	//#endif
	va_list argptr;

	if (logLevel >= 4) {
		va_start(argptr, fmt);
		//#ifdef _USTDIO_
		//u_vsprintf(msg, NULL, fmt, argptr);
		//#else
		vsprintf(msg, fmt, argptr);
		//#endif
		va_end(argptr);

		std::cout << msg;
		std::cout << std::endl;
	}
}


void SWLog::LogInformation(char *fmt, ...)
{
	//#ifdef _USTDIO_
	//UChar msg[2048];
	//#else
	char msg[2048];
	//#endif
	va_list argptr;

	if (logLevel >= 3) {
		va_start(argptr, fmt);
		//#ifdef _USTDIO_
		//u_vsprintf(msg, NULL, fmt, argptr);
		//#else
		vsprintf(msg, fmt, argptr);
		//#endif
		va_end(argptr);

		std::cout << msg;
		std::cout << std::endl;
	}
}

SWORD_NAMESPACE_END
