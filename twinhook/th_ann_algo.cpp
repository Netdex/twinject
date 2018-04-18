#include "stdafx.h"
#include "th_ann_algo.h"
#include "th_player.h"
#include "th_di8_hook.h"
#include "cdraw.h"

void th_ann_algo::on_begin()
{
}

void th_ann_algo::on_tick()
{
	auto di8 = th_di8_hook::inst();
	entity plyr = player->get_plyr_ent();

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
		samp.keys = player->get_kbd_state();
		for (int i = 0; i < SAMPLE_SIZE; ++i)
		{
			vec2 crit;
			if (hit_circle)
			{
				crit = vec2::closest_point_on_circle(partsort[i].p, partsort[i].sz.x, plyr.p);
			}
			else
			{
				// TODO implement this for AABBs
			}

			vec2 rel = crit - plyr.p;
			samp.critpt[i] = rel;
			samp.ptvel[i] = partsort[i].v;
		}
		samples.push_back(samp);
	}
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

}

void th_ann_algo::visualize(IDirect3DDevice9* d3dDev)
{
	char buf[256];
	sprintf_s(buf, 256, "samples: %d", samples.size());
	cdraw::text(buf, D3DCOLOR_ARGB(255,0,255,0),
		10, 10,
		(int)th_param.WINDOW_WIDTH, (int)th_param.WINDOW_HEIGHT);
}

void th_ann_algo::handle_input(const BYTE diKeys[256], const BYTE press[256])
{
	if(press[DIK_T])
		set_sampling(!is_sampling);
	if(press[DIK_Y])
		save_samples(SAMPLE_FILE);
	if (press[DIK_U])
		load_samples(SAMPLE_FILE);
}

void th_ann_algo::set_sampling(bool sampling)
{
	is_sampling = sampling;
	if(sampling)
	{
		LOG("sampling started");
		samples.clear();
		is_sampling = true;
	}
	else
	{
		LOG("sampling stopped");
		is_sampling = false;
	}
}

bool th_ann_algo::save_samples(std::string filename)
{
	LOG("saving samples to %s", filename.c_str());
	std::ofstream ofs(filename, std::ios::out | std::ios::binary);
	if (ofs.fail())
		return false;
	for(auto i = samples.begin(); i != samples.end(); ++i)
	{
		// Yes, struct padding and unused inputs cause a lot of 
		// wasted space and incompatibility issues, but I'd rather 
		// make the save/load operations as fast as possible than have the 
		// sample file as small as possible, since you can compress it 
		// for sharing anyways
		ofs.write((char*)&*i, sizeof(sample));	// this may be the most disgusting looking cast
	}
	ofs.close();
	LOG("samples saved");
	return true;
}

bool th_ann_algo::load_samples(std::string filename)
{
	LOG("loading samples from %s", filename.c_str());
	std::ifstream ifs(filename, std::ios::in | std::ios::binary);
	if (ifs.fail())
		return false;
	samples.clear();
	sample samp;
	while(ifs.read((char*)&samp, sizeof(sample)))	// this is also disgusting
	{
		samples.push_back(samp);
	}
	return true;
}
