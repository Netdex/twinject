#include "debugger.h"

#include <cstdio>
#include <tchar.h>
#include <psapi.h>
#include <strsafe.h>
#include <vector>
#include <atlstr.h>  
#include "winmanip.h"

extern PROCESS_INFORMATION pi;

DWORD OnLoadDllDebugEvent(const LPDEBUG_EVENT DebugEv)
{
	//if (DebugEv->u.LoadDll.lpImageName)
	//{
	//	LPTSTR path = GetFilePathFromHandle(DebugEv->u.LoadDll.hFile);
	//	LPTSTR name = GetFileName(path);
	//	// printf("DLL Load: %s\n", name);
	//	delete[] path;
	//	delete[] name;
	//}

	return DBG_CONTINUE;
}

DWORD OnOutputDebugStringEvent(const LPDEBUG_EVENT debug_event)
{
	CStringW strEventMessage;
	OUTPUT_DEBUG_STRING_INFO & DebugString = debug_event->u.DebugString;
	WCHAR *msg = new WCHAR[DebugString.nDebugStringLength];

	ReadProcessMemory(pi.hProcess, DebugString.lpDebugStringData, msg, DebugString.nDebugStringLength, NULL);

	if (DebugString.fUnicode)
		strEventMessage = msg;
	else
		strEventMessage = (char*)msg;
	_tprintf("%ws\n", (LPCWSTR)strEventMessage);
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