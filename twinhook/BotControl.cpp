#include "stdafx.h"
#include "BotControl.h"
#include "TH08Control.h"
#include "DI8Control.h"
#include "th08_patch_autobomb.h"
#include "vec2.h"

const double M_PI = acos(-1);
const double M_E = exp(1);

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
extern HANDLE TH08_ProcessHandle;

extern std::vector<entity> TH08_Bullets;
extern std::vector<entity> TH08_Powerups;

bool BotControl_FlagBotEnabled = false;
bool BotControl_FlagRenderDetailed = false;

void Bot_OnEnable()
{
	th08_patch_autobomb ptch;
	ptch.patch();
}

void Bot_OnDisable()
{
	th08_patch_autobomb ptch;
	ptch.unpatch();
}

void Bot_SetEnable(BOOL enable)
{
	BotControl_FlagBotEnabled = enable;
	if (BotControl_FlagBotEnabled)
	{
		Bot_OnEnable();
	}
	else
	{
		Bot_OnDisable();
	}
}

bool Bot_IsEnabled()
{
	return BotControl_FlagBotEnabled;
}

// determine whether this actually helps
float Bot_ProjectionTransform(float x)
{
	//return (float)(x * (1 / (0.1*powf(x, 2) + 1) + 1) * BOT_BULLET_PROJECTION_FACTOR);
	return x * BOT_BULLET_PROJECTION_FACTOR;
}

/**
 * \brief Calculate the net weighted vector of forces
 * \param c Player center
 * \param bs Location of main boss
 * \param guide Reference to return guiding vector
 * \param threat Reference to return threat avoidance bector
 */
void CalculateNetVector(vec2 c, vec2 bs, vec2 &guide, vec2 &threat)
{
	for (auto i = TH08_Bullets.begin(); i != TH08_Bullets.end(); ++i)
	{
		/*
		 * check if bullet crosses target boundary
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

		float ur = BOT_MIN_RADIUS + (*i).sz.x / 2 * 1.41421356237f;
		float sr = ur + BOT_MACROSAFETY_DELTA;
		float lr = ur + BOT_MICROSAFETY_DELTA;

		vec2 a = (*i).p;										// bullet position
		vec2 b = a + (*i).v.transform(Bot_ProjectionTransform);			// bullet projected future position
		vec2 ac = c - a;										// vector from player to bullet
		vec2 ab = b - a;										// vector from bullet to future position
		vec2 ad = vec2::proj(ac, ab);							// vector from bullet to normal
		vec2 d = ad + a;										// normal position
		vec2 cd = d - c;										// vector from player to normal

		if (ac.lensq() < lr * lr)
		{
			threat -= BOT_BULLET_PRIORITY * ac.unit() / ac.lensq();
		}
		else if (vec2::in_aabb(d, a, b))
		{
			if (cd.zero())
			{
				// move the player in the direction of the bullet normal
				threat += BOT_BULLET_PRIORITY * (*i).v.normal().unit() * ac.lensq();
			}
			else if (cd.lensq() < sr * sr)
			{
				// move the player away from the normal by a factor relative to the bullet distance
				threat += BOT_BULLET_PRIORITY * cd.unit() / ac.lensq(); // TODO factor in distance cd as well
			}
		}
	}

	for (auto i = TH08_Powerups.begin(); i != TH08_Powerups.end(); ++i)
	{
		// make sure this powerup isn't one of those score particles
		if ((*i).me == 0) {
			vec2 m((*i).p.x - c.x, (*i).p.y - c.y);
			if (abs(m.x) < BOT_POWERUP_MAXY && abs(m.y) < BOT_POWERUP_MINY) {
				guide -= BOT_POWERUP_PRIORITY * m.unit() / m.lensq();
			}
		}
	}

	// calculate wall repulsion
	float dxr = abs(pow(GAME_WIDTH - c.x, 2));
	float dxl = abs(pow(c.x, 2));
	float dyt = abs(pow(c.y, 2));
	float dyb = abs(pow(GAME_HEIGHT - c.y, 2));
	guide += vec2(.2f / dxr + -.2f / dxl, -.6f / dyt + .1f / dyb);

	// calculate boss attraction
	/*if (!b.nan()) {
		float dxbs = -(signbit(b.x - c.x) ? -1 : 1) * abs(pow(b.x - c.x, 2));
		guide += vec2(dxbs / BOT_BOSS_TARGET_PRIORITY, 0);
	}*/
}

void Bot_ProcessControl(BYTE *diKeys)
{
	if (diKeys[DIK_G])
		Bot_SetEnable(true);
	if (diKeys[DIK_B])
		Bot_SetEnable(false);
	if (diKeys[DIK_H])
		BotControl_FlagRenderDetailed = true;
	if (diKeys[DIK_N])
		BotControl_FlagRenderDetailed = false;
}

void Bot_Tick()
{
	if (!BotControl_FlagBotEnabled) {
		DI8C_ResetKeyState(DIK_LEFT);
		DI8C_ResetKeyState(DIK_RIGHT);
		DI8C_ResetKeyState(DIK_UP);
		DI8C_ResetKeyState(DIK_DOWN);
		DI8C_ResetKeyState(DIK_Z);
		DI8C_ResetKeyState(DIK_LSHIFT);
		DI8C_ResetKeyState(DIK_LCONTROL);
		return;
	}

	DI8C_SetKeyState(DIK_Z, 0x80);			// fire continuously
	DI8C_SetKeyState(DIK_LCONTROL, 0x80);	// skip dialogue continuously

	vec2 plyr = TH08_GetPlayerLocation();
	vec2 boss = TH08_GetBossPosition();
	vec2 guide, threat;
	CalculateNetVector(plyr, boss, guide, threat);
	vec2 net = guide + threat;

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