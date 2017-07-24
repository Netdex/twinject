#include "stdafx.h"
#include "BotControl.h"
#include "TH08Control.h"
#include "IDI8A_Wrapper.h"
#include "DI8Control.h"
#include "cdraw.h"
#include "color.h"
#include "AutoBombPatch.h"

extern HANDLE ProcessHandle;

extern DirectInput8Wrapper *DirectInput8;

extern std::vector<entity> TH08_Bullets;
extern std::vector<entity> TH08_Powerups;

BOOL bBotEnabled = FALSE;

float actionRange = BOT_DEFAULT_ACTION_RANGE;

void Bot_OnEnable()
{
	AutoBomb_Patch(ProcessHandle);
}

void Bot_OnDisable()
{
	AutoBomb_Unpatch(ProcessHandle);
}

void Bot_SetEnable(BOOL enable)
{
	bBotEnabled = enable;
	if(bBotEnabled)
	{
		Bot_OnEnable();
	}
	else
	{
		Bot_OnDisable();
	}
}

BOOL Bot_IsEnabled()
{
	return bBotEnabled;
}

D3DVECTOR CalculateNetVector(D3DVECTOR ship)
{
	D3DVECTOR net{};
	bool shrinkRange = false;
	for (auto i = TH08_Bullets.begin(); i != TH08_Bullets.end(); ++i)
	{
		float mx = (*i).p.x - ship.x;
		float my = (*i).p.y - ship.y;
		// check if bullet is inside range
		if (mx * mx + my * my < actionRange * actionRange ||
			(mx + (*i).v.x) * (mx + (*i).v.x) + (my + (*i).v.y) * (my + (*i).v.y) < actionRange * actionRange) {
			shrinkRange = true;
			float lsq = abs(pow(mx, 4)) + abs(pow(my, 4));
			float inv = 448 / lsq;
			net.x += mx * inv;
			net.y += my * inv;
		}
	}
	for (auto i = TH08_Powerups.begin(); i != TH08_Powerups.end(); ++i)
	{
		// make sure this powerup isn't one of those score particles
		if ((*i).pt == 0) {
			float mx = (*i).p.x - ship.x;
			float my = (*i).p.y - ship.y;
			if (abs(mx) < 200 && abs(my) < 70) {
				float lsq = abs(pow(mx, 2)) + abs(pow(my, 2));
				float inv = .1f / lsq;
				net.x -= mx * inv;
				net.y -= my * inv;
			}
		}
	}

	if (shrinkRange)
	{
		actionRange = (actionRange * 9 + BOT_MIN_ACTION_RANGE) / 10;
	}
	else
	{
		actionRange = (actionRange * 15 + BOT_DEFAULT_ACTION_RANGE) / 16;
	}
	// calculate wall repulsion
	float dxr = abs(pow(GAME_WIDTH - ship.x, 2));
	float dxl = abs(pow(ship.x, 2));
	float dyt = abs(pow(ship.y, 2));
	float dyb = abs(pow(GAME_HEIGHT - ship.y, 2));
	net.x += 2 / dxr + -2 / dxl;
	net.y += -5 / dyt + 1 / dyb;
	return net;
}

void Bot_RenderOverlay(IDirect3DDevice9 *d3dDev)
{
	// simple credits and stats
	char buf[256];
	CDraw_FillRect(d3dDev, 445, 250, 640, 480, D3DCOLOR_ARGB(200, 0, 0, 0));
	CDraw_Text("TWINJECT [netdex]", D3DCOLOR_ARGB(255, 0, 255, 255), 450, 255, 640, 480);
	sprintf_s(buf, 256, "BULLET_COUNT: %d", TH08_Bullets.size());
	CDraw_Text(buf, D3DCOLOR_ARGB(255, 255, 255, 255), 450, 285, 640, 480);
	sprintf_s(buf, 256, "BOT_STATE: %s", Bot_IsEnabled() ? "ENABLED" : "DISABLED");
	CDraw_Text(buf, D3DCOLOR_ARGB(255, 255, 255, 255), 450, 300, 640, 480);

#ifdef BOT_RENDER_DETAILED
	// bullet markers
	for (auto i = TH08_Bullets.begin(); i != TH08_Bullets.end(); ++i)
	{
		CDraw_FillRect(d3dDev, (*i).p.x - 2 + GAME_X_OFFSET, (*i).p.y - 2 + GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 255, 2, 200));
		CDraw_Line((*i).p.x + GAME_X_OFFSET, (*i).p.y + GAME_Y_OFFSET,
			(*i).p.x + (*i).v.x * 5 + GAME_X_OFFSET, (*i).p.y + (*i).v.y * 5 + GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
		if((*i).bh)
		{
			CDraw_Rect((*i).p.x - 7 + GAME_X_OFFSET, (*i).p.y - 7 + GAME_Y_OFFSET, 14, 14, D3DCOLOR_HSV((double)(16 * (*i).bh), 1, 1)));
		}
	}
	// powerup markers
	for (auto i = TH08_Powerups.begin(); i != TH08_Powerups.end(); ++i)
	{
		CDraw_FillRect(d3dDev, (*i).p.x - 2 + GAME_X_OFFSET, (*i).p.y - 2 + GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 2, 255, 200));
		CDraw_Line((*i).p.x + GAME_X_OFFSET, (*i).p.y + GAME_Y_OFFSET,
			(*i).p.x + (*i).v.x * 5 + GAME_X_OFFSET, (*i).p.y + (*i).v.y * 5 + GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
	}
#endif
	// player area
	D3DVECTOR plyr = TH08_GetPlayerLocation();
	CDraw_FillRect(d3dDev, plyr.x - 2 + GAME_X_OFFSET, plyr.y - 2 + GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 0, 255, 0));
	CDraw_Circle(plyr.x + GAME_X_OFFSET, plyr.y + GAME_Y_OFFSET, actionRange, 20, D3DCOLOR_ARGB(255, 255, 0, 0));
}

void Bot_ProcessControl(BYTE *diKeys)
{
	if (diKeys[DIK_G] & 0x80)
		Bot_SetEnable(true);
	if (diKeys[DIK_B] & 0x80)
		Bot_SetEnable(false);
}

void Bot_Tick()
{
	if (!bBotEnabled) {
		DI8C_ResetKeyState(DIK_LEFT);
		DI8C_ResetKeyState(DIK_RIGHT);
		DI8C_ResetKeyState(DIK_UP);
		DI8C_ResetKeyState(DIK_DOWN);
		DI8C_ResetKeyState(DIK_Z);
		DI8C_ResetKeyState(DIK_X);
		return;
	}
	D3DVECTOR plyr = TH08_GetPlayerLocation();
	D3DVECTOR net = CalculateNetVector(plyr);

	DI8C_SetKeyState(DIK_Z, 0x80);
	if (abs(net.x) > BOT_ACTION_THRESHOLD)
	{
		if (net.x > 0) {
			DI8C_SetKeyState(DIK_LEFT, 0x80);
			DI8C_SetKeyState(DIK_RIGHT, 0);
		}
		else
		{
			DI8C_SetKeyState(DIK_RIGHT, 0x80);
			DI8C_SetKeyState(DIK_LEFT, 0);
		}
	}
	else
	{
		DI8C_SetKeyState(DIK_RIGHT, 0);
		DI8C_SetKeyState(DIK_LEFT, 0);
	}
	if (abs(net.y) > BOT_ACTION_THRESHOLD)
	{
		if (net.y > 0) {
			DI8C_SetKeyState(DIK_UP, 0x80);
			DI8C_SetKeyState(DIK_DOWN, 0);
		}
		else
		{
			DI8C_SetKeyState(DIK_DOWN, 0x80);
			DI8C_SetKeyState(DIK_UP, 0);
		}
	}
	else
	{
		DI8C_SetKeyState(DIK_DOWN, 0);
		DI8C_SetKeyState(DIK_UP, 0);
	}
	DI8C_SetKeyState(DIK_X, 0);
}