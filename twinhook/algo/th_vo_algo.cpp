#include "../stdafx.h"
#include "th_vo_algo.h"
#include "../control/th_player.h"
#include "../hook/th_di8_hook.h"
#include "../config/th_config.h"
#include "../util/cdraw.h"
#include "../util/color.h"
#include "../control/th15_player.h"
#include "control/th10_player.h"

void th_vo_algo::onBegin()
{
	calibInit();
}

void th_vo_algo::onTick()
{
	auto di8 = th_di8_hook::inst();

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

	if (!isCalibrated)
	{
		isCalibrated = calibTick();
		if (isCalibrated) {
			LOG("calibrated plyr vel: %f %f", playerVel, playerFocVel);
		}
		return;
	}

	entity plyr = player->getPlayerEntity();

	/*
	 * Ticks until collision whilst moving in this direction
	 * Uses same direction numbering schema
	 */
	float collisionTicks[NUM_DIRS];
	std::fill_n(collisionTicks, NUM_DIRS, FLT_MAX);

	bool bounded = true;

	// Bullet collision frame calculations
	for (auto bullet = player->bullets.begin(); bullet != player->bullets.end(); ++bullet)
	{
		for (int dir = 0; dir < NUM_DIRS; ++dir)
		{
			vec2 pvel = DIRECTION_VEL[dir] * (FOCUSED_DIR[dir] ? playerFocVel : playerVel);
			float colTick;
			// TODO this hitCircle stuff is painful, replace with interfaces and impl
			if (hitCircle) {
				colTick = vec2::willCollideCircle(
					plyr.p, bullet->p,
					plyr.sz.x, bullet->sz.y,
					pvel,
					bullet->v
				);
			}
			else
			{
				colTick = vec2::willCollideAABB(
					plyr.p - plyr.sz / 2,
					bullet->p - bullet->sz / 2,
					plyr.sz,
					bullet->sz,
					pvel,
					bullet->v
				);
			}
			if (colTick >= 0) {
				collisionTicks[dir] = std::min(colTick, collisionTicks[dir]);
				bounded = false;
			}
		}
	}

	// Powerup collision frame calculations
	// BUG powerups move non-linearly so this doesn't work well
	// TODO do not target powerups that will go off the screen
	// TODO do not target powerups that are too high
	/*
	 * Ticks until collision with target whilst moving in this diretion
	 * Uses same direction numbering schema
	 */
	float targetTicks[NUM_DIRS];
	std::fill_n(targetTicks, NUM_DIRS, FLT_MAX);

	for (auto p = player->powerups.begin(); p != player->powerups.end(); ++p)
	{
		// filter out unwanted powerups
		if (p->me == 0 && p->p.y > 200) {
			for (int dir = 0; dir < NUM_DIRS; ++dir)
			{
				vec2 pvel = DIRECTION_VEL[dir] * (FOCUSED_DIR[dir] ? playerFocVel : playerVel);
				float colTick;
				if (hitCircle)
				{
					colTick = vec2::willCollideCircle(
						plyr.p, p->p,
						plyr.sz.x, p->sz.x,
						pvel,
						p->v
					);
				}
				else
				{
					colTick = vec2::willCollideAABB(
						plyr.p - plyr.sz / 2,
						p->p - p->sz / 2,
						plyr.sz,
						p->sz,
						pvel,
						p->v
					);
				}
				if (colTick >= 0) {
					targetTicks[dir] = std::min(colTick, targetTicks[dir]);
				}
			}
		}
	}

	// Wall collision frame calculations
	for (int dir = 1; dir < NUM_DIRS; ++dir)
	{
		vec2 pvel = DIRECTION_VEL[dir] * (FOCUSED_DIR[dir] ? playerFocVel : playerVel);
		float t;
		if (hitCircle)
		{
			t = vec2::willExitAABB(
				vec2(0, 0), plyr.p - plyr.sz,
				vec2(th_param.GAME_WIDTH, th_param.GAME_HEIGHT), plyr.sz * 2,
				vec2(), pvel
			);
		}
		else
		{
			t = vec2::willExitAABB(
				vec2(0, 0), plyr.p - plyr.sz / 2,
				vec2(th_param.GAME_WIDTH, th_param.GAME_HEIGHT), plyr.sz,
				vec2(), pvel
			);
		}

		if (t >= 0) {
			if (t < collisionTicks[dir])
			{
				collisionTicks[dir] = t;
			}
		}
	}


	// Viable target calculations
	int tarIdx = -1;
	for (int dir = 0; dir < NUM_DIRS; ++dir)
	{
		if (targetTicks[dir] < collisionTicks[dir]
			&& (tarIdx == -1 || targetTicks[dir] < targetTicks[tarIdx]))
			tarIdx = dir;
	}

	// check if we could find a targetable powerup
	if (tarIdx == -1) {
		// find direction with maximum frames until collision

		/* TODO changed from 0 to 1 to disable hold position
		 * note: there is a major problem with how we do things.
		 * We do not know the velocities of all entities (e.g. enemies, lasers),
		 * so we assume them to have zero velocity.
		 * Therefore the hold position grace time may be overestimated significantly,
		 * especially if the hazard is heading directly towards the player.
		 * Thus, we disable the hold position. This would be fine, but causes the player
		 * to constantly spaz because it cannot stay still. Then, the gameplay does not look
		 * realistic.
		 */
		int maxIdx = 1;

		// HACK if we are not being threatened by any bullets, then allow hold position
		if (bounded)
		{
			maxIdx = 0;
		}

		for (int dir = 1; dir < NUM_DIRS; ++dir)
		{
			if (collisionTicks[dir] != FLT_MAX &&
				collisionTicks[dir] > collisionTicks[maxIdx])
			{
				maxIdx = dir;
			}
		}
		tarIdx = maxIdx;
	}

	/*
	LOG("C[%d] | H:%.0f U:%.0f D:%.0f L:%.0f R:%.0f UL:%.0f UR:%.0f DL:%.0f DR:%.0f",
		tarIdx,
		collisionTicks[0] == FLT_MAX ? -1 : collisionTicks[0], collisionTicks[1],
		collisionTicks[2], collisionTicks[3], collisionTicks[4],
		collisionTicks[5], collisionTicks[6], collisionTicks[7], collisionTicks[8]
	);*/

	di8->setVkState(DIK_Z, DIK_KEY_DOWN);			// fire continuously
	di8->setVkState(DIK_LCONTROL, DIK_KEY_DOWN);	// skip dialogue continuously

	// release all control keys
	for (int i = 0; i < sizeof CONTROL_KEYS / sizeof BYTE; ++i)
		di8->resetVkState(CONTROL_KEYS[i]);

	// press required keys for moving in desired direction
	for (int i = 0; i < 3; ++i)
		if (DIR_KEYS[tarIdx][i])
			di8->setVkState(DIR_KEYS[tarIdx][i], DIK_KEY_DOWN);

	// deathbomb
	// if the bot is going to die in the next frame, then bomb
	if (collisionTicks[tarIdx] <= 1)
	{
		di8->setVkState(DIK_X, DIK_KEY_DOWN);
	}
}

void th_vo_algo::calibInit()
{
	isCalibrated = false;
	calibFrames = 0;
	calibStartX = -1;
	playerVel = 0;
}

float th_vo_algo::minStaticCollideTick(
	const std::vector<entity> &bullets,
	const vec2 &p, const vec2 &s,
	std::vector<entity> &collided) const
{
	float minTick = FLT_MAX;
	for (auto bullet = bullets.begin(); bullet != bullets.end(); ++bullet)
	{
		float colTick;
		if (hitCircle) {
			colTick = vec2::willCollideAABB(
				p, bullet->p - bullet->sz,
				s, bullet->sz * 2,
				vec2(), bullet->v
			);
		}
		else
		{
			colTick = vec2::willCollideAABB(
				p, bullet->p - bullet->sz / 2,
				s, bullet->sz,
				vec2(), bullet->v
			);
		}
		if (colTick >= 0) {
			minTick = std::min(colTick, minTick);
			collided.push_back(*bullet);
		}
		//// don't do more calculations if the density is saturated
		//if (minTick != FLT_MAX && minTick / MAX_FRAMES_TILL_COLLISION > 1)
		//	return minTick;
	}
	if (minTick != FLT_MAX && minTick >= 0)
		return minTick;
	return -1.f;
};

void th_vo_algo::vizPotentialQuadtree(
	const std::vector<entity> &bullets,
	vec2 p, vec2 s,
	float minRes) const
{
	/*cdraw::rect(
		th_param.GAME_X_OFFSET + p.x, th_param.GAME_Y_OFFSET + p.y,
		s.x, s.y,
		D3DCOLOR_ARGB(80, 255, 255, 0)
	);*/

	// create four square regions of equal size which contain (p, s)
	// they are square, since we want the final pixels to be square
	vec2 center = p + s / 2.f;
	float fSqsz = std::max(s.x, s.y) / 2;

	// not necessary, just a safety net
	if (fSqsz < minRes) {
		return;
	}
	vec2 sqsz(fSqsz, fSqsz);

	vec2 colDomains[] = {
		center - sqsz,							// top-left
		vec2(center.x, center.y - sqsz.y),		// top-right
		vec2(center.x - sqsz.x, center.y),		// bottom-left
		center									// bottom-right
	};

	for (int i = 0; i < 4; i++)
	{
		std::vector<entity> collided;
		float colTick = minStaticCollideTick(bullets, colDomains[i], sqsz, collided);
		if (colTick >= 0) {
			if (fSqsz / 2 <= minRes)
			{
				hsv col_hsv = { colTick / MAX_FRAMES_TILL_COLLISION * 360, 1, 1 };
				rgb col_rgb = hsv2rgb(col_hsv);
				cdraw::fillRect(
					th_param.GAME_X_OFFSET + colDomains[i].x,
					th_param.GAME_Y_OFFSET + colDomains[i].y,
					sqsz.x, sqsz.y,
					D3DCOLOR_ARGB(100,
					(int)(col_rgb.r * 255), (int)(col_rgb.g * 255), (int)(col_rgb.b * 255))
				);
			}
			else {
				vizPotentialQuadtree(collided, colDomains[i], sqsz, minRes);
			}
		}
	}

}

void th_vo_algo::visualize(IDirect3DDevice9* d3dDev)
{
	if (player->render)
	{
		entity plyr = player->getPlayerEntity();

		// draw vector field (laggy)
		/*viz_potential_quadtree(
			player->bullets,
			vec2(), vec2(th_param.GAME_WIDTH, th_param.GAME_HEIGHT),
			VEC_FIELD_MIN_RESOLUTION);*/

			// draw laser points
		for (auto i = player->lasers.begin(); i != player->lasers.end(); ++i)
		{
			cdraw::fillRect(
				th_param.GAME_X_OFFSET + i->p.x - 3,
				th_param.GAME_Y_OFFSET + i->p.y - 3,
				6, 6,
				D3DCOLOR_ARGB(255, 0, 0, 255));

			// voodoo witchcraft magic

			float rex = i->rad * (float)cos(M_PI / 2 + i->ang);
			float rey = i->rad * (float)sin(M_PI / 2 + i->ang);
			cdraw::line(
				th_param.GAME_X_OFFSET + i->p.x - rex,
				th_param.GAME_Y_OFFSET + i->p.y - rey,
				th_param.GAME_X_OFFSET + i->p.x + i->ex.x - rex,
				th_param.GAME_Y_OFFSET + i->p.y + i->ex.y - rey,
				D3DCOLOR_ARGB(255, 0, 0, 255));
			cdraw::line(
				th_param.GAME_X_OFFSET + i->p.x + rex,
				th_param.GAME_Y_OFFSET + i->p.y + rey,
				th_param.GAME_X_OFFSET + i->p.x + i->ex.x + rex,
				th_param.GAME_Y_OFFSET + i->p.y + i->ex.y + rey,
				D3DCOLOR_ARGB(255, 0, 0, 255));
		}
		// dependant on hit circle vs hit box
		if (!hitCircle) {
			// bullet markers
			for (auto i = player->bullets.begin(); i != player->bullets.end(); ++i)
			{
				if ((*i).me)
				{
					cdraw::rect((*i).p.x - 7 + th_param.GAME_X_OFFSET, (*i).p.y - 7 + th_param.GAME_Y_OFFSET, 14, 14, D3DCOLOR_HSV((double)(16 * (*i).me), 1, 1)));
				}
				else {
					cdraw::rect((*i).p.x - (*i).sz.x / 2 + th_param.GAME_X_OFFSET, (*i).p.y - (*i).sz.x / 2 + th_param.GAME_Y_OFFSET, (*i).sz.x, (*i).sz.y, D3DCOLOR_ARGB(255, 255, 2, 200));
				}
				vec2 proj = (*i).p + (*i).v * 10;

				cdraw::line((*i).p.x + th_param.GAME_X_OFFSET, (*i).p.y + th_param.GAME_Y_OFFSET,
					proj.x + th_param.GAME_X_OFFSET, proj.y + th_param.GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
			}

			/*for(auto i = player->powerups.begin(); i != player->powerups.end(); ++i)
			{
				char buf[16];
				sprintf_s(buf, 16, "%d", i->me);
				cdraw::text(buf, D3DCOLOR_ARGB(255, 255, 255, 255), i->p.x, i->p.y, 700, 700);
			}*/
			for (auto b : player->powerups)
			{
				cdraw::rect(
					th_param.GAME_X_OFFSET + b.p.x - b.sz.x / 2,
					th_param.GAME_Y_OFFSET + b.p.y - b.sz.y / 2,
					b.sz.x, b.sz.y, D3DCOLOR_ARGB(255, 255, 0, 0));
				vec2 proj = b.p + b.v * 10;

				cdraw::line(b.p.x + th_param.GAME_X_OFFSET, b.p.y + th_param.GAME_Y_OFFSET,
					proj.x + th_param.GAME_X_OFFSET, proj.y + th_param.GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
			}

			cdraw::fillRect(plyr.p.x - 2 + th_param.GAME_X_OFFSET, plyr.p.y - 2 + th_param.GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 0, 255, 0));
		}
		else
		{
			for (auto b = player->bullets.begin(); b != player->bullets.end(); ++b)
			{
				// note that bullets are actually circles
				/*cdraw::circle(th_param.GAME_X_OFFSET + b->p.x,
				th_param.GAME_Y_OFFSET + b->p.y, b->sz.x, 8, D3DCOLOR_ARGB(255, 0, 255, 0));*/
				cdraw::rect(
					th_param.GAME_X_OFFSET + b->p.x - b->sz.x,
					th_param.GAME_Y_OFFSET + b->p.y - b->sz.y,
					b->sz.x * 2, b->sz.y * 2, D3DCOLOR_ARGB(255, 255, 0, 0));
				vec2 proj = (*b).p + (*b).v * 10;

				cdraw::line((*b).p.x + th_param.GAME_X_OFFSET, (*b).p.y + th_param.GAME_Y_OFFSET,
					proj.x + th_param.GAME_X_OFFSET, proj.y + th_param.GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
			}
			cdraw::fillRect(
				plyr.p.x - plyr.sz.x + th_param.GAME_X_OFFSET,
				plyr.p.y - plyr.sz.y + th_param.GAME_Y_OFFSET, plyr.sz.x * 2, plyr.sz.y * 2,
				D3DCOLOR_ARGB(255, 0, 255, 0));
		}
		
	}


}

bool th_vo_algo::calibTick()
{
	entity plyr = player->getPlayerEntity();

	switch (calibFrames)
	{
	case 0:
		// do not allow player interaction during calibration
		th_di8_hook::inst()->setVkState(DIK_LEFT, DIK_KEY_DOWN);
		th_di8_hook::inst()->setVkState(DIK_RIGHT, DIK_KEY_UP);
		th_di8_hook::inst()->setVkState(DIK_UP, DIK_KEY_UP);
		th_di8_hook::inst()->setVkState(DIK_DOWN, DIK_KEY_UP);
		break;
	case 1:
		th_di8_hook::inst()->setVkState(DIK_LEFT, DIK_KEY_UP);
		calibStartX = plyr.p.x;
		break;
	case 2:
		th_di8_hook::inst()->setVkState(DIK_RIGHT, DIK_KEY_DOWN);
		break;
	case 3:
		th_di8_hook::inst()->resetVkState(DIK_LEFT);
		th_di8_hook::inst()->resetVkState(DIK_RIGHT);
		th_di8_hook::inst()->resetVkState(DIK_UP);
		th_di8_hook::inst()->resetVkState(DIK_DOWN);

		// BUG why does LoLK do this differently
		if (dynamic_cast<th15_player*>(player) 
			|| dynamic_cast<th10_player*>(player) )
			playerVel = plyr.p.x - calibStartX;
		else
			playerVel = calibStartX - plyr.p.x;
		break;
	case 4:
		// do not allow player interaction during calibration
		th_di8_hook::inst()->setVkState(DIK_LEFT, DIK_KEY_DOWN);
		th_di8_hook::inst()->setVkState(DIK_RIGHT, DIK_KEY_UP);
		th_di8_hook::inst()->setVkState(DIK_UP, DIK_KEY_UP);
		th_di8_hook::inst()->setVkState(DIK_DOWN, DIK_KEY_UP);
		th_di8_hook::inst()->setVkState(DIK_LSHIFT, DIK_KEY_DOWN);
		break;
	case 5:
		th_di8_hook::inst()->setVkState(DIK_LEFT, DIK_KEY_UP);
		calibStartX = plyr.p.x;
		break;
	case 6:
		th_di8_hook::inst()->setVkState(DIK_RIGHT, DIK_KEY_DOWN);
		break;
	case 7:
		isCalibrated = true;
		th_di8_hook::inst()->resetVkState(DIK_LEFT);
		th_di8_hook::inst()->resetVkState(DIK_RIGHT);
		th_di8_hook::inst()->resetVkState(DIK_UP);
		th_di8_hook::inst()->resetVkState(DIK_DOWN);
		th_di8_hook::inst()->resetVkState(DIK_LSHIFT);

		// BUG why do MoF and LoLK do this differently
		if (dynamic_cast<th15_player*>(player)
			|| dynamic_cast<th10_player*>(player))
			playerFocVel = plyr.p.x - calibStartX;
		else
			playerFocVel = calibStartX - plyr.p.x;
		return true;
	}
	++calibFrames;
	return false;
}





