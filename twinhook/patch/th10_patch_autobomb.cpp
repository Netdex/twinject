#include "../stdafx.h"
#include "th10_patch_autobomb.h"

void th10_patch_autobomb::patch()
{
	HANDLE hProcess = GetCurrentProcess();
	WriteProcessMemory(hProcess, (void*)0x00425C13,
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",
		13, NULL);
}

void th10_patch_autobomb::unpatch()
{
	HANDLE hProcess = GetCurrentProcess();
	WriteProcessMemory(hProcess, (void*)0x00425C13,
		"\xF6\x05\x5C\x4E\x47\x00\x02\x0F\x84\x52\x01\x00\x00",
		13, NULL);
}
