#include "stdafx.h"
#include "AutoBombPatch.h"

void AutoBomb_Patch(HANDLE hProcess)
{
	WriteProcessMemory(hProcess, (void*)0x0044CC18, "\xFF\x89", 2, NULL);
	WriteProcessMemory(hProcess, (void*)0x0044CC21, "\x66\xC7\x05\x28\xD5\x64\x01\x02", 8, NULL);
}

void AutoBomb_Unpatch(HANDLE hProcess)
{
	WriteProcessMemory(hProcess, (void*)0x0044CC18, "\x8B\x91", 2, NULL);
	WriteProcessMemory(hProcess, (void*)0x0044CC21, "\x8B\x45\xF8\x89\x90\x68\x2A\x0E", 8, NULL);
}