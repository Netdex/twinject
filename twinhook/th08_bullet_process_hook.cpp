#include "stdafx.h"
#include "th08_bullet_process_hook.h"
#include "detour.h"
#include "TH08Control.h"

void th08_bullet_process_hook::hook()
{
	Hook_TH08_sub_410A70();
}

/*
* Hook of bullet/object processing code
*/

PBYTE pSub_410A70 = (PBYTE)0x00410A70;
sub_410A70_t sub_410A70_Original = (sub_410A70_t)pSub_410A70;
__declspec(naked) int __fastcall sub_410A70_Hook(int a1, int a2, int a3)
{
	// this is why you need MSVC to compile this
	__asm {
		// get return address, put it somewhere safe
		mov eax, [esp]
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		// move __fastcall arguments into function parameters
		mov a1, ecx
		mov a2, edx
		mov ecx, [ebp + 8]
		mov a3, ecx
	}

	unsigned int addr;
	__asm mov addr, eax

	TH08_VectorUpdate_Hook(addr, a1, a2, a3);

	// return original value
	int retval;
	retval = sub_410A70_Original(a1, a2, a3);
	__asm {
		mov eax, retval

		mov esp, ebp
		pop ebp
		ret
	}
}
void Hook_TH08_sub_410A70()
{
	if (DetourFunction(&(PVOID&)sub_410A70_Original, sub_410A70_Hook))
		LOG("Detours: Hooked sub_410A70");
	else
		LOG("Detours: Failed to hook sub_410A70");
}