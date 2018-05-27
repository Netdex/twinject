//#include "stdafx.h"
//#include "th_ann_algo.h"
//#include "th_player.h"
//#include "th_di8_hook.h"
//#include "util/cdraw.h"
//
//void th_ann_algo::on_begin()
//{
//	nnetwork.load(NNET_FILE);
//	LOG("loaded network from %s", NNET_FILE);
//}
//
//void th_ann_algo::on_tick()
//{
//	auto di8 = th_di8_hook::inst();
//	entity plyr = player->get_plyr_ent();
//
//	if (this->is_sampling)
//	{
//		// find SAMPLE_SIZE closest bullets to the player (by center, not by crit point!)
//		// we should probably sort by crit point, but it's too inefficient
//		sample currentSample = do_sample();
//		samples.push_back(currentSample);
//	}
//	if (!player->enabled) {
//		di8->reset_vk_state(DIK_LEFT);
//		di8->reset_vk_state(DIK_RIGHT);
//		di8->reset_vk_state(DIK_UP);
//		di8->reset_vk_state(DIK_DOWN);
//		di8->reset_vk_state(DIK_Z);
//		di8->reset_vk_state(DIK_LSHIFT);
//		di8->reset_vk_state(DIK_LCONTROL);
//		return;
//	}
//
//	sample currentSample = do_sample();
//	tiny_dnn::vec_t input = sample_to_imat(currentSample);
//	tiny_dnn::vec_t output = nnetwork.predict(input);
//
//	char buf[256];
//	sprintf_s(buf, 256, "%f %f %f %f %f", output[0], output[1], output[2], output[3], output[4]);
//	LOG("%s", buf);
//
//	BYTE keys[] = { DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_LSHIFT };
//	for (int i = 0; i < 5; ++i)
//	{
//		if (output[i] > 0.2f)
//		{
//			di8->set_vk_state(keys[i], 0x80);
//		}
//		else
//		{
//			di8->set_vk_state(keys[i], 0x0);
//		}
//	}
//
//
//	di8->set_vk_state(DIK_Z, 0x80);
//}
//
//void th_ann_algo::visualize(IDirect3DDevice9* d3dDev)
//{
//	char buf[256];
//	sprintf_s(buf, 256, "samples: %d", samples.size());
//	cdraw::text(buf, D3DCOLOR_ARGB(255, 0, 255, 0),
//		10, 10,
//		(int)th_param.WINDOW_WIDTH, (int)th_param.WINDOW_HEIGHT);
//}
//
//void th_ann_algo::handle_input(const BYTE diKeys[256], const BYTE press[256])
//{
//	if (press[DIK_T])
//		set_sampling(!is_sampling);
//	if (press[DIK_I])
//		samples.clear();
//	if (press[DIK_Y])
//		save_samples(SAMPLE_FILE);
//	if (press[DIK_U])
//		load_samples(SAMPLE_FILE);
//}
//
//void th_ann_algo::set_sampling(bool sampling)
//{
//	is_sampling = sampling;
//	if (sampling)
//	{
//		LOG("sampling started");
//		is_sampling = true;
//	}
//	else
//	{
//		LOG("sampling stopped");
//		is_sampling = false;
//	}
//}
//
//bool th_ann_algo::save_samples(std::string filename)
//{
//	if (is_sampling)
//		return false;
//	LOG("saving samples to %s", filename.c_str());
//	std::ofstream ofs(filename, std::ios::out | std::ios::binary);
//	ASSERT(!ofs.fail());
//	for (auto i = samples.begin(); i != samples.end(); ++i)
//	{
//		ofs.write((char*)&i->sampleCount, sizeof(int));
//		for (int j = 0; j < i->sampleCount; ++j)
//		{
//			ofs.write((char*)&i->pos[j].x, sizeof(float));
//			ofs.write((char*)&i->pos[j].y, sizeof(float));
//			ofs.write((char*)&i->vel[j].x, sizeof(float));
//			ofs.write((char*)&i->vel[j].y, sizeof(float));
//		}
//		uint8_t keyu8 = kbd_state_to_u8(i->keys);
//		ofs.write((char*)&keyu8, sizeof(uint8_t));
//	}
//	ofs.close();
//	LOG("samples saved");
//	return true;
//}
//
//bool th_ann_algo::load_samples(std::string filename)
//{
//	if (is_sampling)
//		return false;
//	LOG("loading samples from %s", filename.c_str());
//	std::ifstream ifs(filename, std::ios::in | std::ios::binary);
//	ASSERT(!ifs.fail());
//	samples.clear();
//	int sampleCount;
//	while (ifs.read((char*)&sampleCount, sizeof(int)))	// this is also disgusting
//	{
//		sample samp;
//		samp.sampleCount = sampleCount;
//		for (int i = 0; i < sampleCount; ++i)
//		{
//			vec2 p, v;
//			ifs.read((char*)&p.x, sizeof(float));
//			ifs.read((char*)&p.y, sizeof(float));
//			ifs.read((char*)&v.x, sizeof(float));
//			ifs.read((char*)&v.y, sizeof(float));
//			samp.pos.push_back(p);
//			samp.vel.push_back(v);
//		}
//		uint8_t keyu8;
//		ifs.read((char*)&keyu8, sizeof(uint8_t));
//		samp.keys = u8_to_kbd_state(keyu8);
//		samples.push_back(samp);
//	}
//	return true;
//}
//
//th_ann_algo::sample th_ann_algo::do_sample()
//{
//	entity plyr = player->get_plyr_ent();
//
//	auto comp = [&](const entity &a, const entity &b)
//	{
//		return (a.p - plyr.p).lensq() < (b.p - plyr.p).lensq();
//	};
//
//	int sampleCount = std::min(SAMPLE_SIZE, (int)player->bullets.size());
//	std::vector<entity> sortedSamples(sampleCount);
//	std::partial_sort_copy(player->bullets.begin(), player->bullets.end(),
//		sortedSamples.begin(), sortedSamples.end(), comp);
//
//	sample currentSample;
//	currentSample.sampleCount = sampleCount;
//	currentSample.keys = player->get_kbd_state();
//	for (int i = 0; i < sampleCount; ++i)
//	{
//		vec2 crit;
//		if (hit_circle)
//		{
//			crit = vec2::closest_point_on_circle(sortedSamples[i].p, sortedSamples[i].sz.x, plyr.p);
//		}
//		else
//		{
//			// TODO implement this for AABBs
//		}
//
//		vec2 rel = crit - plyr.p;
//		currentSample.pos.push_back(rel);
//		currentSample.vel.push_back(sortedSamples[i].v);
//	}
//	return currentSample;
//}
//
//tiny_dnn::vec_t th_ann_algo::sample_to_imat(const sample &i)
//{
//	tiny_dnn::vec_t inputMat;
//	for (int j = 0; j < SAMPLE_SIZE; ++j)
//	{
//		if (j < i.sampleCount)
//		{
//			inputMat.push_back(i.pos[j].x);
//			inputMat.push_back(i.pos[j].y);
//			inputMat.push_back(i.vel[j].x);
//			inputMat.push_back(i.vel[j].y);
//		}
//		else
//		{
//			inputMat.push_back(0);
//			inputMat.push_back(0);
//			inputMat.push_back(0);
//			inputMat.push_back(0);
//		}
//	}
//	return inputMat;
//}
//
//uint8_t th_ann_algo::kbd_state_to_u8(th_kbd_state s)
//{
//	return (uint8_t)(
//		(s.keys[0] ? 1 : 0) << 0 |
//		(s.keys[1] ? 1 : 0) << 1 |
//		(s.keys[2] ? 1 : 0) << 2 |
//		(s.keys[3] ? 1 : 0) << 3 |
//		(s.keys[4] ? 1 : 0) << 4 |
//		(s.keys[5] ? 1 : 0) << 5 |
//		(s.keys[6] ? 1 : 0) << 6 |
//		(s.keys[7] ? 1 : 0) << 7
//		);
//}
//
//th_kbd_state th_ann_algo::u8_to_kbd_state(uint8_t s)
//{
//	return th_kbd_state{
//		(bool)(s & (1 << 0)),
//		(bool)(s & (1 << 1)),
//		(bool)(s & (1 << 2)),
//		(bool)(s & (1 << 3)),
//		(bool)(s & (1 << 4)),
//		(bool)(s & (1 << 5)),
//		(bool)(s & (1 << 6)),
//		(bool)(s & (1 << 7)),
//	};
//}