#include "stdafx.h"
#include "BulletProcessHook.h"
#include "detour.h"

/*
 * Hook of bullet/object processing code
 */

std::vector<bullet> bullets;


PBYTE pSub_410A70 = (PBYTE)0x00410A70;
sub_431240_t sub_431240_Original = (sub_431240_t)pSub_410A70;
__declspec(naked) int __fastcall sub_431240_Hook(int a1, int a2, int a3)
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

	// routine from bullet update
	if (addr == 0x004314B3)
	{
		bullet b;
		b.x = *(float*)(a1 + 0);
		b.y = *(float*)(a1 + 4);
		b.vx = *(float*)(a3 + 0);
		b.vy = *(float*)(a3 + 4);
		bullets.push_back(b);
	}

	int retval;
	retval = sub_431240_Original(a1, a2, a3);
	__asm {
		mov eax, retval

		mov esp, ebp
		pop ebp
		ret
	}
}
void Hook_TH08_sub_410A70()
{
	if (DetourFunction(&(PVOID&)sub_431240_Original, sub_431240_Hook))
		logfs("Detours: Hooked sub_431240");
	else
		logfs("Detours: Failed to hook sub_431240");
}