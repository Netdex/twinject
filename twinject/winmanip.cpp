#include "winmanip.h"

#include <cstdio>
#include <vector>

#include <tchar.h>
#include <string.h>
#include <psapi.h>
#include <strsafe.h>
#include <atlstr.h>

#define BUFSIZE 512

LPTSTR GetFileName(LPTSTR path)
{
	TCHAR *pszFileName = new TCHAR[MAX_PATH + 1];
	char* lm = strrchr(path, '\\');
	if (!lm) return NULL;
	strcpy_s(pszFileName, MAX_PATH + 1, path + (lm - path) + 1);
	return pszFileName;
}

LPTSTR GetFilePathFromHandle(HANDLE hFile)
{
	TCHAR *pszFilePath = new TCHAR[MAX_PATH + 1];

	// Get the file size.
	DWORD dwFileSizeHi = 0;
	DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

	if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
	{
		return NULL;
	}

	// Create a file mapping object.
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 1, NULL);

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
			UnmapViewOfFile(pMem);
		}
		CloseHandle(hFileMap);
	}
	return pszFilePath;
}