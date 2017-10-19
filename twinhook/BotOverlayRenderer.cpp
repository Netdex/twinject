#include "stdafx.h"
#include "BotOverlayRenderer.h"
#include "cdraw.h"
#include "TH08Control.h"
#include "color.h"
#include "BotControl.h"
#include "InputOverlayRenderer.h"

extern std::vector<entity> TH08_Bullets;
extern std::vector<entity> TH08_Powerups;

extern bool bRenderDetailed;

void Render_BotOverlay(IDirect3DDevice9 *d3dDev)
{
	// simple credits and stats
	char buf[256];
	CDraw_FillRect(445, 250, 640, 480, D3DCOLOR_ARGB(200, 0, 0, 0));
	CDraw_Text("TWINJECT [netdex]", D3DCOLOR_ARGB(255, 0, 255, 255), 450, 255, 640, 480);
	sprintf_s(buf, 256, "BULLET_COUNT: %d", TH08_Bullets.size());
	CDraw_Text(buf, D3DCOLOR_ARGB(255, 255, 255, 255), 450, 285, 640, 480);
	sprintf_s(buf, 256, "BOT_STATE: %s", Bot_IsEnabled() ? "ENABLED" : "DISABLED");
	CDraw_Text(buf, D3DCOLOR_ARGB(255, 255, 255, 255), 450, 300, 640, 480);

	if (bRenderDetailed) {
		// bullet markers
		for (auto i = TH08_Bullets.begin(); i != TH08_Bullets.end(); ++i)
		{
			CDraw_Rect((*i).p.x - (*i).sz.x / 2 + GAME_X_OFFSET, (*i).p.y - (*i).sz.x / 2 + GAME_Y_OFFSET, (*i).sz.x, (*i).sz.y, D3DCOLOR_ARGB(255, 255, 2, 200));
			CDraw_Line((*i).p.x + GAME_X_OFFSET, (*i).p.y + GAME_Y_OFFSET,
				(*i).p.x + (*i).v.x * BULLET_PROJECTION_FACTOR + GAME_X_OFFSET, (*i).p.y + (*i).v.y * BULLET_PROJECTION_FACTOR + GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));

			if ((*i).me)
			{
				CDraw_Rect((*i).p.x - 7 + GAME_X_OFFSET, (*i).p.y - 7 + GAME_Y_OFFSET, 14, 14, D3DCOLOR_HSV((double)(16 * (*i).me), 1, 1)));
			}
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
	}
	// player area
	vec2 plyr = TH08_GetPlayerLocation();
	CDraw_FillRect(plyr.x - 2 + GAME_X_OFFSET, plyr.y - 2 + GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 0, 255, 0));

	Overlay_RenderInput(d3dDev);
}
