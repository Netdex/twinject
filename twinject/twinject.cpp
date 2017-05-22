
#include <windows.h>
#include <detours.h>
#include <cstdio>

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

int main(const int argc, const char *argv[])
{
	char *exename = "D:\\Games\\Touhou Project\\11.0 ~ Subterranean Animism\\th11.exe";
	char *dllname = "D:\\Programming\\Multi\\twinject\\Debug\\twinhook.dll";
	char *currentdir = "D:\\Games\\Touhou Project\\11.0 ~ Subterranean Animism";

	//STARTUPINFOA siStartupInfo;
	//PROCESS_INFORMATION piProcessInfo;
	//memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	//memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	//siStartupInfo.cb = sizeof(siStartupInfo);

	//if (!CreateProcessA(NULL,
	//	exename, NULL, NULL, false,
	//	CREATE_SUSPENDED, NULL, currentdir,
	//	&siStartupInfo, &piProcessInfo)) {
	//	MessageBoxA(NULL, exename, "Error", MB_OK);
	//}

	//// get the process id for injection
	//DWORD pId = piProcessInfo.dwProcessId;
	//HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pId);

	//// Inject the dll
	//if (!InjectLibrary(hProc, dllname)) {
	//	MessageBoxA(NULL, "Injection failed", "Error", MB_OK);
	//}

	//ResumeThread(piProcessInfo.hThread);

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };

	if(DetourCreateProcessWithDll(exename, NULL, NULL, NULL, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE, NULL,
		currentdir, &si, &pi, dllname, NULL))
	{
		printf("successful injection\n");
	}
	else
	{
		printf("failed injection\n");
	}
	return 0;

}