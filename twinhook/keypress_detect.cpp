#include "stdafx.h"
#include "keypress_detect.h"

std::vector<BYTE> keypress_detect::tick(BYTE state[256])
{
	std::vector<BYTE> pressed;
	for(int i = 0; i < 256; ++i)
	{
		if (!last_state[i] && state[i])
			pressed.push_back(i);
	}
	memcpy(last_state, state, 256);
	return pressed;
}
