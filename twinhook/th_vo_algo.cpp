#include "stdafx.h"
#include "th_vo_algo.h"
#include "th_player.h"
#include "th_di8_hook.h"


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
		if(is_calibrated)
			LOG("calibrated plyr vel: %f", player_vel);
	}
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
