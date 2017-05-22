#include <Windows.h>
#include <TlHelp32.h>
#include <cstdio>

#ifdef _NEVER_DEFINED_F
BOOL InjectLibrary(HANDLE hProcess, char *fnDll)
{
	BOOL success = FALSE;
	HANDLE hThread = NULL;
	char *fnRemote = NULL;
	FARPROC procLoadLibraryA = NULL;

	size_t lenFilename = strlen(fnDll) + 1;

	/* Allocate space in the remote process */
	fnRemote = (char *)VirtualAllocEx(hProcess, NULL, lenFilename, MEM_COMMIT, PAGE_READWRITE);

	if (fnRemote)
	{
		/* Write the filename to the remote process. */
		if (WriteProcessMemory(hProcess, fnRemote, fnDll, lenFilename, NULL))
		{
			/* Get the address of the LoadLibraryA function */
			procLoadLibraryA = GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
			hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)procLoadLibraryA, fnRemote, 0, NULL);
			if (hThread)
			{
				WaitForSingleObject(hThread, INFINITE);
				success = TRUE;
			}
		}
		VirtualFreeEx(hProcess, fnRemote, 0, MEM_RELEASE);
	}

	return success;
}

BOOL EjectLibrary(HANDLE hProcess, int dwProcessId, char *fnDll)
{
	BOOL success = FALSE;
	HANDLE hSnapshot = NULL;
	HANDLE hThread = NULL;
	FARPROC procFreeLibrary = NULL;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);

	if (hSnapshot)
	{
		MODULEENTRY32 me = { sizeof(me) };
		BOOL isFound = FALSE;
		BOOL isMoreMods = Module32First(hSnapshot, &me);
		for (; isMoreMods && !isFound; isMoreMods = Module32Next(hSnapshot, &me))
			isFound = (_strcmpi(me.szModule, fnDll) == 0 || _strcmpi(me.szExePath, fnDll) == 0);

		if (isFound)
		{
			/* Get the address of the LoadLibraryA function */
			procFreeLibrary = GetProcAddress(GetModuleHandle("Kernel32"), "FreeLibrary");
			hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)procFreeLibrary, me.modBaseAddr, 0, NULL);
			if (hThread)
			{
				WaitForSingleObject(hThread, INFINITE);
				success = TRUE;
			}
		}

		CloseHandle(hSnapshot);
	}

	return success;
}

int GetProcessPID(char *name)
{
	PROCESSENTRY32 pe;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	Process32First(hSnap, &pe);
	do {
		if (!strcmp(name, pe.szExeFile)) {
			return pe.th32ProcessID;
		}
	} while (Process32Next(hSnap, &pe));
	return 0;
}

int sample_main(const int argc, const char *argv[])
{
	int pid = GetProcessPID("th08.exe");
	if (!pid)
	{
		printf("could not find process\n");
		return 0;
	}
	printf("found process pid of %d\n", pid);
	printf("injecting\n");
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (hProc) {
		if (InjectLibrary(hProc, "D:\\Programming\\Multi\\twinject\\Debug\\twinhook.dll"))
		{
			printf("successfully injected\n");
		}
		else
		{
			printf("could not inject\n");
		}
	}
	else
	{
		printf("could not open process\n");
	}
	return 0;
}
#endif