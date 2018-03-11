#include "stdafx.h"
#include "th15_bullet_proc_hook.h"
#include "detour.h"
#include "th_config.h"
#include <emmintrin.h>

th15_bullet_proc_hook* th15_bullet_proc_hook::instance = nullptr;

void th15_bullet_proc_hook::bind(th15_player* player)
{
	assert(("cannot multi-bind", !instance));
	instance = new th15_bullet_proc_hook(player);
	hook_th15_sub_455D00();
	hook_th15_sub_455E10();
}

th15_bullet_proc_hook* th15_bullet_proc_hook::inst()
{
	assert(("cannot obtain unbounded instance", instance));
	return instance;
}

static sub_455D00_t sub_455D00_Original = (sub_455D00_t)0x00455D00;

// signed int __userpurge _col_chk@<eax>(float fRadius@<xmm2>, int pPos, int a3)
// recycle local variables passed by caller
int __declspec(naked) __stdcall sub_455D00_hook(int pPos, int a3)
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
			vec2(*(float*)pPos + th_param.GAME_WIDTH / 2, *(float*)(pPos + 4)),
			vec2(*(float*)(pPos + (3140 - 3128)), *(float*)(pPos + (3144 - 3128))),
			vec2(fRadius, fRadius),
			0
		};
		th15_bullet_proc_hook::inst()->player->bullets.push_back(e);
	}
	__asm {
		// userpurge<eax>(float<xmm2>, int, int) styled call to original function
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

void hook_th15_sub_455D00()
{
	if (DetourFunction(&(PVOID&)sub_455D00_Original, sub_455D00_hook))
		LOG("Detours: Hooked sub_455D00");
	else
		LOG("Detours: Failed to hook sub_455D00");
}

static sub_455E10_t sub_455E10_Original = (sub_455E10_t)0x00455E10;

int __declspec(naked) __stdcall sub_455E10_hook(float* a3, float a4, int a5)
{
	// 16-byte aligned
	__m128 a1;
	__m128 a2;
	float angle;
	float rad;

	__asm {
		// stdcall styled prolog w/ user defined SSE
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		movss angle, xmm2
		movss rad, xmm3
		movups a1, xmm2
		movups a2, xmm3
	}
	{
		laser e = {
			vec2(a3[0] + th_param.GAME_WIDTH / 2, a3[1]),			// position x y
			vec2(), vec2(a4 * cos(angle), a4 * sin(angle)),
			rad/2.f, angle
		};
		th15_bullet_proc_hook::inst()->player->lasers.push_back(e);
	}
	__asm {
		// userpurge<eax>(float<xmm2>, float<xmm3>, float, float, int) 
		// styled call to original function
		movups xmm2, a1
		movups xmm3, a2
		push a5
		push a4
		push a3
		call sub_455E10_Original

		// stdcall styled epilog
		mov esp, ebp
		pop ebp
		ret 0Ch					// clean up, since it is userpurge
	}
}

void hook_th15_sub_455E10()
{
	if (DetourFunction(&(PVOID&)sub_455E10_Original, sub_455E10_hook))
		LOG("Detours: Hooked sub_455E10");
	else
		LOG("Detours: Failed to hook sub_455E10");
}
