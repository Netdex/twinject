#pragma once
struct bullet
{
	float x, y, vx, vy;
};
typedef int(__fastcall *sub_431240_t)(int a1, int a2, int a3);
int __fastcall sub_431240_Hook(int a1, int a2, int a3);
void Hook_TH08_sub_410A70();