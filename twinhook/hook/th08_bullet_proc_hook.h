#pragma once
#include "th_hook.h"
#include "../control/th08_player.h"

typedef int(__fastcall *sub_410A70_t)(int a1, int a2, int a3);
int __fastcall sub_410A70_Hook(int a1, int a2, int a3);
void Hook_TH08_sub_410A70();

class th08_bullet_proc_hook : public th_hook
{
	static th08_bullet_proc_hook *instance;
public:
	th08_bullet_proc_hook(th08_player *player) : th_hook(player) {}

	static void bind(th08_player *player);
	static th08_bullet_proc_hook *inst();
	
	static void vector_update_hook(int retaddr, int a1, int a2, int a3);
};
