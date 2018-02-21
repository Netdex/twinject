#include "stdafx.h"
#include "th08_player.h"
#include "IDI8ADevice_Wrapper.h"
#include "th08_patch_autobomb.h"
#include "th08_config.h"
#include "th_di8_hook.h"
#include "cdraw.h"
#include "color.h"
#include "di8_input_overlay.h"

/*
* TODO
*
* X - BUG: Bullets that come directly will not be dodged (fixed?)
* X - Bullet sizes contribute to action range (fixed?)
*
* - Handle focusing better
* - Lasers
* - Reimplement shrinking radius system
* - Implement simulated annealing tuning of parameters
* - Decrease focus dodge force (ie. focus more often)
* - Implement player velocity determination/calibration
* - Enemy detection and targeting (partially implemented through boss detection)
*/

void th08_player::on_init()
{
	LOG("th08 player initialized");
}

void th08_player::on_tick()
{
	th_di8_hook* di8 = th_di8_hook::inst();
	BYTE diKeys[256];
	if (di8->DirectInput8) {
		if (di8->DirectInput8->DirectInputDevice8->GetDeviceState(256, diKeys) == DI_OK)
		{
			this->handle_input(diKeys);
		}
	}

	if (!enabled) {
		di8->reset_vk_state(DIK_LEFT);
		di8->reset_vk_state(DIK_RIGHT);
		di8->reset_vk_state(DIK_UP);
		di8->reset_vk_state(DIK_DOWN);
		di8->reset_vk_state(DIK_Z);
		di8->reset_vk_state(DIK_LSHIFT);
		di8->reset_vk_state(DIK_LCONTROL);
		return;
	}

	di8->set_vk_state(DIK_Z, 0x80);			// fire continuously
	di8->set_vk_state(DIK_LCONTROL, 0x80);	// skip dialogue continuously

	vec2 plyr = get_plyr_loc();
	vec2 boss = get_boss_loc();
	vec2 guide, threat;
	net_vector(plyr, boss, guide, threat);
	vec2 net = guide + threat;

	if (abs(net.x) > th08_param::BOT_ACTION_THRESHOLD)
	{
		if (net.x > 0) {
			di8->set_vk_state(DIK_LEFT, 0x80);
			di8->set_vk_state(DIK_RIGHT, 0);
		}
		else
		{
			di8->set_vk_state(DIK_RIGHT, 0x80);
			di8->set_vk_state(DIK_LEFT, 0);
		}
	}
	else
	{
		di8->set_vk_state(DIK_RIGHT, 0);
		di8->set_vk_state(DIK_LEFT, 0);
	}
	if (abs(net.y) > th08_param::BOT_ACTION_THRESHOLD)
	{
		if (net.y > 0) {
			di8->set_vk_state(DIK_UP, 0x80);
			di8->set_vk_state(DIK_DOWN, 0);
		}
		else
		{
			di8->set_vk_state(DIK_DOWN, 0x80);
			di8->set_vk_state(DIK_UP, 0);
		}
	}
	else
	{
		di8->set_vk_state(DIK_DOWN, 0);
		di8->set_vk_state(DIK_UP, 0);
	}
	if (abs(threat.x) > th08_param::FOCUS_FORCE_THRESHOLD || abs(threat.y) > th08_param::FOCUS_FORCE_THRESHOLD ||
		(abs(threat.x) < ZERO_EPSILON && abs(threat.y) < ZERO_EPSILON))
	{
		di8->set_vk_state(DIK_LSHIFT, 0);
	}
	else
	{
		di8->set_vk_state(DIK_LSHIFT, 0x80);
	}

	
}

void th08_player::on_begin_tick()
{

}

void th08_player::on_after_tick()
{
	bullets.clear();
	powerups.clear();
}

#define D BotOverlayRenderer_DisplayDebugString
#define B BotOverlayRenderer_BeginDebugString

static char BotOverlayRenderer_StringBuffer[256];
static int BotOverlayRenderer_DebugLineOffset = 0;

static void BotOverlayRenderer_BeginDebugString()
{
	CDraw_FillRect(445, 250, 640, 480, D3DCOLOR_ARGB(200, 0, 0, 0));
	BotOverlayRenderer_DebugLineOffset = 0;
}

// BUG this code actually uses a ridiculous amount of computing power
// render text to a double-buffer then blit onto screen instead
// or use ID3DXSprite
static void BotOverlayRenderer_DisplayDebugString(D3DCOLOR color, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf_s(BotOverlayRenderer_StringBuffer, 256, fmt, args);
	CDraw_Text(BotOverlayRenderer_StringBuffer, color,
		450, 255 + 15 * BotOverlayRenderer_DebugLineOffset, th08_param::WINDOW_WIDTH, th08_param::WINDOW_HEIGHT);
	va_end(args);
	BotOverlayRenderer_DebugLineOffset++;
}

void th08_player::draw(IDirect3DDevice9* d3dDev)
{
	vec2 plyr = get_plyr_loc();
	vec2 boss = get_boss_loc();

	B();
	D(D3DCOLOR_ARGB(255, 0, 255, 255), "TWINJECT [netdex]");
	D(D3DCOLOR_ARGB(255, 255, 255, 255), "BULLET_COUNT: %d", bullets.size());
	D(D3DCOLOR_ARGB(255, 255, 255, 255), "   BOT_STATE: %s", enabled ? "ENABLED" : "DISABLED");
	D(D3DCOLOR_ARGB(255, 255, 255, 255), "         VIZ: %s", render_detailed ? "DETAILED" : "NONE");

	if (render_detailed) {
		// bullet markers
		for (auto i = bullets.begin(); i != bullets.end(); ++i)
		{
			if ((*i).me)
			{
				CDraw_Rect((*i).p.x - 7 + th08_param::GAME_X_OFFSET, (*i).p.y - 7 + th08_param::GAME_Y_OFFSET, 14, 14, D3DCOLOR_HSV((double)(16 * (*i).me), 1, 1)));
			}
			else {
				CDraw_Rect((*i).p.x - (*i).sz.x / 2 + th08_param::GAME_X_OFFSET, (*i).p.y - (*i).sz.x / 2 + th08_param::GAME_Y_OFFSET, (*i).sz.x, (*i).sz.y, D3DCOLOR_ARGB(255, 255, 2, 200));
			}
			vec2 proj = (*i).p + (*i).v.transform(proj_transform);

			CDraw_Line((*i).p.x + th08_param::GAME_X_OFFSET, (*i).p.y + th08_param::GAME_Y_OFFSET,
				proj.x + th08_param::GAME_X_OFFSET, proj.y + th08_param::GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
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

		CDraw_FillRect(plyr.x - 2 + th08_param::GAME_X_OFFSET, plyr.y - 2 + th08_param::GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 0, 255, 0));

		CDraw_Line(boss.x + th08_param::GAME_X_OFFSET, 0, boss.x + th08_param::GAME_X_OFFSET, (float) th08_param::WINDOW_HEIGHT, D3DCOLOR_ARGB(255, 255, 0, 0));
	}

	DI8_Overlay_RenderInput(d3dDev);
}

void th08_player::handle_input(BYTE diKeys[256])
{
	if (diKeys[DIK_G])
		set_enable(true);
	if (diKeys[DIK_B])
		set_enable(false);
	if (diKeys[DIK_H])
		render_detailed = true;
	if (diKeys[DIK_N])
		render_detailed = false;
}

void th08_player::on_enable_changed(bool enable)
{
	th08_patch_autobomb ptch;
	if (enable)
	{
		ptch.patch();
	}
	else
	{
		ptch.unpatch();
	}
}

void th08_player::net_vector(vec2 c, vec2 bs, vec2& guide, vec2& threat)
{
	for (auto i = bullets.begin(); i != bullets.end(); ++i)
	{
		/*
		* check if bullet crosses target boundary
		* Action Radius: center c(x, y) radius r
		* Bullet Path: location a(x, y) target b(x, y) normal d(x, y)
		*/

		float ur = th08_param::BOT_MIN_RADIUS + (*i).sz.x / 2 * 1.41421356237f;
		float sr = ur + th08_param::BOT_MACROSAFETY_DELTA;
		float lr = ur + th08_param::BOT_MICROSAFETY_DELTA;

		vec2 a = (*i).p;										// bullet position
		vec2 b = a + (*i).v.transform(proj_transform);			// bullet projected future position
		vec2 ac = c - a;										// vector from player to bullet
		vec2 ab = b - a;										// vector from bullet to future position
		vec2 ad = vec2::proj(ac, ab);							// vector from bullet to normal
		vec2 d = ad + a;										// normal position
		vec2 cd = d - c;										// vector from player to normal

		if (ac.lensq() < lr * lr)
		{
			threat -= th08_param::BOT_BULLET_PRIORITY * ac.unit() / ac.lensq();
		}
		else if (vec2::in_aabb(d, a, b))
		{
			if (cd.zero())
			{
				// move the player in the direction of the bullet normal
				threat += th08_param::BOT_BULLET_PRIORITY * (*i).v.normal().unit() * ac.lensq();
			}
			else if (cd.lensq() < sr * sr)
			{
				// move the player away from the normal by a factor relative to the bullet distance
				threat += th08_param::BOT_BULLET_PRIORITY * cd.unit() / ac.lensq(); // TODO factor in distance cd as well
			}
		}
	}

	for (auto i = powerups.begin(); i != powerups.end(); ++i)
	{
		// make sure this powerup isn't one of those score particles
		if ((*i).me == 0) {
			vec2 m((*i).p.x - c.x, (*i).p.y - c.y);
			if (abs(m.x) < th08_param::BOT_POWERUP_MAXY && abs(m.y) < th08_param::BOT_POWERUP_MINY) {
				guide -= th08_param::BOT_POWERUP_PRIORITY * m.unit() / m.lensq();
			}
		}
	}

	// calculate wall repulsion
	float dxr = abs(pow(th08_param::GAME_WIDTH - c.x, 2));
	float dxl = abs(pow(c.x, 2));
	float dyt = abs(pow(c.y, 2));
	float dyb = abs(pow(th08_param::GAME_HEIGHT - c.y, 2));
	guide += vec2(.2f / dxr + -.2f / dxl, -.6f / dyt + .1f / dyb);

	// calculate boss attraction
	/*if (!b.nan()) {
	float dxbs = -(signbit(b.x - c.x) ? -1 : 1) * abs(pow(b.x - c.x, 2));
	guide += vec2(dxbs / BOT_BOSS_TARGET_PRIORITY, 0);
	}*/
}

float th08_player::proj_transform(float x)
{
	return x * th08_param::BOT_BULLET_PROJECTION_FACTOR;
}

static PBYTE PlayerPosAddr = (PBYTE)0x017D6110;
static PBYTE BossPosAddr = (PBYTE)0x004CE7EC;

vec2 th08_player::get_plyr_loc()
{
	return vec2(*(float*)PlayerPosAddr - th08_param::GAME_X_OFFSET, *(float*)(PlayerPosAddr + 4) - th08_param::GAME_Y_OFFSET);
}

vec2 th08_player::get_boss_loc()
{
	vec2 a(*(float*)BossPosAddr - th08_param::GAME_X_OFFSET, *(float*)(BossPosAddr + 4) - th08_param::GAME_Y_OFFSET);
	if (a.x < 0 || a.y < 0) return vec2(NAN, NAN);
	return a;
}
