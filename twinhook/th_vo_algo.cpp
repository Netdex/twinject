#include "stdafx.h"
#include "th_vo_algo.h"
#include "th_player.h"
#include "th_di8_hook.h"
#include "th_config.h"



void th_vo_algo::on_begin()
{
	calibration_init();
}

void th_vo_algo::on_tick()
{
	if (!player->enabled)
		return;

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
	float collisionTicks[9] = { FLT_MAX };

	// Bullet collision frame calculations
	for (auto bullet = player->bullets.begin(); bullet != player->bullets.end(); ++bullet)
	{
		// TODO discover diagonal velocities for dir < 9
		for (int dir = 0; dir < 5; ++dir)
		{
			int colTick = vec2::will_collide_aabb(
				pos - th_param::PLAYER_SIZE / 2,
				bullet->p - bullet->sz / 2,
				th_param::PLAYER_SIZE,
				bullet->sz,
				direction_vel[dir] * player_vel,
				bullet->v
			);
			if (colTick >= 0)
				collisionTicks[dir] = min(colTick, collisionTicks[dir]);
		}
	}

	// Wall collision frame calculations

	/*
	 * Offset of player from wall
	 * [0]: Top, [1]: Bottom, [2]: Left, [3]: Right
	 */
	float wallDist[] = {
		-pos.y, th_param::GAME_HEIGHT - pos.y, -pos.x, th_param::GAME_WIDTH - pos.x
	};

	// direction the wall is towards relative to the player
	bool xDependant[] = { false, false, true, true };

	// TODO discover diagonal velocities for dir < 9
	for (int dir = 0; dir < 5; ++dir)
	{
		vec2 pvel = direction_vel[dir] * player_vel;
		for (int wall = 0; wall < 4; ++wall)
		{
			// warning: division by zero
			float t = wallDist[wall] / (xDependant[wall] ? pvel.x : pvel.y);
			if (isfinite(t) && t >= 0)
				collisionTicks[dir] = min(t, collisionTicks[dir]);
		}
	}

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

	auto di8 = th_di8_hook::inst();
	
	// release all control keys
	for (int i = 0; i < 4; ++i)
		di8->reset_vk_state(ctrl_keys[i]);

	// press required keys for moving in desired direction
	for (int i = 0; i < 2; ++i)
		if (dir_keys[minIdx][i])
			di8->set_vk_state(ctrl_keys[i], DIK_KEY_DOWN);
}

void th_vo_algo::calibration_init()
{
	is_calibrated = false;
	cal_frames = 0;
	cal_start_x = -1;
	player_vel = 0;
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

void th_vo_algo::visualize(IDirect3DDevice9* d3dDev)
{
}
