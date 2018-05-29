#pragma once
#define _D BotOverlayRenderer_DisplayDebugString
#define _B BotOverlayRenderer_BeginDebugString

void BotOverlayRenderer_BeginDebugString();

// BUG this code actually uses a ridiculous amount of computing power
// render text to a double-buffer then blit onto screen instead
// or use ID3DXSprite
void BotOverlayRenderer_DisplayDebugString(D3DCOLOR color, const char* fmt, ...);