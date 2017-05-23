
#include <windows.h>
#include <detours.h>
#include <cstdio>

int main(const int argc, const char *argv[])
{
	char *exename = "D:\\Programming\\Multi\\th08\\th08.exe";
	char *dllname = "D:\\Programming\\Multi\\twinject\\Release\\twinhook.dll";
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