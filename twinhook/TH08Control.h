#pragma once

struct entity
{
	D3DVECTOR p;
	D3DVECTOR v;
	DWORD bh;			// bullet behavior
	BYTE pt;			// powerup type
};

void TH08Control_FrameInit();
void TH08Control_FrameCleanup();

void TH08_VectorUpdate_Hook(int retaddr, int a1, int a2, int a3);
D3DVECTOR TH08_GetPlayerLocation();