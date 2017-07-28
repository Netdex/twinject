#include "stdafx.h"
#include "TH08Control.h"

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
		b.v.x = *(float*)(a3 + 0);
		b.v.y = *(float*)(a3 + 4);
		// find log2 of bullet action binary flag
		if(!_BitScanReverse(&b.me, *((DWORD*)a2 + 875)))
			b.me = 0;
		
		float bx = *(float*)(a2 + 3380);
		float by = *((float*)(a2 + 3380) + 1);
		b.sz = vec2(bx, by);
		TH08_Bullets.push_back(b);
	}
	else if(retaddr == 0x0044095B)
	{
		entity b;
		b.p.x = *(float*)(a1 + 0);
		b.p.y = *(float*)(a1 + 4);
		b.v.x = *(float*)(a3 + 0);
		b.v.y = *(float*)(a3 + 4);
		// get powerup temporal factor
		b.me = *(BYTE*)(a1 - 676 + 727);
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

vec2 TH08_GetPlayerLocation()
{
	return vec2(*(float*)PlayerPosAddr - GAME_X_OFFSET, *(float*)(PlayerPosAddr + 4) - GAME_Y_OFFSET);
}

