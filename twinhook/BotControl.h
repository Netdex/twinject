#pragma once

//#define BOT_RENDER_DETAILED

const float BOT_ACTION_THRESHOLD = 0.000005f;
const int BOT_MIN_ACTION_RANGE = 5;
const int BOT_DEFAULT_ACTION_RANGE = 60;

void Bot_SetEnable(BOOL enabled);
BOOL Bot_IsEnabled();
void Bot_ProcessControl(BYTE *diKeys);
void Bot_RenderOverlay(IDirect3DDevice9 *d3dDev);
void Bot_Tick();