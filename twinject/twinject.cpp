
#include <windows.h>
#include <detours.h>
#include <cstdio>
#include <tchar.h>
#include <string.h>
#include <psapi.h>
#include <strsafe.h>
#include <vector>
#include <atlstr.h>

#include "debugger.h"
#include "ini.h"
#include "config.h"

STARTUPINFOA si;
PROCESS_INFORMATION pi;

const char *ininame = "twinject.ini";

int main(const int argc, const char *argv[])
{
#ifdef DEBUG
	char *exepath = "D:\\Programming\\Multi\\th08\\th08.exe";
	char *dllpath = "D:\\Programming\\Multi\\twinject\\Debug\\twinhook.dll";
	char *currentdir = "D:\\Programming\\Multi\\th08";
#else
	configuration config;

	if (ini_parse(ininame, handler, &config) < 0) {
		printf("failed to load configuration %s\n", ininame);
		return 1;
	}

	char exepath[MAX_PATH];
	char dllpath[MAX_PATH];
	char currentdir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentdir);
	PathCombine(exepath, currentdir, config.exename);
	PathCombine(dllpath, currentdir, config.dllname);
#endif

	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);
	if (DetourCreateProcessWithDll(exepath, NULL, NULL, NULL, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | DEBUG_PROCESS, NULL,
		currentdir, &si, &pi, dllpath, NULL))
	{
		printf("Injection: OK\n");
	}
	else
	{
		printf("Injection: Fail\n");
	}

	DEBUG_EVENT debugEv;
	EnterDebugLoop(&debugEv);

	return 0;

}

