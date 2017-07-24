#include "stdafx.h"
#include "TH08Control.h"
#include "DI8Control.h"

std::vector<entity> TH08_Bullets;
std::vector<entity> TH08_Powerups;

void TH08_VectorUpdate_Hook(int retaddr, int a1, int a2, int a3)
{
	// routine from bullet update
	if (retaddr == 0x004314B3)
	{
		entity b;
		b.p.x = *(float*)(a1 + 0);
		b.p.y = *(float*)(a1 + 4);
		b.p.z = *(float*)(a1 + 8);
		b.v.x = *(float*)(a3 + 0);
		b.v.y = *(float*)(a3 + 4);
		b.v.z = *(float*)(a3 + 8);
		// find log2 of bullet action binary flag
		if(!_BitScanReverse(&b.bh, *((DWORD*)a2 + 875)))
			b.bh = 0;
		TH08_Bullets.push_back(b);
	}
	else if(retaddr == 0x0044095B)
	{
		entity b;
		b.p.x = *(float*)(a1 + 0);
		b.p.y = *(float*)(a1 + 4);
		b.p.z = *(float*)(a1 + 8);
		b.v.x = *(float*)(a3 + 0);
		b.v.y = *(float*)(a3 + 4);
		b.v.z = *(float*)(a3 + 8);
		// get powerup temporal factor
		b.pt = *(BYTE*)(a1 - 676 + 727);
		TH08_Powerups.push_back(b);
	}
}

void TH08Control_FrameInit()
{
	
}

void TH08Control_FrameCleanup()
{
	TH08_Bullets.clear();
	TH08_Powerups.clear();
	
}

PBYTE PlayerPosAddr = (PBYTE)0x017D6110;

D3DVECTOR TH08_GetPlayerLocation()
{
	D3DVECTOR pos{};
	pos.x = *(float*)PlayerPosAddr - GAME_X_OFFSET;
	pos.y = *(float*)(PlayerPosAddr + 4) - GAME_Y_OFFSET;
	return pos;
}

