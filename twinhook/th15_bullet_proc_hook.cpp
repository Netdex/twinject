#include "stdafx.h"
#include "th15_bullet_proc_hook.h"
#include "detour.h"
#include "th_config.h"

th15_bullet_proc_hook* th15_bullet_proc_hook::instance = nullptr;

void th15_bullet_proc_hook::bind(th15_player* player)
{
	assert(("cannot multi-bind", !instance));
	instance = new th15_bullet_proc_hook(player);
	Hook_TH15_sub_455D00();
}

th15_bullet_proc_hook* th15_bullet_proc_hook::inst()
{
	assert(("cannot obtain unbounded instance", instance));
	return instance;
}

static sub_455D00_t sub_455D00_Original = (sub_455D00_t)0x00455D00;

// signed int __userpurge _col_chk@<eax>(float fRadius@<xmm2>, int pPos, int a3)
// recycle local variables passed by caller
int __declspec(naked) __stdcall sub_455D00_Hook(int pPos, int a3)
{
	// declare storage for XMM2 register
	float fRadius;
	__asm {
		// stdcall styled prolog w/ user defined SSE
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		movss fRadius, xmm2
	}
	{
		entity e = {
			// for some bizarre reason, the x-coordinate is offset by half the screen width,
			// such that the middle of the screen is 0
			vec2(*(float*)pPos + th_param::GAME_WIDTH / 2, *(float*)(pPos + 4)),
			vec2(*(float*)(pPos + (3140 - 3128)), *(float*)(pPos + (3144 - 3128))),
			vec2(fRadius, fRadius),
			0
		};
		th15_bullet_proc_hook::inst()->player->bullets.push_back(e);
	}
	__asm {
		// userpurge<eax>(float<xm22>, int, int) styled call to original function
		movss xmm2, fRadius
		push a3
		push pPos
		call sub_455D00_Original

		// stdcall styled epilog
		mov esp, ebp
		pop ebp
		ret 8					// clean up, since it is userpurge
	}
}

void Hook_TH15_sub_455D00()
{
	if (DetourFunction(&(PVOID&)sub_455D00_Original, sub_455D00_Hook))
		LOG("Detours: Hooked sub_455D00");
	else
		LOG("Detours: Failed to hook sub_455D00");
}
