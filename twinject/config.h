#pragma once


typedef struct
{
	const char *exename = "th08.exe";
	const char *dllname = "twinject.dll";
	const char *env = "th=th08";
} configuration;

int handler(void* user, const char* section, const char* name, const char* value);