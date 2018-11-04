#include "stdafx.h"
#include <imgui/imgui.h>

#include "th_vo_algo.h"
#include "control/th_player.h"
#include "hook/th_di8_hook.h"
#include "config/th_config.h"
#include "util/cdraw.h"
#include "util/color.h"
#include "control/th15_player.h"
#include "control/th10_player.h"
#include "gfx/imgui_mixins.h"
#include "control/th11_player.h"
#include <numeric>

void th_vo_algo::onBegin()
{
	calibInit();
}

void th_vo_algo::onTick()
{
	/* IMGUI Integration */
	using namespace ImGui;
	Begin("th_vo_algo");
	Text("Constrained Velocity Obstacle Algorithm");
	if (CollapsingHeader("Info"))
	{
		Text("calib: %s", isCalibrated ? "true" : "false");
		SameLine(); ShowHelpMarker("Algorithm player speed calibration");

		Text("calib vel: norm %.2f, foc %.2f", playerVel, playerFocVel);
		SameLine(); ShowHelpMarker("Calibrated velocities in normal and focused mode");

		Text("col test: %s", hitCircle ? "hit circle" : "hit box");
		SameLine(); ShowHelpMarker("Collision test used");
	}
	Checkbox("Show Vector Field", &this->renderVectorField);

	auto di8 = th_di8_hook::inst();

	if (!player->enabled) {
		di8->resetVkState(DIK_LEFT);
		di8->resetVkState(DIK_RIGHT);
		di8->resetVkState(DIK_UP);
		di8->resetVkState(DIK_DOWN);
		di8->resetVkState(DIK_Z);
		di8->resetVkState(DIK_LSHIFT);
		di8->resetVkState(DIK_LCONTROL);
		End();
		return;
	}

	if (!isCalibrated)
	{
		isCalibrated = calibTick();
		if (isCalibrated) {
			LOG("calibrated plyr vel: %f %f", playerVel, playerFocVel);
		}
		End();
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
	for (auto b = player->bullets.begin(); b != player->bullets.end(); ++b)
	{
		for (int dir = 0; dir < NUM_DIRS; ++dir)
		{
			vec2 pvel = this->getPlayerMovement(dir);
			float colTick;
			// TODO this hitCircle stuff is painful, replace with interfaces and impl
			if (hitCircle) {
				colTick = vec2::willCollideCircle(
					plyr.position, b->position,
					plyr.size.x / 2, b->size.x / 2,
					pvel,
					b->velocity
				);
			}
			else
			{
				colTick = vec2::willCollideAABB(
					plyr.position - plyr.size / 2,
					b->position - b->size / 2,
					plyr.size,
					b->size,
					pvel,
					b->velocity
				);
			}
			if (colTick >= 0) {
				collisionTicks[dir] = std::min(colTick, collisionTicks[dir]);
				bounded = false;
			}
		}
	}

	for (laser l : player->lasers)
	{
		for (int dir = 0; dir < NUM_DIRS; ++dir)
		{
			vec2 pvel = this->getPlayerMovement(dir);
			float colTick;
			if (hitCircle)
			{
				// TODO I have not written the SAT predictor code for circles yet, 
				// so this part won't work for circles.
				colTick = -1;
			}
			else
			{

				std::vector<vec2> playerVert = vec2::aabbVert(plyr.position - plyr.size / 2, plyr.size);
				std::vector<vec2> laserVert = l.getVertices();

				colTick = vec2::willCollideSAT(playerVert, pvel, laserVert, l.velocity);
			}
			if (colTick >= 0) {
				collisionTicks[dir] = std::min(colTick, collisionTicks[dir]);
				bounded = false;
			}
		}
	}

	/* 
	 * Powerup collision frame calculations
	 * Note: Powerups do not move linearly so using a linear model might be poor.
	 */

	// Ticks until collision with target whilst moving in this direction
	float targetTicks[NUM_DIRS];
	std::fill_n(targetTicks, NUM_DIRS, FLT_MAX);

	for (auto& powerup : player->powerups)
	{
		// Filter out unwanted powerups
		if (powerup.meta == 0 && powerup.position.y > 200) {
			for (int dir = 0; dir < NUM_DIRS; ++dir)
			{
				vec2 pvel = this->getPlayerMovement(dir);
				
				/* 
				 * Powerups tend to be attracted towards the player, so we can be 
				 * very lax with the collision predictor and use the AABB model 
				 * all the time
				 */
				float colTick = vec2::willCollideAABB(
						plyr.position - plyr.size / 2, powerup.position - powerup.size / 2, plyr.size, powerup.size, pvel, powerup.velocity);
				
				if (colTick >= 0) {
					targetTicks[dir] = std::min(colTick, targetTicks[dir]);
				}
			}
		}
	}

	// We should probably prioritize larger enemies over smaller ones, 
	// and prioritize powerup gathering over enemies
	for (auto enemy : player->enemies)
	{
		if (enemy.position.y < plyr.position.y) {
			for (int dir = Direction::Left; dir <= Direction::Right; ++dir)
			{
				vec2 pvel = this->getPlayerMovement(dir);

				// Calculate x-distance to y-aligned axis of the enemy
				float xDist = enemy.position.x - plyr.position.x;
				float colTick = xDist / pvel.x;
				// TODO check if we are actually under them
				// Filter out impossible values
				if (colTick >= 0 && colTick <= 6000)
				{
					targetTicks[dir] = std::min(colTick, targetTicks[dir]);
				}
			}
		}
	}

	// Wall collision frame calculations
	for (int dir = 1; dir < NUM_DIRS; ++dir)
	{
		vec2 pvel = this->getPlayerMovement(dir);
		float t = vec2::willExitAABB(
			vec2(0, 0), plyr.position - plyr.size / 2, vec2(th_param.GAME_WIDTH, th_param.GAME_HEIGHT),
			plyr.size, vec2(), pvel);
		if (t >= 0) {
			if (t < collisionTicks[dir])
				collisionTicks[dir] = t;
		}
	}
	// Look for best viable target, aka targeting will not result in collision
	int tarIdx = -1;
	if(*std::min_element(collisionTicks, collisionTicks + NUM_DIRS) > MIN_SAFETY_TICK)
	{
		for (int dir = 0; dir < NUM_DIRS; ++dir)
		{
			if (targetTicks[dir] < collisionTicks[dir]
				&& (tarIdx == -1 || targetTicks[dir] < targetTicks[tarIdx]))
				tarIdx = dir;
		}
	}
	
	

	bool powerupTarget = true;
	// check if we could find a targetable powerup
	if (tarIdx == -1) {
		powerupTarget = false;
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

	/* IMGUI Integration */
	int minTimeIdx = 0;
	for (int dir = 1; dir < NUM_DIRS; ++dir)
	{
		if (collisionTicks[dir] < collisionTicks[minTimeIdx])
			minTimeIdx = dir;
	}
	for (int i = 1; i < RISK_HISTORY_SIZE; ++i)
		riskHistory[i - 1] = riskHistory[i];
	riskHistory[RISK_HISTORY_SIZE - 1] = collisionTicks[minTimeIdx];
	PlotLines("danger hist", riskHistory, IM_ARRAYSIZE(riskHistory), 0, "",
		0.f, 30.f, ImVec2(0, 80));
	SameLine(); ShowHelpMarker("frames until collision of the best move,\n"
		"maximization parameter");

	/*LOG("C[%d] | H:%.0f U:%.0f D:%.0f L:%.0f R:%.0f UL:%.0f UR:%.0f DL:%.0f DR:%.0f",
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

	// deathbomb if the bot is going to die in the next frame
	// this is very dependent on the collision predictor being very accurate
	if (!powerupTarget && collisionTicks[tarIdx] < 0.5f)
	{
		di8->setVkState(DIK_X, DIK_KEY_DOWN);
	}
	End();

}

void th_vo_algo::calibInit()
{
	isCalibrated = false;
	calibFrames = 0;
	calibStartX = -1;
	playerVel = 0;
}

vec2 th_vo_algo::getPlayerMovement(int dir)
{
	return DIRECTION_VEL[dir] * (FOCUSED_DIR[dir] ? playerFocVel : playerVel);
}

float th_vo_algo::minStaticCollideTick(
	const std::vector<entity> &bullets,
	const vec2 &p, const vec2 &s,
	std::vector<entity> &collided) const
{
	float minTick = FLT_MAX;
	for (const auto& bullet : bullets)
	{
		float colTick;
		colTick = vec2::willCollideAABB(
			p, bullet.position - bullet.size / 2,
			s, bullet.size,
			vec2(), bullet.velocity
		);

		if (colTick >= 0) {
			minTick = std::min(colTick, minTick);
			collided.push_back(bullet);
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
				float fadeCoeff = std::max(0.0f, std::min(1.0f, 1.0f / (colTick / MAX_FRAMES_TILL_COLLISION)));
				hsv col_hsv = { 0, fadeCoeff,  fadeCoeff };
				rgb col_rgb = hsv2rgb(col_hsv);
				cdraw::fillRect(
					th_param.GAME_X_OFFSET + colDomains[i].x,
					th_param.GAME_Y_OFFSET + colDomains[i].y,
					sqsz.x, sqsz.y,
					D3DCOLOR_ARGB((int)(fadeCoeff * 128),
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

		if (this->renderVectorField) 
		{
			// draw vector field (laggy)
			vizPotentialQuadtree(
				player->bullets,
				vec2(), vec2(th_param.GAME_WIDTH, th_param.GAME_HEIGHT),
				VEC_FIELD_MIN_RESOLUTION);
		}

		// draw laser points
		for (auto i = player->lasers.begin(); i != player->lasers.end(); ++i)
		{
			cdraw::fillRect(
				th_param.GAME_X_OFFSET + i->position.x - 3,
				th_param.GAME_Y_OFFSET + i->position.y - 3,
				6, 6,
				D3DCOLOR_ARGB(255, 0, 0, 255));

			vec2 proj = (*i).position + (*i).velocity * 10;
			cdraw::line((*i).position.x + th_param.GAME_X_OFFSET, (*i).position.y + th_param.GAME_Y_OFFSET,
				proj.x + th_param.GAME_X_OFFSET, proj.y + th_param.GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 255, 0, 0));

			// voodoo witchcraft magic

			float rex = i->radius * (float)cos(M_PI / 2 + i->angle);
			float rey = i->radius * (float)sin(M_PI / 2 + i->angle);
			cdraw::line(
				th_param.GAME_X_OFFSET + i->position.x - rex,
				th_param.GAME_Y_OFFSET + i->position.y - rey,
				th_param.GAME_X_OFFSET + i->position.x + i->extent.x - rex,
				th_param.GAME_Y_OFFSET + i->position.y + i->extent.y - rey,
				D3DCOLOR_ARGB(255, 0, 0, 255));
			cdraw::line(
				th_param.GAME_X_OFFSET + i->position.x + rex,
				th_param.GAME_Y_OFFSET + i->position.y + rey,
				th_param.GAME_X_OFFSET + i->position.x + i->extent.x + rex,
				th_param.GAME_Y_OFFSET + i->position.y + i->extent.y + rey,
				D3DCOLOR_ARGB(255, 0, 0, 255));
		}

		// bullet markers
		for (auto i = player->bullets.begin(); i != player->bullets.end(); ++i)
		{
			if ((*i).meta)
			{
				cdraw::rect((*i).position.x - 7 + th_param.GAME_X_OFFSET, (*i).position.y - 7 + th_param.GAME_Y_OFFSET, 14, 14, D3DCOLOR_HSV((double)(16 * (*i).meta), 1, 1)));
			}
			else {
				cdraw::rect(
					(*i).position.x - (*i).size.x / 2 + th_param.GAME_X_OFFSET,
					(*i).position.y - (*i).size.y / 2 + th_param.GAME_Y_OFFSET,
					(*i).size.x, (*i).size.y, D3DCOLOR_ARGB(255, 255, 2, 200));
			}
			vec2 proj = (*i).position + (*i).velocity * 10;

			cdraw::line((*i).position.x + th_param.GAME_X_OFFSET, (*i).position.y + th_param.GAME_Y_OFFSET,
				proj.x + th_param.GAME_X_OFFSET, proj.y + th_param.GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
		}

		for (auto b : player->powerups)
		{
			cdraw::rect(
				th_param.GAME_X_OFFSET + b.position.x - b.size.x / 2,
				th_param.GAME_Y_OFFSET + b.position.y - b.size.y / 2,
				b.size.x, b.size.y, D3DCOLOR_ARGB(255, 255, 0, 0));
			vec2 proj = b.position + b.velocity * 10;

			cdraw::line(b.position.x + th_param.GAME_X_OFFSET, b.position.y + th_param.GAME_Y_OFFSET,
				proj.x + th_param.GAME_X_OFFSET, proj.y + th_param.GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
		}
		cdraw::fillRect(
			plyr.position.x - plyr.size.x / 2 + th_param.GAME_X_OFFSET,
			plyr.position.y - plyr.size.y / 2 + th_param.GAME_Y_OFFSET,
			plyr.size.x, plyr.size.y, D3DCOLOR_ARGB(255, 0, 255, 0));
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
		calibStartX = plyr.position.x;
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
			|| dynamic_cast<th10_player*>(player)
			|| dynamic_cast<th11_player*>(player))
			playerVel = plyr.position.x - calibStartX;
		else
			playerVel = calibStartX - plyr.position.x;
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
		calibStartX = plyr.position.x;
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
			|| dynamic_cast<th10_player*>(player)
			|| dynamic_cast<th11_player*>(player))
			playerFocVel = plyr.position.x - calibStartX;
		else
			playerFocVel = calibStartX - plyr.position.x;
		return true;
	}
	++calibFrames;
	return false;
}





