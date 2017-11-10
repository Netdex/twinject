#pragma once


const float BOT_ACTION_THRESHOLD = 0.000005f;		// 0.000005
const float BOT_MIN_RADIUS = 5;							// 4
const float BOT_MACROSAFETY_DELTA = 2;				// 4
const float BOT_MICROSAFETY_DELTA = 4;				// 5

const float BOT_BULLET_PROJECTION_FACTOR = 10.f;			// 20

const float BOT_BULLET_PRIORITY = 20.f;				// 10				
const float BOT_POWERUP_PRIORITY = 0.1f;			// 0.1
const float BOT_BOSS_TARGET_PRIORITY = 500000000.f;		// 100000000

const float BOT_POWERUP_MAXY = 300;
const float BOT_POWERUP_MINY = 70;

const float FOCUS_FORCE_THRESHOLD = 1.f;			// 0.05

void Bot_SetEnable(BOOL enabled);
bool Bot_IsEnabled();
void Bot_ProcessControl(BYTE *diKeys);
void Bot_Tick();