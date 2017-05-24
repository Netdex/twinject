#include "stdafx.h"
#include "logfs.h"

void logfs(const char* fmt, ...)
{
	char buf[256];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(buf,256, fmt, args);
	OutputDebugStringA(buf);
	va_end(args);
}