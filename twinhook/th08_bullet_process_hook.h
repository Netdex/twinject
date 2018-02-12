#pragma once
#include "th_hook.h"

typedef int(__fastcall *sub_410A70_t)(int a1, int a2, int a3);
int __fastcall sub_410A70_Hook(int a1, int a2, int a3);
void Hook_TH08_sub_410A70();

class th08_bullet_process_hook : th_hook
{
public:
	th08_bullet_process_hook(th_player *player) : th_hook(player) {}

	void hook() override;
};
