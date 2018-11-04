#include "../stdafx.h"
#include "th_vector_algo.h"
#include "../config/th_config.h"
#include "../hook/th_di8_hook.h"
#include "../util/cdraw.h"
#include "../util/color.h"

void th_vector_algo::onTick()
{
	th_di8_hook* di8 = th_di8_hook::inst();
	if (!player->enabled) {
		di8->resetVkState(DIK_LEFT);
		di8->resetVkState(DIK_RIGHT);
		di8->resetVkState(DIK_UP);
		di8->resetVkState(DIK_DOWN);
		di8->resetVkState(DIK_Z);
		di8->resetVkState(DIK_LSHIFT);
		di8->resetVkState(DIK_LCONTROL);
		return;
	}

	di8->setVkState(DIK_Z, DIK_KEY_DOWN);			// fire continuously
	di8->setVkState(DIK_LCONTROL, DIK_KEY_DOWN);	// skip dialogue continuously

	entity plyr = player->getPlayerEntity();
	vec2 guide, threat;
	net_vector(plyr.position, vec2(), guide, threat);
	vec2 net = guide + threat;

	if (abs(net.x) > BOT_ACTION_THRESHOLD)
	{
		if (net.x > 0) {
			di8->setVkState(DIK_LEFT, DIK_KEY_DOWN);
			di8->setVkState(DIK_RIGHT, DIK_KEY_UP);
		}
		else
		{
			di8->setVkState(DIK_RIGHT, DIK_KEY_DOWN);
			di8->setVkState(DIK_LEFT, DIK_KEY_UP);
		}
	}
	else
	{
		di8->setVkState(DIK_RIGHT, DIK_KEY_UP);
		di8->setVkState(DIK_LEFT, DIK_KEY_UP);
	}
	if (abs(net.y) > BOT_ACTION_THRESHOLD)
	{
		if (net.y > 0) {
			di8->setVkState(DIK_UP, DIK_KEY_DOWN);
			di8->setVkState(DIK_DOWN, DIK_KEY_UP);
		}
		else
		{
			di8->setVkState(DIK_DOWN, DIK_KEY_DOWN);
			di8->setVkState(DIK_UP, DIK_KEY_UP);
		}
	}
	else
	{
		di8->setVkState(DIK_DOWN, DIK_KEY_UP);
		di8->setVkState(DIK_UP, DIK_KEY_UP);
	}
	if (abs(threat.x) > FOCUS_FORCE_THRESHOLD || abs(threat.y) > FOCUS_FORCE_THRESHOLD ||
		(abs(threat.x) < 0.000001 && abs(threat.y) < 0.000001))
	{
		di8->setVkState(DIK_LSHIFT, DIK_KEY_UP);
	}
	else
	{
		di8->setVkState(DIK_LSHIFT, DIK_KEY_DOWN);
	}
}

void th_vector_algo::visualize(IDirect3DDevice9* d3dDev)
{
	entity plyr = player->getPlayerEntity();
	//vec2 boss = player->get_boss_loc();
	if (player->render) {
		// bullet markers
		for (auto i = player->bullets.begin(); i != player->bullets.end(); ++i)
		{
			if ((*i).meta)
			{
				cdraw::rect((*i).position.x - 7 + th_param.GAME_X_OFFSET, (*i).position.y - 7 + th_param.GAME_Y_OFFSET, 14, 14, D3DCOLOR_HSV((double)(16 * (*i).meta), 1, 1)));
			}
			else {
				cdraw::rect((*i).position.x - (*i).size.x / 2 + th_param.GAME_X_OFFSET, (*i).position.y - (*i).size.x / 2 + th_param.GAME_Y_OFFSET, (*i).size.x, (*i).size.y, D3DCOLOR_ARGB(255, 255, 2, 200));
			}
			vec2 proj = (*i).position + (*i).velocity.transform(proj_transform);

			cdraw::line((*i).position.x + th_param.GAME_X_OFFSET, (*i).position.y + th_param.GAME_Y_OFFSET,
				proj.x + th_param.GAME_X_OFFSET, proj.y + th_param.GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
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

		cdraw::fillRect(plyr.position.x - 2 + th_param.GAME_X_OFFSET, plyr.position.y - 2 + th_param.GAME_Y_OFFSET, 5, 5, D3DCOLOR_ARGB(255, 0, 255, 0));

		//CDraw_Line(boss.x + th08_param::GAME_X_OFFSET, 0, boss.x + th08_param::GAME_X_OFFSET, (float)th08_param::WINDOW_HEIGHT, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
}

void th_vector_algo::net_vector(vec2 c, vec2 bs, vec2& guide, vec2& threat) const
{
	for (auto i = player->bullets.begin(); i != player->bullets.end(); ++i)
	{
		/*
		* check if bullet crosses target boundary
		* Action Radius: center c(x, y) radius r
		* Bullet Path: location a(x, y) target b(x, y) normal d(x, y)
		*/

		float ur = BOT_MIN_RADIUS + (*i).size.x / 2 * 1.41421356237f;
		float sr = ur + BOT_MACROSAFETY_DELTA;
		float lr = ur + BOT_MICROSAFETY_DELTA;

		vec2 a = (*i).position;										// bullet position
		vec2 b = a + (*i).velocity.transform(proj_transform);			// bullet projected future position
		vec2 ac = c - a;										// vector from player to bullet
		vec2 ab = b - a;										// vector from bullet to future position
		vec2 ad = vec2::proj(ac, ab);							// vector from bullet to normal
		vec2 d = ad + a;										// normal position
		vec2 cd = d - c;										// vector from player to normal

		if (ac.lensq() < lr * lr)
		{
			threat -= BOT_BULLET_PRIORITY * ac.unit() / ac.lensq();
		}
		else if (vec2::inAABB(d, a, b))
		{
			if (cd.zero())
			{
				// move the player in the direction of the bullet normal
				threat += BOT_BULLET_PRIORITY * (*i).velocity.normal().unit() * ac.lensq();
			}
			else if (cd.lensq() < sr * sr)
			{
				// move the player away from the normal by a factor relative to the bullet distance
				threat += BOT_BULLET_PRIORITY * cd.unit() / ac.lensq(); // TODO factor in distance cd as well
			}
		}
	}

	for (auto i = player->powerups.begin(); i != player->powerups.end(); ++i)
	{
		// make sure this powerup isn't one of those score particles
		if ((*i).meta == 0) {
			vec2 m((*i).position.x - c.x, (*i).position.y - c.y);
			if (abs(m.x) < BOT_POWERUP_MAXY && abs(m.y) < BOT_POWERUP_MINY) {
				guide -= BOT_POWERUP_PRIORITY * m.unit() / m.lensq();
			}
		}
	}

	// calculate wall repulsion
	float dxr = abs(pow(th_param.GAME_WIDTH - c.x, 2));
	float dxl = abs(pow(c.x, 2));
	float dyt = abs(pow(c.y, 2));
	float dyb = abs(pow(th_param.GAME_HEIGHT - c.y, 2));
	guide += vec2(.2f / dxr + -.2f / dxl, -.6f / dyt + .1f / dyb);

	// calculate boss attraction
	/*if (!b.nan()) {
	float dxbs = -(signbit(b.x - c.x) ? -1 : 1) * abs(pow(b.x - c.x, 2));
	guide += vec2(dxbs / BOT_BOSS_TARGET_PRIORITY, 0);
	}*/
}

float th_vector_algo::proj_transform(float x)
{
	return x * BOT_BULLET_PROJECTION_FACTOR;
}