#pragma once
#include "vec2.h"

struct entity
{
	vec2 p;
	vec2 v;
	DWORD bh;			// bullet behavior
	BYTE pt;			// powerup type
	int vt;				// temporary value
};

void TH08Control_FrameInit();
void TH08Control_FrameCleanup();

void TH08_VectorUpdate_Hook(int retaddr, int a1, int a2, int a3);
vec2 TH08_GetPlayerLocation();