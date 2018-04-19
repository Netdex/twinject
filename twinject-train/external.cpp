#include "stdafx.h"
#include "external.h"

static th_kbd_state u8_to_kbd_state(uint8_t s)
{
	return th_kbd_state{
		(bool)(s & (1 << 0)),
		(bool)(s & (1 << 1)),
		(bool)(s & (1 << 2)),
		(bool)(s & (1 << 3)),
		(bool)(s & (1 << 4)),
		(bool)(s & (1 << 5)),
		(bool)(s & (1 << 6)),
		(bool)(s & (1 << 7)),
	};
}

std::vector<sample> load_samples(std::string filename)
{
	std::vector<sample> samples;
	std::ifstream ifs(filename, std::ios::in | std::ios::binary);
	if (ifs.fail())
		return samples;
	int sampleCount;
	while (ifs.read((char*)&sampleCount, sizeof(int)))
	{
		sample samp;
		samp.sampleCount = sampleCount;
		for (int i = 0; i < sampleCount; ++i)
		{
			vec2 p, v;
			ifs.read((char*)&p.x, sizeof(float));
			ifs.read((char*)&p.y, sizeof(float));
			ifs.read((char*)&v.x, sizeof(float));
			ifs.read((char*)&v.y, sizeof(float));
			samp.pos.push_back(p);
			samp.vel.push_back(v);
		}
		uint8_t keyu8;
		ifs.read((char*)&keyu8, sizeof(uint8_t));
		samp.keys = u8_to_kbd_state(keyu8);
		samples.push_back(samp);
	}
	return samples;
}