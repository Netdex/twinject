#pragma once


typedef struct
{
	const char *exename = "th08.exe";
	const char *dllname = "twinject.dll";
} configuration;

int handler(void* user, const char* section, const char* name, const char* value);