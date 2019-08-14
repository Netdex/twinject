#include "../stdafx.h"
#include "th08_bullet_proc_hook.h"
#include "../util/detour.h"
#include "../control/th08_player.h"

th08_bullet_proc_hook* th08_bullet_proc_hook::instance = nullptr;

void th08_bullet_proc_hook::bind(th08_player* player)
{
	ASSERT(("cannot multi-bind", !instance));
	instance = new th08_bullet_proc_hook(player);
	Hook_TH08_sub_410A70();
}

th08_bullet_proc_hook* th08_bullet_proc_hook::inst()
{
	ASSERT(("cannot obtain unbounded instance", instance));
	return instance;
}

static sub_410A70_t sub_410A70_Original = (sub_410A70_t)0x00410A70;
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

	th08_bullet_proc_hook::vectorUpdateHook(addr, a1, a2, a3);

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
static void Hook_TH08_sub_410A70()
{
	if (DetourFunction(&(PVOID&)sub_410A70_Original, sub_410A70_Hook))
		SPDLOG_INFO("Detours: Hooked sub_410A70");
	else
		SPDLOG_ERROR("Detours: Failed to hook sub_410A70");
}

void th08_bullet_proc_hook::vectorUpdateHook(int retaddr, int a1, int a2, int a3)
{
	// HACK this might cause performance problems, also are we guaranteed a th08_player?
	th_player *player = inst()->player;
	ASSERT(("wrong player type bound to hook", player));
	std::vector<bullet> &TH08_Bullets = player->bullets;
	std::vector<powerup> &TH08_Powerups = player->powerups;

	// routine from bullet update
	if (retaddr == 0x004314B3)
	{
		vec2 sz(*(float*)(a2 + 3380), *((float*)(a2 + 3380) + 1));
		aabb a{
			vec2(*(float*)(a1 + 0), *(float*)(a1 + 4)) - sz / 2,
			vec2(*(float*)(a3 + 0),*(float*)(a3 + 4)),
			sz
		};
		bullet b{ a };
		// find log2 of bullet action binary flag
		if (!_BitScanReverse((DWORD*)&b.meta, *((DWORD*)a2 + 875)))
			b.meta = 0;

		TH08_Bullets.push_back(b);
	}
	else if (retaddr == 0x0044095B)
	{
		vec2 sz(10, 10);
		aabb a{
			vec2(*(float*)(a1 + 0), *(float*)(a1 + 4)) - sz / 2,
			vec2(*(float*)(a3 + 0), *(float*)(a3 + 4)),
			sz
		};
		powerup p{ a,  *(BYTE*)(a1 - 676 + 727) };

		TH08_Powerups.push_back(p);
	}
}