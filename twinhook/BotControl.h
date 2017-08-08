#pragma once


const float BOT_ACTION_THRESHOLD = 0.000005f;
const float BOT_RADIUS = 5;
const float BOT_MACROSAFETY_DELTA = 3;
const float BOT_MICROSAFETY_DELTA = 4;
const float BULLET_PROJECTION_FACTOR = 20;
const float FOCUS_FORCE_THRESHOLD = 0.05f;
const float BOT_BULLET_PRIORITY = 10;

void Bot_SetEnable(BOOL enabled);
BOOL Bot_IsEnabled();
void Bot_ProcessControl(BYTE *diKeys);
void Bot_RenderOverlay(IDirect3DDevice9 *d3dDev);
void Bot_Tick();