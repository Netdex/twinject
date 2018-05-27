#pragma once
#include "th_hook.h"
#include "../control/th15_player.h"


typedef signed int (__stdcall *sub_455D00_t)(int a1, int a2, int a3);
typedef signed int (__stdcall *sub_455E10_t)(float *a3, float a4, int a5);

// signed int __userpurge _col_chk@<eax>(float fRadius@<xmm2>, int pPos, int a3)
int __stdcall sub_455D00_hook(int pPos, int a3);
// signed int __userpurge _laser_col_chk@<eax>(__m128 a1@<xmm2>, float a2@<xmm3>, 
//													float *a3, float a4, int a5)
int __stdcall sub_455E10_hook(float *a3, float a4, int a5);

void hook_th15_sub_455D00();
void hook_th15_sub_455E10();

class th15_bullet_proc_hook : public th_hook
{
	static th15_bullet_proc_hook *instance;
public:
	th15_bullet_proc_hook(th15_player *player) : th_hook(player) {}

	static void bind(th15_player *player);
	static th15_bullet_proc_hook *inst();
};
