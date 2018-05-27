#pragma once
#include "th_hook.h"
#include "control/th07_player.h"

typedef int(__fastcall *sub_424290_t)(void *ecx);
int __fastcall sub_424290_Hook(void *ecx);
void Hook_TH07_sub_424290();

class th07_bullet_proc_hook : public th_hook
{
	static th07_bullet_proc_hook *instance;
public:
	th07_bullet_proc_hook(th07_player *player) : th_hook(player) {}

	static void bind(th07_player *player);
	static th07_bullet_proc_hook *inst();

	static void vector_update_hook(void *ecx);
};
