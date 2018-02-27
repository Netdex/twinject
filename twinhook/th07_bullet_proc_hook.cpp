#include "stdafx.h"
#include "th07_bullet_proc_hook.h"
#include "detour.h"

th07_bullet_proc_hook *th07_bullet_proc_hook::instance = nullptr;

sub_424290_t sub_424290_Original = (sub_424290_t)0x00424290;
int __fastcall sub_424290_Hook(void *ecx)
{
	th07_bullet_proc_hook::vector_update_hook(ecx);
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
	assert(("cannot multi-bind", !instance));
	instance = new th07_bullet_proc_hook(player);

	Hook_TH07_sub_424290();
}

th07_bullet_proc_hook* th07_bullet_proc_hook::inst()
{
	assert(("cannot obtain unbounded instance", instance));
	return instance;
}

void th07_bullet_proc_hook::vector_update_hook(void *ecx)
{
	th07_player *player = dynamic_cast<th07_player*>(inst()->player);
	assert(("wrong player type bound to hook", player));

	std::vector<entity> &bullets = player->bullets;
	std::vector<entity> &powerups = player->powerups;

	// TODO something is quite wrong with the bullet size
	entity e = {
		{ *(float*)((char*)ecx + 0xB8C), *((float*)((char*)ecx + 0xB8C) + 1) },
		{ *(float*)((char*)ecx + 0xB98), *((float*)((char*)ecx + 0xB98) + 1) },
		{ *(float*)(*(int*)((char*)ecx + 0x1E4) + 48), *(float*)(*(int*)((char*)ecx + 0x1E4) + 44) },
		static_cast<DWORD>(*(int*)((char*)ecx + 0xBF4))
	};
	bullets.push_back(e);
}
