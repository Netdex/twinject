
#include <windows.h>
#include <detours.h>
#include <tchar.h>
#include <psapi.h>
#include <strsafe.h>
#include <atlstr.h>

#include "debugger.h"
#include "config.h"
#include "ini.h"

STARTUPINFOA si;
PROCESS_INFORMATION pi;

const char *ininame = "twinject.ini";

#define DEBUGGER				// If you use an external debugger, this line must be commented out. Debugging messages will not be received.
//#define TH10_LOADER				// Since I have not implemented the universal loader, you must define the game to load here.

int main(const int argc, const char *argv[])
{
	printf("TWINJECT - Touhou Windows Injector (netdex)\n\
===========================================\n\n");

	// The following block of code defines debugging paths to the games I have in my debug environment.
	// TODO use dynamic configuration based loading instead of hard coding everything
#if defined(TH07_LOADER)
	_putenv("th=th07");
	char *exepath = "D:\\Programming\\Multi\\th07\\th07.exe";
	char *dllpath = "D:\\Programming\\Multi\\twinject\\Release\\twinhook.dll";
	char *currentdir = "D:\\Programming\\Multi\\th07";
#elif defined(TH08_LOADER)
	_putenv("th=th08");
	char *exepath = "D:\\Programming\\Multi\\th08\\th08.exe";
	char *dllpath = "D:\\Programming\\Multi\\twinject\\Release\\twinhook.dll";
	char *currentdir = "D:\\Programming\\Multi\\th08";
#elif defined(TH10_LOADER)
	_putenv("th=th10");
	char *exepath = "D:\\Programming\\Multi\\th10\\th10.exe";
	char *dllpath = "D:\\Programming\\Multi\\twinject\\Release\\twinhook.dll";
	char *currentdir = "D:\\Programming\\Multi\\th10";
#elif defined(TH15_LOADER)
	_putenv("th=th15");
	char *exepath = "D:\\Programming\\Multi\\th15\\th15.exe";
	char *dllpath = "D:\\Programming\\Multi\\twinject\\Release\\twinhook.dll";
	char *currentdir = "D:\\Programming\\Multi\\th15";
#elif defined(USER_LOADER)
	_putenv("th=th08");
	char *exepath = "D:\\Games\\Touhou Project\\08.0 ~ Imperishable Night\\th08.exe";
	char *dllpath = "D:\\Programming\\Multi\\twinject\\Release\\twinhook.dll";
	char *currentdir = "D:\\Games\\Touhou Project\\08.0 ~ Imperishable Night";
#else
	// The following code loads configuration data from an external file
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
	_putenv(config.env);
#endif
	char twinjectDir[256];
	GetModuleFileName(NULL, twinjectDir, 256);
	SetDllDirectory(twinjectDir);
	printf("SetDllDirectory(%s)\n", twinjectDir);

	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);
	if (DetourCreateProcessWithDll(exepath, NULL, NULL, NULL, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE 
#ifdef DEBUGGER
		| DEBUG_PROCESS
#endif
		, NULL,
		currentdir, &si, &pi, dllpath, NULL))
	{
		printf("twinject: Injection OK\n");
	}
	else
	{
		printf("twinject: Injection Fail\n");
	}

	DEBUG_EVENT debugEv;
	EnterDebugLoop(&debugEv);

	return 0;
}

