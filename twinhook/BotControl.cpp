#include "stdafx.h"
#include "BotControl.h"
#include "TH08Control.h"
#include "IDI8A_Wrapper.h"
#include "DI8Control.h"
#include "cdraw.h"
#include "color.h"
#include "AutoBombPatch.h"
#include "vec2.h"

/*
 * TODO
 * - BUG: Bullets that come directly will not be dodged
 * - Bullet sizes contribute to action range
 * - Handle focusing better
 * - Lasers
 */
extern HANDLE ProcessHandle;

extern DirectInput8Wrapper *DirectInput8;

extern std::vector<entity> TH08_Bullets;
extern std::vector<entity> TH08_Powerups;

BOOL bBotEnabled = FALSE;
bool bRenderDetailed = false;

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
	if (bBotEnabled)
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

void CalculateNetVector(vec2 c, vec2 &guide, vec2 &threat)
{
	for (auto i = TH08_Bullets.begin(); i != TH08_Bullets.end(); ++i)
	{
		/*
		*
		*check if bullet crosses target boundary
		*
		* Action Radius:
		* center c(x, y)
		* radius r
		*
		* Bullet Path:
		* location a(x, y)
		* target b(x, y)
		*
		* normal d(x, y)
		*/

		float ur = BOT_RADIUS + (*i).sz.x / 2 * 1.414;
		float sr = ur + BOT_MACROSAFETY_DELTA;
		float lr = ur + BOT_MICROSAFETY_DELTA;

		vec2 a = (*i).p;										// bullet position
		vec2 b = a + (*i).v * BULLET_PROJECTION_FACTOR;			// bullet projected future position
		vec2 ac = c - a;										// vector from player to bullet
		vec2 ab = b - a;										// vector from bullet to future position
		vec2 ad = vec2::proj(ac, ab);							// vector from bullet to normal
		vec2 d = ad + a;										// normal position
		vec2 cd = d - c;										// vector from player to normal

		if (ac.lensq() < lr * lr)
		{
			threat -= BOT_BULLET_PRIORITY * ac.unit() / ac.lensq();
		}
		else if ((a - d).len() + (b - d).len() - (a - b).len() < ZERO_EPSILON)	// TODO replace with bounding box check
		{
			if (cd.zero())
			{
				// move the player in the direction of the bullet normal
				threat += BOT_BULLET_PRIORITY * (*i).v.normal().unit() * ac.lensq();
			}
			else if (cd.lensq() < sr * sr)
			{
				// move the player away from the normal by a factor relative to the bullet distance
				threat += BOT_BULLET_PRIORITY * cd.unit() / ac.lensq();
			}
		}
	}

	for (auto i = TH08_Powerups.begin(); i != TH08_Powerups.end(); ++i)
	{
		// make sure this powerup isn't one of those score particles
		if ((*i).me == 0) {
			vec2 m((*i).p.x - c.x, (*i).p.y - c.y);
			if (abs(m.x) < 300 && abs(m.y) < 70) {
				guide -= m.unit() * .1f / m.lensq();
			}
		}
	}

	// calculate wall repulsion
	float dxr = abs(pow(GAME_WIDTH - c.x, 2));
	float dxl = abs(pow(c.x, 2));
	float dyt = abs(pow(c.y, 2));
	float dyb = abs(pow(GAME_HEIGHT - c.y, 2));
	guide += vec2(.2f / dxr + -.2f / dxl, -.6f / dyt + .1f / dyb);
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
		for (auto i = TH08_Powerups.begin(); i != TH08_Powerups.end(); ++i)
		{
			if ((*i).me == 0) {
				CDraw_FillRect(d3dDev, (*i).p.x - 2 + GAME_X_OFFSET, (*i).p.y - 2 + GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 2, 255, 200));
				CDraw_Line((*i).p.x + GAME_X_OFFSET, (*i).p.y + GAME_Y_OFFSET,
					(*i).p.x + (*i).v.x * 5 + GAME_X_OFFSET, (*i).p.y + (*i).v.y * 5 + GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
			}
		}
	}
	// player area
	vec2 plyr = TH08_GetPlayerLocation();
	CDraw_FillRect(d3dDev, plyr.x - 2 + GAME_X_OFFSET, plyr.y - 2 + GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 0, 255, 0));
}

void Bot_ProcessControl(BYTE *diKeys)
{
	if (diKeys[DIK_G] & 0x80)
		Bot_SetEnable(true);
	if (diKeys[DIK_B] & 0x80)
		Bot_SetEnable(false);
	if (diKeys[DIK_H] & 0x80)
		bRenderDetailed = true;
	if (diKeys[DIK_N] & 0x80)
		bRenderDetailed = false;
}

void Bot_Tick()
{
	if (!bBotEnabled) {
		DI8C_ResetKeyState(DIK_LEFT);
		DI8C_ResetKeyState(DIK_RIGHT);
		DI8C_ResetKeyState(DIK_UP);
		DI8C_ResetKeyState(DIK_DOWN);
		DI8C_ResetKeyState(DIK_Z);
		DI8C_ResetKeyState(DIK_LSHIFT);
		return;
	}
	vec2 plyr = TH08_GetPlayerLocation();
	vec2 guide, threat;
	CalculateNetVector(plyr, guide, threat);
	vec2 net = guide + threat;
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
	if (abs(threat.x) > FOCUS_FORCE_THRESHOLD || abs(threat.y) > FOCUS_FORCE_THRESHOLD ||
		(abs(threat.x) < ZERO_EPSILON && abs(threat.y) < ZERO_EPSILON))
	{
		DI8C_SetKeyState(DIK_LSHIFT, 0);
	}
	else
	{
		DI8C_SetKeyState(DIK_LSHIFT, 0x80);
	}
}