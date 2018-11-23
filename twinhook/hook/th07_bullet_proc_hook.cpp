#include "../stdafx.h"
#include "th07_bullet_proc_hook.h"
#include "../util/detour.h"

th07_bullet_proc_hook *th07_bullet_proc_hook::instance = nullptr;

sub_424290_t sub_424290_Original = (sub_424290_t)0x00424290;
int __fastcall sub_424290_Hook(void *ecx)
{
	th07_bullet_proc_hook::vectorUpdateHook(ecx);
	return sub_424290_Original(ecx);
}

void Hook_TH07_sub_424290()
{
	if (DetourFunction(&(PVOID&)sub_424290_Original, sub_424290_Hook))
		LOG("Detours: Hooked sub_424290");
	else
		LOG("Detours: Failed to hook sub_424290");
}

void th07_bullet_proc_hook::bind(th07_player* player)
{
	ASSERT(("cannot multi-bind", !instance));
	instance = new th07_bullet_proc_hook(player);

	Hook_TH07_sub_424290();
}

th07_bullet_proc_hook* th07_bullet_proc_hook::inst()
{
	ASSERT(("cannot obtain unbounded instance", instance));
	return instance;
}

void th07_bullet_proc_hook::vectorUpdateHook(void *ecx)
{
	th07_player *player = dynamic_cast<th07_player*>(inst()->player);
	ASSERT(("wrong player type bound to hook", player));

	std::vector<bullet> &bullets = player->bullets;
	std::vector<powerup> &powerups = player->powerups;

	// TODO something is quite wrong with the bullet size
	aabb bulletBounds{
		vec2(*(float*)((char*)ecx + 0xB8C), *((float*)((char*)ecx + 0xB8C) + 1)),
		vec2(*(float*)((char*)ecx + 0xB98), *((float*)((char*)ecx + 0xB98) + 1)),
		vec2(*(float*)(*(int*)((char*)ecx + 0x1E4) + 48) / 2, *(float*)(*(int*)((char*)ecx + 0x1E4) + 44) / 2)
	};
	bullet b{ bulletBounds };
	bullets.push_back(b);
}
