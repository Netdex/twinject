#pragma once
#include "vec2.h"

struct entity
{
	vec2 p;
	vec2 v;
	vec2 sz;			// bullet size
	DWORD me;			// metadata
};

void TH08Control_FrameInit();
void TH08Control_FrameCleanup();

void TH08_VectorUpdate_Hook(int retaddr, int a1, int a2, int a3);

/**
 * \brief Get the player's position
 * \return The player's position relative to game frame
 */
vec2 TH08_GetPlayerLocation();

/**
 * \brief Get the main boss' position (y-coordinate is not correct!)
 * \return Main boss' position relative to game frame
 */
vec2 TH08_GetBossPosition();