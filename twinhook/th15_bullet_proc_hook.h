#pragma once
#include "th_hook.h"
#include "th15_player.h"


typedef signed int (__stdcall *sub_455D00_t)(int a1, int a2, int a3);

// signed int __userpurge _col_chk@<eax>(float fRadius@<xmm2>, int pPos, int a3)
int __stdcall sub_455D00_Hook(int pPos, int a3);
void Hook_TH15_sub_455D00();



class th15_bullet_proc_hook : public th_hook
{
	static th15_bullet_proc_hook *instance;
public:
	th15_bullet_proc_hook(th15_player *player) : th_hook(player) {}

	static void bind(th15_player *player);
	static th15_bullet_proc_hook *inst();
};
