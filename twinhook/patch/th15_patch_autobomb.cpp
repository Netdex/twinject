#include "../stdafx.h"
#include "th15_patch_autobomb.h"

void th15_patch_autobomb::patch()
{
	HANDLE hProcess = GetCurrentProcess();
	WriteProcessMemory(hProcess, (void*)0x00454CC9, "\xC6", 1, NULL);
}

void th15_patch_autobomb::unpatch()
{
	HANDLE hProcess = GetCurrentProcess();
	WriteProcessMemory(hProcess, (void*)0x00454CC9, "\xF6", 1, NULL);
}
