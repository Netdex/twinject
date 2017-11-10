#include "stdafx.h"
#include "BotOverlayRenderer.h"
#include "cdraw.h"
#include "TH08Control.h"
#include "color.h"
#include "BotControl.h"
#include "InputOverlayRenderer.h"

#define D BotOverlayRenderer_DisplayDebugString
#define B BotOverlayRenderer_BeginDebugString

extern std::vector<entity> TH08_Bullets;
extern std::vector<entity> TH08_Powerups;

extern bool BotControl_FlagRenderDetailed;
extern float Bot_ProjectionTransform(float n);

static char BotOverlayRenderer_StringBuffer[256];
static int BotOverlayRenderer_DebugLineOffset = 0;

static void BotOverlayRenderer_BeginDebugString()
{
	CDraw_FillRect(445, 250, 640, 480, D3DCOLOR_ARGB(200, 0, 0, 0));
	BotOverlayRenderer_DebugLineOffset = 0;
}
static void BotOverlayRenderer_DisplayDebugString(D3DCOLOR color, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf_s(BotOverlayRenderer_StringBuffer, 256, fmt, args);
	CDraw_Text(BotOverlayRenderer_StringBuffer, color,
		450, 255 + 15 * BotOverlayRenderer_DebugLineOffset, WINDOW_WIDTH, WINDOW_HEIGHT);
	va_end(args);
	BotOverlayRenderer_DebugLineOffset++;
}

void Render_BotOverlay(IDirect3DDevice9 *d3dDev)
{
	vec2 plyr = TH08_GetPlayerLocation();
	vec2 boss = TH08_GetBossPosition();

	B();
	D(D3DCOLOR_ARGB(255, 0, 255, 255), "TWINJECT [netdex]");
	D(D3DCOLOR_ARGB(255, 255, 255, 255), "BULLET_COUNT: %d", TH08_Bullets.size());
	D(D3DCOLOR_ARGB(255, 255, 255, 255), "   BOT_STATE: %s", Bot_IsEnabled() ? "ENABLED" : "DISABLED");
	D(D3DCOLOR_ARGB(255, 255, 255, 255), "         VIZ: %s", BotControl_FlagRenderDetailed ? "DETAILED" : "NONE");

	if (BotControl_FlagRenderDetailed) {
		// bullet markers
		for (auto i = TH08_Bullets.begin(); i != TH08_Bullets.end(); ++i)
		{
			if ((*i).me)
			{
				CDraw_Rect((*i).p.x - 7 + GAME_X_OFFSET, (*i).p.y - 7 + GAME_Y_OFFSET, 14, 14, D3DCOLOR_HSV((double)(16 * (*i).me), 1, 1)));
			}
			else {
				CDraw_Rect((*i).p.x - (*i).sz.x / 2 + GAME_X_OFFSET, (*i).p.y - (*i).sz.x / 2 + GAME_Y_OFFSET, (*i).sz.x, (*i).sz.y, D3DCOLOR_ARGB(255, 255, 2, 200));
			}
			vec2 proj = (*i).p + (*i).v.transform(Bot_ProjectionTransform);

			CDraw_Line((*i).p.x + GAME_X_OFFSET, (*i).p.y + GAME_Y_OFFSET,
				proj.x + GAME_X_OFFSET, proj.y + GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
		}

		// powerup markers
		/*for (auto i = TH08_Powerups.begin(); i != TH08_Powerups.end(); ++i)
		{
			if ((*i).me == 0) {
				CDraw_FillRect((*i).p.x - 2 + GAME_X_OFFSET, (*i).p.y - 2 + GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 2, 255, 200));
				CDraw_Line((*i).p.x + GAME_X_OFFSET, (*i).p.y + GAME_Y_OFFSET,
					(*i).p.x + (*i).v.x * 5 + GAME_X_OFFSET, (*i).p.y + (*i).v.y * 5 + GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
			}
		}*/

		// player area

		CDraw_FillRect(plyr.x - 2 + GAME_X_OFFSET, plyr.y - 2 + GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 0, 255, 0));

		CDraw_Line(boss.x + GAME_X_OFFSET, 0, boss.x + GAME_X_OFFSET, WINDOW_HEIGHT, D3DCOLOR_ARGB(255, 255, 0, 0));
	}

	Overlay_RenderInput(d3dDev);
}
