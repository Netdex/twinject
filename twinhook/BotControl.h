#pragma once


const float BOT_ACTION_THRESHOLD = 0.000009f;
const int BOT_MIN_ACTION_RANGE = 7;
const int BOT_DEFAULT_ACTION_RANGE = 20;
const int BOT_SIDESTEP_RANGE = 12;
const float BULLET_PROJECTION_FACTOR = 10;
const float FOCUS_FORCE_THRESHOLD = 0.005f;
const float GATHER_THRESHOLD = 0.00005f;

void Bot_SetEnable(BOOL enabled);
BOOL Bot_IsEnabled();
void Bot_ProcessControl(BYTE *diKeys);
void Bot_RenderOverlay(IDirect3DDevice9 *d3dDev);
void Bot_Tick();