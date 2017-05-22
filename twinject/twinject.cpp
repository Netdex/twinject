
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
	char *exename = "D:\\Programming\\Multi\\th08\\th08.exe";
	char *dllname = "D:\\Programming\\Multi\\twinject\\Debug\\twinhook.dll";
	char *currentdir = "D:\\Programming\\Multi\\th08";

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