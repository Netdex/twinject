#include "stdafx.h"
#include "th_ann_algo.h"
#include "th_player.h"
#include "th_di8_hook.h"

void th_ann_algo::on_begin()
{
}

void th_ann_algo::on_tick()
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

	entity plyr = player->get_plyr_cz();

	if (this->is_sampling)
	{
		// find SAMPLE_SIZE closest bullets to the player (by center, not by crit point!)
		// we should probably sort by crit point, but it's too inefficient
		auto comp = [&](const entity &a, const entity &b)
		{
			return (a.p - plyr.p).lensq() < (b.p - plyr.p).lensq();
		};
		std::vector<entity> partsort(SAMPLE_SIZE);
		std::partial_sort_copy(player->bullets.begin(), player->bullets.end(),
			partsort.begin(), partsort.end(), comp);

		sample samp;
		for (auto i = partsort.begin(); i != partsort.end(); ++i)
		{
			vec2 crit;
			if (hit_circle)
			{
				crit = vec2::closest_point_on_circle(i->p, i->sz.x, plyr.p);
			}
			else
			{
				// TODO implement this for AABBs
			}

			vec2 rel = crit - plyr.p;
			

		}
	}
	if (!player->enabled)
		return;
}

void th_ann_algo::visualize(IDirect3DDevice9* d3dDev)
{
}

void th_ann_algo::start_sampling()
{
	samples.clear();
}

void th_ann_algo::stop_sampling()
{
	save_samples(SAMPLE_FILE);
}

bool th_ann_algo::save_samples(std::string filename)
{
}

bool th_ann_algo::load_samples(std::string filename)
{
}
