#include "../stdafx.h"
#include "th_info_overlay.h"
#include "util/cdraw.h"
#include "config/th_config.h"

static char BotOverlayRenderer_StringBuffer[256];
static int BotOverlayRenderer_DebugLineOffset = 0;

void BotOverlayRenderer_BeginDebugString()
{
	cdraw::fillRect(th_param.WINDOW_WIDTH - (650 - 445), th_param.WINDOW_HEIGHT - (480 - 250),
		th_param.WINDOW_WIDTH, th_param.WINDOW_HEIGHT, D3DCOLOR_ARGB(200, 0, 0, 0));
	BotOverlayRenderer_DebugLineOffset = 0;
}

// BUG this code actually uses a ridiculous amount of computing power
// render text to a double-buffer then blit onto screen instead
// or use ID3DXSprite
void BotOverlayRenderer_DisplayDebugString(D3DCOLOR color, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf_s(BotOverlayRenderer_StringBuffer, 256, fmt, args);
	cdraw::text(BotOverlayRenderer_StringBuffer, color,
		(int)th_param.WINDOW_WIDTH - (640 - 450), (int)th_param.WINDOW_HEIGHT - (480 - 255) + 15 * BotOverlayRenderer_DebugLineOffset);
	va_end(args);
	BotOverlayRenderer_DebugLineOffset++;
}