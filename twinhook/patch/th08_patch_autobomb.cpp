#include "stdafx.h"
#include "th08_patch_autobomb.h"

void th08_patch_autobomb::patch()
{
	HANDLE hProcess = GetCurrentProcess();
	WriteProcessMemory(hProcess, (void*)0x0044CC18, "\xFF\x89", 2, NULL);
	WriteProcessMemory(hProcess, (void*)0x0044CC21, "\x66\xC7\x05\x28\xD5\x64\x01\x02", 8, NULL);
}

void th08_patch_autobomb::unpatch()
{
	HANDLE hProcess = GetCurrentProcess();
	WriteProcessMemory(hProcess, (void*)0x0044CC18, "\x8B\x91", 2, NULL);
	WriteProcessMemory(hProcess, (void*)0x0044CC21, "\x8B\x45\xF8\x89\x90\x68\x2A\x0E", 8, NULL);
}
