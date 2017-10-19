#pragma once


const float BOT_ACTION_THRESHOLD = 0.000005f;		// 0.000005
const float BOT_RADIUS = 4;							// 4
const float BOT_MACROSAFETY_DELTA = 4;				// 4
const float BOT_MICROSAFETY_DELTA = 5;				// 5
const float BULLET_PROJECTION_FACTOR = 20;			// 20
const float FOCUS_FORCE_THRESHOLD = 0.05f;			// 0.05
const float BOT_BULLET_PRIORITY = 10;				// 10				

void Bot_SetEnable(BOOL enabled);
bool Bot_IsEnabled();
void Bot_ProcessControl(BYTE *diKeys);
void Bot_Tick();