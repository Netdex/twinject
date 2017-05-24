
#include <windows.h>
#include <detours.h>
#include <cstdio>
#include <tchar.h>
#include <string.h>
#include <psapi.h>
#include <strsafe.h>
#include <vector>
#include <atlstr.h>  

#define BUFSIZE 512

bool Injected = false;

LPTSTR FileName(LPTSTR path)
{
	TCHAR *pszFileName = new TCHAR[MAX_PATH + 1];
	char* lm = strrchr(path, '\\');
	if (!lm) return NULL;
	strcpy_s(pszFileName, MAX_PATH + 1, path + (lm - path) + 1);
	return pszFileName;
}


LPTSTR GetFilePathFromHandle(HANDLE hFile)
{
	BOOL bSuccess = FALSE;
	TCHAR *pszFilePath = new TCHAR[MAX_PATH + 1];
	HANDLE hFileMap;

	// Get the file size.
	DWORD dwFileSizeHi = 0;
	DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

	if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
	{
		return NULL;
	}

	// Create a file mapping object.
	hFileMap = CreateFileMapping(hFile,
		NULL,
		PAGE_READONLY,
		0,
		1,
		NULL);

	if (hFileMap)
	{
		// Create a file mapping to get the file name.
		void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

		if (pMem)
		{
			if (GetMappedFileName(GetCurrentProcess(),
				pMem,
				pszFilePath,
				MAX_PATH))
			{

				// Translate path with device name to drive letters.
				TCHAR szTemp[BUFSIZE];
				szTemp[0] = '\0';

				if (GetLogicalDriveStrings(BUFSIZE - 1, szTemp))
				{
					TCHAR szName[MAX_PATH];
					TCHAR szDrive[3] = TEXT(" :");
					BOOL bFound = FALSE;
					TCHAR* p = szTemp;

					do
					{
						// Copy the drive letter to the template string
						*szDrive = *p;

						// Look up each device name
						if (QueryDosDevice(szDrive, szName, MAX_PATH))
						{
							size_t uNameLen = _tcslen(szName);

							if (uNameLen < MAX_PATH)
							{
								bFound = _tcsnicmp(pszFilePath, szName, uNameLen) == 0
									&& *(pszFilePath + uNameLen) == _T('\\');

								if (bFound)
								{
									// Reconstruct pszFilename using szTempFile
									// Replace device path with DOS path
									TCHAR szTempFile[MAX_PATH];
									StringCchPrintf(szTempFile,
										MAX_PATH,
										TEXT("%s%s"),
										szDrive,
										pszFilePath + uNameLen);
									StringCchCopyN(pszFilePath, MAX_PATH + 1, szTempFile, _tcslen(szTempFile));
								}
							}
						}

						// Go to the next NULL character.
						while (*p++);
					} while (!bFound && *p); // end of string
				}
			}
			bSuccess = TRUE;
			UnmapViewOfFile(pMem);
		}

		CloseHandle(hFileMap);
	}
	return pszFilePath;
}

STARTUPINFOA si;
PROCESS_INFORMATION pi;

char *exename = "D:\\Programming\\Multi\\th08\\th08.exe";
char *dllname = "D:\\Programming\\Multi\\twinject\\Release\\twinhook.dll";
char *currentdir = "D:\\Programming\\Multi\\th08";

DWORD OnLoadDllDebugEvent(const LPDEBUG_EVENT DebugEv)
{
	if (DebugEv->u.LoadDll.lpImageName)
	{
		LPTSTR path = GetFilePathFromHandle(DebugEv->u.LoadDll.hFile);
		LPTSTR name = FileName(path);
		// printf("DLL Load: %s\n", name);
		delete[] path;
		delete[] name;
	}

	return DBG_CONTINUE;
}
DWORD OnOutputDebugStringEvent(const LPDEBUG_EVENT debug_event)
{
	CStringW strEventMessage;
	OUTPUT_DEBUG_STRING_INFO & DebugString = debug_event->u.DebugString;
	WCHAR *msg = new WCHAR[DebugString.nDebugStringLength];

	ReadProcessMemory(pi.hProcess,DebugString.lpDebugStringData,msg,  DebugString.nDebugStringLength, NULL);

	if (DebugString.fUnicode)
		strEventMessage = msg;
	else
		strEventMessage = (char*)msg;
	_tprintf("DEBUG: %ws\n", (LPCWSTR) strEventMessage);
	delete[]msg;
	return DBG_CONTINUE;
}
void EnterDebugLoop(const LPDEBUG_EVENT DebugEv)
{
	DWORD dwContinueStatus = DBG_CONTINUE;
	for (;;)
	{
		WaitForDebugEvent(DebugEv, INFINITE);
		switch (DebugEv->dwDebugEventCode)
		{
		case EXCEPTION_DEBUG_EVENT:
			// Process the exception code. When handling 
			// exceptions, remember to set the continuation 
			// status parameter (dwContinueStatus). This value 
			// is used by the ContinueDebugEvent function. 

			switch (DebugEv->u.Exception.ExceptionRecord.ExceptionCode)
			{
			case EXCEPTION_ACCESS_VIOLATION:
				// First chance: Pass this on to the system. 
				// Last chance: Display an appropriate error. 
				printf("EXCEPTION_ACCESS_VIOLATION\n");
				break;

			case EXCEPTION_BREAKPOINT:
				// First chance: Display the current 
				// instruction and register values. 
				printf("EXCEPTION_BREAKPOINT\n");
				break;

			case EXCEPTION_DATATYPE_MISALIGNMENT:
				// First chance: Pass this on to the system. 
				// Last chance: Display an appropriate error. 
				printf("EXCEPTION_DATATYPE_MISALIGNMENT\n");
				break;

			case EXCEPTION_SINGLE_STEP:
				// First chance: Update the display of the 
				// current instruction and register values. 
				printf("EXCEPTION_SINGLE_STEP\n");
				break;

			case DBG_CONTROL_C:
				// First chance: Pass this on to the system. 
				// Last chance: Display an appropriate error. 
				printf("DBG_CONTROL_C\n");
				break;

			default:
				// Handle other exceptions. 
				printf("EXCEPTION_UNKNOWN\n");
				break;
			}

			break;

		case LOAD_DLL_DEBUG_EVENT:
			dwContinueStatus = OnLoadDllDebugEvent(DebugEv);
			break;
		case OUTPUT_DEBUG_STRING_EVENT:
			// Display the output debugging string. 
			dwContinueStatus = OnOutputDebugStringEvent(DebugEv);
			break;
		}
		ContinueDebugEvent(DebugEv->dwProcessId,
			DebugEv->dwThreadId,
			dwContinueStatus);
	}
}



int main(const int argc, const char *argv[])
{
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);

	if (DetourCreateProcessWithDll(exename, NULL, NULL, NULL, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | DEBUG_PROCESS, NULL,
		currentdir, &si, &pi, dllname, NULL))
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

