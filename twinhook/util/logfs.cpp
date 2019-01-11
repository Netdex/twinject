#include "../stdafx.h"
#include "logfs.h"

void logm(const char* file, const char* func, int line, const char* fmt, ...)
{
	char szUserFmt[256];
	char szDebugMsg[256];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(szUserFmt, 256, fmt, args);
	sprintf_s(szDebugMsg, 256, "%20s:%03d @%30s(): %s", file, line, func, szUserFmt);
	OutputDebugStringA(szDebugMsg);
	va_end(args);
}
