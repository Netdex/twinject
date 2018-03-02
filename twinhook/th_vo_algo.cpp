#include "stdafx.h"
#include "th_vo_algo.h"
#include "th_player.h"
#include "th_di8_hook.h"
#include "th_config.h"
#include "cdraw.h"
#include "color.h"


void th_vo_algo::on_begin()
{
	calibration_init();
}

void th_vo_algo::on_tick()
{
	auto di8 = th_di8_hook::inst();

	if (!player->enabled) {
		di8->reset_vk_state(DIK_LEFT);
		di8->reset_vk_state(DIK_RIGHT);
		di8->reset_vk_state(DIK_UP);
		di8->reset_vk_state(DIK_DOWN);
		di8->reset_vk_state(DIK_Z);
		di8->reset_vk_state(DIK_LSHIFT);
		di8->reset_vk_state(DIK_LCONTROL);
		return;
	}

	if (!is_calibrated)
	{
		is_calibrated = calibration_tick();
		if (is_calibrated)
			LOG("calibrated plyr vel: %f", player_vel);
		return;
	}

	vec2 pos = player->get_plyr_loc();

	/*
	 * Ticks until collision whilst moving in this direction
	 * Uses same direction numbering schema
	 */
	float collisionTicks[9];
	std::fill_n(collisionTicks, 9, FLT_MAX);

	// Bullet collision frame calculations
	for (auto bullet = player->bullets.begin(); bullet != player->bullets.end(); ++bullet)
	{
		for (int dir = 0; dir < 9; ++dir)
		{
			float colTick = vec2::will_collide_aabb(
				pos - th_param::PLAYER_SIZE / 2,
				bullet->p - bullet->sz / 2,
				th_param::PLAYER_SIZE,
				bullet->sz,
				direction_vel[dir] * player_vel,
				bullet->v
			);
			if (colTick >= 0) {
				collisionTicks[dir] = min(colTick, collisionTicks[dir]);
			}
		}
	}

	// Powerup collision frame calculations
	/*
	 * Ticks until collision with target whilst moving in this diretion
	 * Uses same direction numbering schema
	 */
	float targetTicks[9];
	std::fill_n(targetTicks, 9, FLT_MAX);

	vec2 ppsz(10, 10);
	for (auto powerup = player->powerups.begin(); powerup != player->powerups.end(); ++powerup)
	{
		// filter out unwanted powerups
		if (powerup->me == 0 && powerup->p.y > 200) {
			for (int dir = 0; dir < 9; ++dir)
			{
				float colTick = vec2::will_collide_aabb(
					pos - th_param::PLAYER_SIZE / 2,
					powerup->p - ppsz / 2,
					th_param::PLAYER_SIZE,
					ppsz,					// guess to powerup size, because we don't know
					direction_vel[dir] * player_vel,
					powerup->v
				);
				if (colTick >= 0) {
					targetTicks[dir] = min(colTick, targetTicks[dir]);
				}
			}
		}
	}

	// Wall collision frame calculations
	for (int dir = 0; dir < 9; ++dir)
	{
		vec2 pvel = direction_vel[dir] * player_vel;
		float t = vec2::will_exit_aabb(
			vec2(0, 0), pos - th_param::PLAYER_SIZE / 2,
			vec2(th_param::GAME_WIDTH, th_param::GAME_HEIGHT), th_param::PLAYER_SIZE,
			vec2(), pvel
		);
		if (t >= 0)
			collisionTicks[dir] = min(t, collisionTicks[dir]);
	}


	// Viable target calculations
	int tarIdx = -1;
	for(int dir = 0; dir < 9; ++dir)
	{
		if (targetTicks[dir] < collisionTicks[dir] 
			&& (tarIdx == -1 || targetTicks[dir] < targetTicks[tarIdx]))
			tarIdx = dir;
	}

	// check if we could find a targetable powerup
	// TODO do not target powerups that will go off the screen
	// TODO do not target powerups that are too high
	if (tarIdx == -1) {
		// find direction with maximum frames until collision
		int minIdx = 0;
		for (int dir = 1; dir < 9; ++dir)
		{
			if (collisionTicks[dir] != FLT_MAX &&
				collisionTicks[dir] > collisionTicks[minIdx])
			{
				minIdx = dir;
			}
		}
		tarIdx = minIdx;
	}

	/*
	LOG("C[%d] | H:%.0f U:%.0f D:%.0f L:%.0f R:%.0f UL:%.0f UR:%.0f DL:%.0f DR:%.0f",
		tarIdx,
		collisionTicks[0] == FLT_MAX ? -1 : collisionTicks[0], collisionTicks[1],
		collisionTicks[2], collisionTicks[3], collisionTicks[4],
		collisionTicks[5], collisionTicks[6], collisionTicks[7], collisionTicks[8]
	);*/

	di8->set_vk_state(DIK_Z, DIK_KEY_DOWN);			// fire continuously
	di8->set_vk_state(DIK_LCONTROL, DIK_KEY_DOWN);	// skip dialogue continuously

	// release all control keys
	for (int i = 0; i < 4; ++i)
		di8->reset_vk_state(ctrl_keys[i]);

	// press required keys for moving in desired direction
	for (int i = 0; i < 2; ++i)
		if (dir_keys[tarIdx][i])
			di8->set_vk_state(dir_keys[tarIdx][i], DIK_KEY_DOWN);
}

void th_vo_algo::calibration_init()
{
	is_calibrated = false;
	cal_frames = 0;
	cal_start_x = -1;
	player_vel = 0;
}

void th_vo_algo::visualize(IDirect3DDevice9* d3dDev)
{
	vec2 pos = player->get_plyr_loc();
	if (player->render)
	{
		// bullet markers
		for (auto i = player->bullets.begin(); i != player->bullets.end(); ++i)
		{
			if ((*i).me)
			{
				cdraw::rect((*i).p.x - 7 + th_param::GAME_X_OFFSET, (*i).p.y - 7 + th_param::GAME_Y_OFFSET, 14, 14, D3DCOLOR_HSV((double)(16 * (*i).me), 1, 1)));
			}
			else {
				cdraw::rect((*i).p.x - (*i).sz.x / 2 + th_param::GAME_X_OFFSET, (*i).p.y - (*i).sz.x / 2 + th_param::GAME_Y_OFFSET, (*i).sz.x, (*i).sz.y, D3DCOLOR_ARGB(255, 255, 2, 200));
			}
			vec2 proj = (*i).p + (*i).v * 10;

			cdraw::line((*i).p.x + th_param::GAME_X_OFFSET, (*i).p.y + th_param::GAME_Y_OFFSET,
				proj.x + th_param::GAME_X_OFFSET, proj.y + th_param::GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
		}

		/*for(auto i = player->powerups.begin(); i != player->powerups.end(); ++i)
		{
			char buf[16];
			sprintf_s(buf, 16, "%d", i->me);
			cdraw::text(buf, D3DCOLOR_ARGB(255, 255, 255, 255), i->p.x, i->p.y, 700, 700);
		}*/

		cdraw::line(th_param::GAME_X_OFFSET, pos.y + th_param::GAME_Y_OFFSET,
			th_param::GAME_WIDTH + th_param::GAME_X_OFFSET, pos.y + th_param::GAME_Y_OFFSET,
			D3DCOLOR_ARGB(255, 0, 255, 0));
		cdraw::line(pos.x + th_param::GAME_X_OFFSET, th_param::GAME_Y_OFFSET,
			pos.x + th_param::GAME_X_OFFSET, th_param::GAME_HEIGHT + th_param::GAME_Y_OFFSET,
			D3DCOLOR_ARGB(255, 0, 255, 0));

		cdraw::fill_rect(pos.x - 2 + th_param::GAME_X_OFFSET, pos.y - 2 + th_param::GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 0, 255, 0));
	}


}

bool th_vo_algo::calibration_tick()
{
	vec2 pos = player->get_plyr_loc();
	switch (cal_frames)
	{
	case 0:
		// do not allow player interaction during calibration
		th_di8_hook::inst()->set_vk_state(DIK_LEFT, DIK_KEY_DOWN);
		th_di8_hook::inst()->set_vk_state(DIK_RIGHT, DIK_KEY_UP);
		th_di8_hook::inst()->set_vk_state(DIK_UP, DIK_KEY_UP);
		th_di8_hook::inst()->set_vk_state(DIK_DOWN, DIK_KEY_UP);
		break;
	case 1:
		th_di8_hook::inst()->set_vk_state(DIK_LEFT, DIK_KEY_UP);
		cal_start_x = pos.x;
		break;
	case 2:
		th_di8_hook::inst()->set_vk_state(DIK_RIGHT, DIK_KEY_DOWN);
		break;
	case 3:
		is_calibrated = true;
		th_di8_hook::inst()->reset_vk_state(DIK_LEFT);
		th_di8_hook::inst()->reset_vk_state(DIK_RIGHT);
		th_di8_hook::inst()->reset_vk_state(DIK_UP);
		th_di8_hook::inst()->reset_vk_state(DIK_DOWN);
		player_vel = cal_start_x - pos.x;
		return true;
	}
	++cal_frames;
	return false;
}


