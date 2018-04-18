#include "stdafx.h"
#include "keypress_detect.h"

void keypress_detect::tick(const BYTE state[256], BYTE pressed[256])
{
	for(int i = 0; i < 256; ++i)
	{
		if (!last_state[i] && state[i])
			pressed[i] = 1;
		else
			pressed[i] = 0;
	}
	memcpy(last_state, state, 256);
}
